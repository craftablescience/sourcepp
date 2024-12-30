#include <vpkpp/format/PCK.h>

#include <filesystem>
#include <ranges>

#include <FileStream.h>
#include <sourcepp/crypto/MD5.h>

using namespace sourcepp;
using namespace vpkpp;

constexpr int PCK_DIRECTORY_STRING_PADDING = 4;
constexpr int PCK_FILE_DATA_PADDING = 16;

std::unique_ptr<PackFile> PCK::create(const std::string& path, uint32_t version, uint32_t godotMajorVersion, uint32_t godotMinorVersion, uint32_t godotPatchVersion) {
	if (version != 1 && version != 2) {
		return nullptr;
	}

	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};

		stream
			.write(PCK_SIGNATURE)
			.write(version)
			.write(godotMajorVersion)
			.write(godotMinorVersion)
			.write(godotPatchVersion);

		if (version > 1) {
			stream
				.write(FLAG_DIR_NONE)
				.write<uint64_t>(0);
		}

		stream
			.write(std::array<int32_t, 16>{})
			.write<uint32_t>(0);
	}
	return PCK::open(path);
}

std::unique_ptr<PackFile> PCK::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* pck = new PCK{path};
	auto packFile = std::unique_ptr<PackFile>(pck);

	FileStream reader{pck->fullFilePath};
	reader.seek_in(0);

	if (auto signature = reader.read<uint32_t>(); signature != PCK_SIGNATURE) {
		// PCK might be embedded
		reader.seek_in(sizeof(uint32_t), std::ios::end);
		if (auto endSignature = reader.read<uint32_t>(); endSignature != PCK_SIGNATURE) {
			return nullptr;
		}

		reader.seek_in(-static_cast<int64_t>(sizeof(uint32_t) + sizeof(uint64_t)), std::ios::cur);
		auto distanceIntoFile = reader.read<uint64_t>();

		reader.seek_in(-static_cast<int64_t>(distanceIntoFile + sizeof(uint64_t)), std::ios::cur);
		if (auto startSignature = reader.read<uint32_t>(); startSignature != PCK_SIGNATURE) {
			return nullptr;
		}

		pck->startOffset = reader.tell_in() - sizeof(uint32_t);
	}

	reader.read(pck->header.packVersion);
	reader.read(pck->header.godotVersionMajor);
	reader.read(pck->header.godotVersionMinor);
	reader.read(pck->header.godotVersionPatch);

	pck->header.flags = FLAG_DIR_NONE;
	std::size_t extraEntryContentsOffset = 0;
	if (pck->header.packVersion > 1) {
		pck->header.flags = reader.read<FlagsDirV2>();
		extraEntryContentsOffset = reader.read<uint64_t>();
	}

	if (pck->header.flags & FLAG_DIR_ENCRYPTED) {
		// File directory is encrypted
		return nullptr;
	}
	if (pck->header.flags & FLAG_DIR_RELATIVE_FILE_DATA) {
		extraEntryContentsOffset += pck->startOffset;
		pck->header.flags = static_cast<FlagsDirV2>(pck->header.flags & ~FLAG_DIR_RELATIVE_FILE_DATA);
	}

	// Reserved
	reader.skip_in<int32_t>(16);

	// Directory
	auto fileCount = reader.read<uint32_t>();
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		auto entryPath = pck->cleanEntryPath(reader.read_string(reader.read<uint32_t>()));
		if (entryPath.starts_with(PCK_PATH_PREFIX)) {
			entryPath = entryPath.substr(PCK_PATH_PREFIX.length());
		}

		entry.offset = reader.read<uint64_t>() + extraEntryContentsOffset;
		entry.length = reader.read<uint64_t>();
		entry.extraData = reader.read_bytes(16);

		if (pck->header.packVersion > 1) {
			entry.flags = reader.read<uint32_t>();
			if (entry.flags & FLAG_FILE_REMOVED) {
				continue;
			}
		}

		pck->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	// File data
	pck->dataOffset = reader.tell_in();

	return packFile;
}

std::optional<std::vector<std::byte>> PCK::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	if (entry->flags & FLAG_FILE_ENCRYPTED) {
		// File is encrypted
		return std::nullopt;
	}

	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

void PCK::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.length = buffer.size();

	const auto md5 = crypto::computeMD5(buffer);
	entry.extraData = {md5.begin(), md5.end()};

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool PCK::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	for (auto& [path, entry] : entriesToBake) {
		if (auto binData = this->readEntry(path)) {
			entry->offset = fileData.size();

			fileData.insert(fileData.end(), binData->begin(), binData->end());
			const auto padding = math::paddingForAlignment(PCK_FILE_DATA_PADDING, static_cast<int>(entry->length));
			for (int i = 0; i < padding; i++) {
				fileData.push_back(static_cast<std::byte>(0));
			}
		} else {
			entry->offset = 0;
			entry->length = 0;
		}
	}

	// If this is an embedded pck, read the executable data first
	std::vector<std::byte> exeData;
	if (this->startOffset > 0) {
		FileStream stream{this->fullFilePath};
		if (!stream) {
			return false;
		}
		stream.seek_in(0);
		exeData = stream.read_bytes(this->startOffset);
	}

	// Write data
	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		if (!exeData.empty()) {
			stream.write(exeData);
		}

		// Signature
		stream.write(PCK_SIGNATURE);

		// Header
		stream.write(this->header.packVersion);
		stream.write(this->header.godotVersionMajor);
		stream.write(this->header.godotVersionMinor);
		stream.write(this->header.godotVersionPatch);

		if (this->header.packVersion > 1) {
			stream.write(this->header.flags);
			stream.write<uint64_t>(0);
		}

		// Reserved
		stream.write(std::array<int32_t, 16>{});

		// Directory start
		stream.write(static_cast<uint32_t>(entriesToBake.size()));

		// Dry-run to get the length of the directory section
		this->dataOffset = stream.tell_out();
		for (const auto& path : std::views::keys(entriesToBake)) {
			const auto entryPath = std::string{PCK_PATH_PREFIX} + path;
			const auto padding = math::paddingForAlignment(PCK_DIRECTORY_STRING_PADDING, static_cast<int>(entryPath.length()));
			this->dataOffset +=
					sizeof(uint32_t) +             // Path length
					entryPath.length() + padding + // Path
					(sizeof(std::size_t) * 2) +    // Offset, Length
					(sizeof(std::byte) * 16);      // MD5

			if (this->header.packVersion > 1) {
				this->dataOffset += sizeof(uint32_t); // Flags
			}
		}

		// Directory
		for (const auto& [path, entry] : entriesToBake) {
			const auto entryPath = std::string{PCK_PATH_PREFIX} + path;
			const auto padding = math::paddingForAlignment(PCK_DIRECTORY_STRING_PADDING, static_cast<int>(entryPath.length()));
			stream.write(static_cast<uint32_t>(entryPath.length() + padding));
			stream.write(entryPath, false, entryPath.length() + padding);

			entry->offset += this->dataOffset;
			stream.write(entry->offset);
			stream.write(entry->length);
			stream.write(entry->extraData);

			if (this->header.packVersion > 1) {
				stream.write(entry->flags);
			}

			if (callback) {
				callback(path, *entry);
			}
		}

		// File data
		stream.write(fileData);

		// Write offset to start
		if (this->startOffset > 0) {
			stream.write(stream.tell_out() - startOffset);
			stream.write(PCK_SIGNATURE);
		}
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute PCK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | PCK_MD5;
}

PCK::operator std::string() const {
	auto out = PackFile::operator std::string() +
		" | Version v" + std::to_string(this->header.packVersion) +
		" | Godot Version v" + std::to_string(this->header.godotVersionMajor) + '.' + std::to_string(this->header.godotVersionMinor) + '.' + std::to_string(this->header.godotVersionPatch);
	if (this->startOffset > 0) {
		out += " | Embedded";
	}
	if (this->header.flags & FLAG_DIR_ENCRYPTED) {
		out += " | Encrypted";
	}
	return out;
}

uint32_t PCK::getVersion() const {
	return this->header.packVersion;
}

void PCK::setVersion(uint32_t version) {
	if (version == 1 || version == 2) {
		this->header.packVersion = version;
	}
}

std::tuple<uint32_t, uint32_t, uint32_t> PCK::getGodotVersion() const {
	return {this->header.godotVersionMajor, this->header.godotVersionMinor, this->header.godotVersionPatch};
}

void PCK::setGodotVersion(uint32_t major, uint32_t minor, uint32_t patch) {
	this->header.godotVersionMajor = major;
	this->header.godotVersionMinor = minor;
	this->header.godotVersionPatch = patch;
}
