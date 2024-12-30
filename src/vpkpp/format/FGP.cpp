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

// TODO:
// - weird file data in some maps, files unreadable?
// - check loading screen file path is pointing to vtf or raw image data in setter
// - format extension to use filenames instead of hashes (append filenames to the end)
//   - as long as the format extension is there a separate application to reverse the hashes can be added later

std::unique_ptr<PackFile> FGP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* fgp = new FGP{path};
	auto packFile = std::unique_ptr<PackFile>(fgp);

	auto fileData = fs::readFileBuffer(fgp->fullFilePath);
	BufferStream reader{fileData};
	reader.seek(0);

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
	fgp->headerSize = sizeof(uint32_t) * 4 * (fileCount + 1);

	const auto loadingScreen = reader.read<uint32_t>();

	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		// note: NOT a CRC32! check FGP::hashFilePath
		entry.crc32 = reader.read<uint32_t>();
		auto entryPath = fgp->cleanEntryPath("__hashed__/" + crypto::encodeHexString({reinterpret_cast<const std::byte*>(&entry.crc32), sizeof(entry.crc32)}));
		if (loadingScreen > 0 && i == loadingScreen) {
			fgp->loadingScreenPath = entryPath;
		}

		entry.offset = reader.read<uint32_t>();
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
	stream.seek_in_u(entry->offset + this->headerSize);
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

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	{
		FileStream stream{this->fullFilePath};
		for (const auto& [path, entry] : entriesToBake) {
			if (!entry->unbaked) {
				stream.seek_in_u(entry->offset + this->headerSize);
				const auto binData = stream.read_bytes(entry->compressedLength > 0 ? entry->compressedLength : entry->length);
				fileData.insert(fileData.end(), binData.begin(), binData.end());
			} else if (const auto binData = this->readEntry(path)) {
				entry->offset = fileData.size();
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
				.write<uint32_t>(entry->offset)
				.write<uint32_t>(entry->length)
				.write<uint32_t>(entry->compressedLength);

			if (callback) {
				callback(path, *entry);
			}

			i++;
		}
		this->headerSize = sizeof(uint32_t) * 4 * (entriesToBake.size() + 1);

		// File data
		stream.write(fileData);
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
