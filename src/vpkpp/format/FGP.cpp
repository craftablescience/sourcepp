#include <vpkpp/format/FGP.h>

#include <cctype>
#include <filesystem>
#include <numeric>

#include <BufferStream.h>
#include <FileStream.h>
#include <miniz.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vpkpp;

constexpr auto SOURCEPP_FILENAMES_SIGNATURE = parser::binary::makeEightCC("PPECRUOS");
constexpr std::string_view FILENAME_HASHED_PREFIX = "__hashed__/";

std::unique_ptr<PackFile> FGP::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write(FGP_SIGNATURE)
			.set_big_endian(true)
			.write<uint32_t>(3)
			.write<uint32_t>(0)
			.write<uint32_t>(0);
	}
	return FGP::open(path);
}

std::unique_ptr<PackFile> FGP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* fgp = new FGP{path};
	auto packFile = std::unique_ptr<PackFile>(fgp);

	FileStream reader{fgp->fullFilePath};
	reader.seek_in(0);

	if (reader.read<uint32_t>() != FGP_SIGNATURE) {
		// File is not an FGP
		return nullptr;
	}

	reader.set_big_endian(true);

	reader >> fgp->version;
	if (fgp->version != 3) {
		// Version 3 shipped with 1.00, and was never incremented post-release
		return nullptr;
	}

	const auto fileCount = reader.read<uint32_t>();
	const auto headerSize = sizeof(uint32_t) * 4 * (fileCount + 1);

	const auto loadingScreen = reader.read<uint32_t>();

	// Here we skip to the end and load in the filename map if it exists
	std::unordered_map<uint32_t, std::string> crackedHashes;
	const auto directoryPos = reader.tell_in();
	if (reader.seek_in_u(sizeof(uint64_t), std::ios::end).read<uint64_t>() == SOURCEPP_FILENAMES_SIGNATURE) {
		reader.seek_in_u(reader.seek_in(sizeof(uint64_t) * 2, std::ios::end).read<uint64_t>());
		if (reader.read<uint64_t>() == SOURCEPP_FILENAMES_SIGNATURE && reader.read<uint32_t>() == 1) {
			const auto filepathCount = reader.read<uint32_t>();
			for (int i = 0; i < filepathCount; i++) {
				const auto hash = reader.read<uint32_t>();
				crackedHashes[hash] = fgp->cleanEntryPath(reader.read_string(reader.read<uint16_t>()));
			}
		}
	}
	reader.seek_in_u(directoryPos);

	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		// note: NOT a CRC32! check FGP::hashFilePath
		entry.crc32 = reader.read<uint32_t>();

		std::string entryPath;
		if (crackedHashes.contains(entry.crc32)) {
			entryPath = crackedHashes[entry.crc32];
		} else {
			entryPath = fgp->cleanEntryPath(FILENAME_HASHED_PREFIX.data() + crypto::encodeHexString({reinterpret_cast<const std::byte*>(&entry.crc32), sizeof(entry.crc32)}));
		}
		if (loadingScreen > 0 && i == loadingScreen) {
			fgp->loadingScreenPath = entryPath;
		}

		entry.offset = reader.read<uint32_t>() + headerSize;
		entry.length = reader.read<uint32_t>();
		entry.compressedLength = reader.read<uint32_t>();

		fgp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> FGP::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return PackFile::readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	stream.seek_in_u(entry->offset);
	if (entry->compressedLength == 0) {
		return stream.read_bytes(entry->length);
	}

	// Decode
	const auto compressedData = stream.read_bytes(entry->compressedLength);
	std::vector<std::byte> data(entry->length);
	mz_ulong len = entry->length;
	if (mz_uncompress(reinterpret_cast<unsigned char*>(data.data()), &len, reinterpret_cast<const unsigned char*>(compressedData.data()), entry->compressedLength) != MZ_OK) {
		return std::nullopt;
	}
	return data;
}

void FGP::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	// note: NOT a CRC32! check FGP::hashFilePath
	entry.crc32 = FGP::hashFilePath(this->cleanEntryPath(path));
	entry.length = buffer.size();
	entry.compressedLength = 0;

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool FGP::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});
	const auto headerSize = sizeof(uint32_t) * 4 * (entriesToBake.size() + 1);

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	{
		FileStream stream{this->fullFilePath};
		for (const auto& [path, entry] : entriesToBake) {
			if (!entry->unbaked) {
				stream.seek_in_u(entry->offset);
				const auto binData = stream.read_bytes(entry->compressedLength > 0 ? entry->compressedLength : entry->length);
				entry->offset = headerSize + fileData.size();
				fileData.insert(fileData.end(), binData.begin(), binData.end());
			} else if (const auto binData = this->readEntry(path)) {
				entry->offset = headerSize + fileData.size();
				entry->length = binData->size();
				entry->compressedLength = 0;

				if (!options.zip_compressionStrength) {
					fileData.insert(fileData.end(), binData->begin(), binData->end());
				} else {
					mz_ulong compressedSize = mz_compressBound(binData->size());
					std::vector<std::byte> out(compressedSize);

					int status = MZ_OK;
					while ((status = mz_compress2(reinterpret_cast<unsigned char*>(out.data()), &compressedSize, reinterpret_cast<const unsigned char*>(binData->data()), binData->size(), options.zip_compressionStrength)) == MZ_BUF_ERROR) {
						compressedSize *= 2;
						out.resize(compressedSize);
					}

					if (status != MZ_OK) {
						fileData.insert(fileData.end(), binData->begin(), binData->end());
						continue;
					}
					out.resize(compressedSize);
					fileData.insert(fileData.end(), out.begin(), out.end());
					entry->compressedLength = compressedSize;
				}
			} else {
				entry->offset = 0;
				entry->length = 0;
				entry->compressedLength = 0;
			}
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		stream << FGP_SIGNATURE;
		stream.set_big_endian(true);

		stream
			.write<uint32_t>(this->version)
			.write<uint32_t>(entriesToBake.size());

		const auto loadingScreenPos = stream.tell_out();
		stream.write<uint32_t>(0);

		// Directory
		uint32_t i = 0;
		for (const auto& [path, entry] : entriesToBake) {
			if (path == this->loadingScreenPath) {
				const auto curPos = stream.tell_out();
				stream.seek_out_u(loadingScreenPos).write<uint32_t>(i).seek_out_u(curPos);
			}

			stream
				.write<uint32_t>(entry->crc32)
				.write<uint32_t>(entry->offset - headerSize)
				.write<uint32_t>(entry->length)
				.write<uint32_t>(entry->compressedLength);

			if (callback) {
				callback(path, *entry);
			}

			i++;
		}

		// File data
		stream.write(fileData);

		// Filename mappings
		const auto filenameMappingPos = stream.tell_out();
		stream
			.write(SOURCEPP_FILENAMES_SIGNATURE)
			.write<uint32_t>(1); // version
		const auto filepathCountPos = stream.tell_out();
		stream.write<uint32_t>(0);
		uint32_t filepathCount = 0;
		for (const auto& [path, entry] : entriesToBake) {
			if (path.starts_with(FILENAME_HASHED_PREFIX)) {
				continue;
			}
			stream
				.write<uint32_t>(entry->crc32)
				.write<uint16_t>(path.size())
				.write(path, false);
			filepathCount++;
		}
		stream
			.write<uint64_t>(filenameMappingPos)
			.write(SOURCEPP_FILENAMES_SIGNATURE)
			.seek_out_u(filepathCountPos)
			.write<uint32_t>(filepathCount);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute FGP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}

FGP::operator std::string() const {
	return PackFile::operator std::string() +
		" | Version v" + std::to_string(this->version);
}

std::string FGP::getLoadingScreenFilePath() const {
	return this->loadingScreenPath;
}

void FGP::setLoadingScreenFilePath(const std::string& path) {
	if (this->hasEntry(path)) {
		this->loadingScreenPath = this->cleanEntryPath(path);
	}
}

uint32_t FGP::hashFilePath(const std::string& filepath) {
	return std::accumulate(filepath.begin(), filepath.end(), 0xAAAAAAAAu, [](uint32_t hash, char c) { return (hash << 5) + hash + static_cast<uint8_t>(tolower(c)); });
}
