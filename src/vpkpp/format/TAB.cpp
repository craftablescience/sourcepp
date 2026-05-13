// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/TAB.h>

#include <filesystem>
#include <format>
#include <numeric>

#include <BufferStream.h>
#include <FileStream.h>
#include <tomcrypt.h>

using namespace sourcepp;
using namespace vpkpp;

namespace {

constexpr std::string_view TAB_FILEPATH_LIST_STRIP_PATH_INDEX = "projects/justcause/data/";

[[nodiscard]] std::filesystem::path getArchivePath(const TAB& tab, uint32_t archiveIndex) {
	return std::filesystem::path{tab.getFilepath()}.parent_path() / std::format("{}{}{}", tab.getFilestem(), archiveIndex, ARC_EXTENSION);
}

} // namespace

std::unique_ptr<PackFile> TAB::create(const std::string& path, Version version, uint32_t sectorSize) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.set_big_endian(version == Version::JC1_BE)
			.write<uint32_t>(3)
			.write<uint32_t>(sectorSize)
			.write<uint32_t>(0);
	}
	return TAB::open(path);
}

std::unique_ptr<PackFile> TAB::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* tab = new TAB{path};
	auto packFile = std::unique_ptr<PackFile>(tab);

	FileStream reader{tab->fullFilePath};
	reader.seek_in(0);

	if (auto version = reader.read<uint32_t>(); version == 3) {
		tab->version = Version::JC1_LE;
	} else {
		BufferStream::swap_endian(&version);
		if (version == 3) {
			tab->version = Version::JC1_BE;
		} else {
			return nullptr;
		}
	}

	reader >> tab->sectorSize >> tab->numArchives;

	std::vector<uint32_t> archiveAlignments(tab->numArchives);
	uint32_t alignment = 0;
	for (uint32_t i = 0; i < tab->numArchives; i++) {
		alignment += (static_cast<uint32_t>(std::filesystem::file_size(::getArchivePath(*tab, i))) + tab->sectorSize - 1) / tab->sectorSize;
		archiveAlignments[i] = alignment;
	}

	// Here we load in the filepath list if it exists
	std::unordered_map<uint32_t, std::string> crackedHashes;
	if (const std::filesystem::path mapPath{std::filesystem::path{tab->fullFilePath}.parent_path() / std::format("{}list.txt", tab->getFilestem())}; std::filesystem::exists(mapPath)) {
		std::ifstream mapFile{mapPath};
		std::string filepath;
		while (std::getline(mapFile, filepath)) {
			if (filepath.empty() || filepath.starts_with(':')) {
				continue;
			}
			string::trim(filepath);
			string::normalizeSlashes(filepath);
			string::toLower(filepath);
			if (const auto index = filepath.rfind(TAB_FILEPATH_LIST_STRIP_PATH_INDEX); index != std::string::npos) {
				filepath = filepath.substr(index + TAB_FILEPATH_LIST_STRIP_PATH_INDEX.size());
			}
			crackedHashes[TAB::hashFilePath(filepath)] = filepath;
		}
	}

	const auto fileCount = (std::filesystem::file_size(tab->fullFilePath) - sizeof(uint32_t) * 3) / (sizeof(uint32_t) * 3);
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		std::string entryPath;

		// note: NOT a CRC32! check TAB::hashFilePath
		entry.crc32 = reader.read<uint32_t>();
		if (crackedHashes.contains(entry.crc32)) {
			entryPath = tab->cleanEntryPath(crackedHashes[entry.crc32]);
		} else {
			entryPath = tab->cleanEntryPath(TAB_HASHED_FILEPATH_PREFIX.data() + string::encodeHex({reinterpret_cast<const std::byte*>(&entry.crc32), sizeof(entry.crc32)}));
		}

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();
		entry.archiveIndex = 0;

		for (int j = 0; j < tab->numArchives; j++) {
			if (entry.offset < archiveAlignments[j]) {
				entry.archiveIndex = j;
				break;
			}
		}
		if (entry.archiveIndex == 0) {
			entry.offset = (entry.offset * tab->sectorSize) % ARC_CHUNK_SIZE;
		} else {
			entry.offset = (entry.offset - archiveAlignments[entry.archiveIndex - 1]) * tab->sectorSize % ARC_CHUNK_SIZE;
		}

		tab->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> TAB::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{::getArchivePath(*this, entry->archiveIndex)};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

void TAB::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	// note: NOT a CRC32! check TAB::hashFilePath
	entry.crc32 = TAB::hashFilePath(path);
	entry.length = buffer.size();

	// These will be reset when it's baked
	entry.archiveIndex = this->numArchives + 1;
	entry.offset = 0;
}

bool TAB::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});
	std::ranges::sort(entriesToBake, [](const std::pair<std::string, Entry*>& lhs, const std::pair<std::string, Entry*>& rhs) {
		return lhs.second->crc32 < rhs.second->crc32;
	});

	// Cracked hash list
	const std::filesystem::path mapPath{std::filesystem::path{outputDir} / std::format("{}list.txt", this->getFilestem())};
	if (!std::filesystem::exists(mapPath)) {
		fs::writeFileText(mapPath, "");
	}
	std::ofstream mapStream{mapPath};

	// Open directory file
	FileStream outDir{outputPath, FileStream::OPT_READ | FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	outDir.seek_in(0);
	outDir.seek_out(0);
	outDir.set_big_endian(this->version == Version::JC1_BE);

	// Dummy header
	outDir.pad<uint32_t>(3);

	// Archive alignment setup, because this format is weird
	int32_t currentArchiveIndex = -1;
	uint64_t currentArchiveLength = 0;
	std::vector<uint32_t> archiveAlignments;
	uint32_t totalArchiveSectors = 0;
	const auto getNewArchivePath = [this, &outputDir](uint32_t archiveIndex, bool srcPath = true) {
		return std::filesystem::path{outputDir} / std::format("{}{}{}{}", this->getFilestem(), archiveIndex, ARC_EXTENSION, srcPath ? ".new" : "");
	};
	const auto getNewArchiveStream = [this, &currentArchiveIndex, &currentArchiveLength, &archiveAlignments, &totalArchiveSectors, &getNewArchivePath] {
		if (currentArchiveIndex >= 0) {
			totalArchiveSectors += (currentArchiveLength + this->sectorSize - 1) / this->sectorSize;
			archiveAlignments.push_back(totalArchiveSectors);
		}
		currentArchiveLength = 0;

		auto out = FileStream{getNewArchivePath(++currentArchiveIndex), FileStream::OPT_READ | FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		out.seek_in(0);
		out.seek_out(0);
		return out;
	};
	FileStream currentArchive = getNewArchiveStream();

	// File tree and data
	for (auto& [path, entry] : entriesToBake) {
		if (!path.starts_with(TAB_HASHED_FILEPATH_PREFIX)) {
			mapStream << path << '\n';
		}

		if (const auto data = this->readEntry(path)) {
			const uint16_t padLength = math::paddingForAlignment(this->sectorSize, data->size());
			uint64_t entryTotalSize = data->size() + padLength;
			if (currentArchiveLength + entryTotalSize > ARC_CHUNK_SIZE) {
				currentArchive = getNewArchiveStream();
			}

			entry->archiveIndex = currentArchiveIndex;
			entry->offset = currentArchive.tell_out();

			currentArchive
				.write(*data)
				.pad<char>(padLength, 'P');

			outDir
				.write<uint32_t>(entry->crc32)
				.write<uint32_t>(currentArchiveIndex == 0
					? currentArchiveLength / this->sectorSize
					: archiveAlignments[currentArchiveIndex - 1] + currentArchiveLength / this->sectorSize)
				.write<uint32_t>(entry->length);

			currentArchiveLength += entryTotalSize;
		} else {
			entry->archiveIndex = 0;
			entry->offset = 0;
			entry->length = 0;
		}
	}

	// Write header
	this->numArchives = currentArchiveIndex + 1;
	outDir
		.seek_out(0)
		.write<uint32_t>(3)
		.write<uint32_t>(this->sectorSize)
		.write<uint32_t>(this->numArchives);

	// Rename new archives
	for (uint32_t i = 0; i < this->numArchives; i++) {
		const auto srcPath = getNewArchivePath(i);
		const auto destPath = getNewArchivePath(i, false);
		if (std::filesystem::exists(destPath)) {
			std::filesystem::remove(destPath);
		}
		std::filesystem::rename(srcPath, destPath);
	}

	// Merge unbaked into baked entries
	this->mergeUnbakedEntries();

	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute TAB::getSupportedEntryAttributes() const {
	using enum Attribute;
	return ARCHIVE_INDEX | LENGTH;
}

TAB::operator std::string() const {
	return PackFile::operator std::string() + std::format(" | {}", this->version == Version::JC1_LE ? "JC1 LE" : "JC1 BE");
}

TAB::Version TAB::getVersion() const {
	return this->version;
}

void TAB::setVersion(Version version_) {
	this->version = version_;
}

uint32_t TAB::getSectorSize() const {
	return this->sectorSize;
}

void TAB::setSectorSize(uint32_t sectorSize_) {
	this->sectorSize = sectorSize_;
}

uint32_t TAB::hashFilePath(const std::string& filepath) {
	auto cleanPath = filepath;
	string::normalizeSlashes(cleanPath);
	string::toLower(cleanPath);
	cleanPath = string::trim(std::filesystem::path{cleanPath}.filename().string());

	std::vector<std::byte> buffer;
	buffer.resize(TAB_FILENAME_MAX_SIZE);
	std::memset(buffer.data(), 0, TAB_FILENAME_MAX_SIZE);
	std::memcpy(buffer.data(), cleanPath.c_str(), cleanPath.size());

	buffer.push_back(std::byte{0x80});
	if (buffer.size() % 64 < 56) {
		buffer.resize(buffer.size() + (56 - buffer.size() % 64));
	} else if (buffer.size() % 64 > 56) {
		buffer.resize(buffer.size() + (64 - buffer.size() % 64) + 56);
	}
	for (int i = 7; i >= 0; i--) {
		buffer.push_back(static_cast<std::byte>(static_cast<uint64_t>(TAB_FILENAME_MAX_SIZE) * 8 >> i * 8 & 0xff));
	}

	for (std::span bufferU32{reinterpret_cast<uint32_t*>(buffer.data()), buffer.size() / sizeof(uint32_t)}; auto& uint : bufferU32) {
		BufferStream::swap_endian(&uint);
	}
	hash_state sha1;
	sha1_init(&sha1);
	sha1_process(&sha1, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());
	BufferStream::swap_endian(&sha1.sha1.state[0]);
	return sha1.sha1.state[0];
}
