#include <vpkpp/format/GMA.h>

#include <filesystem>

#include <FileStream.h>
#include <sourcepp/crypto/CRC32.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> GMA::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* gma = new GMA{path};
	auto packFile = std::unique_ptr<PackFile>(gma);

	FileStream reader{gma->fullFilePath};
	reader.seek_in(0);

	reader.read(gma->header.signature);
	if (gma->header.signature != GMA_SIGNATURE) {
		// File is not a GMA
		return nullptr;
	}
	reader.read(gma->header.version);
	reader.read(gma->header.steamID);
	reader.read(gma->header.timestamp);
	reader.read(gma->header.requiredContent);
	reader.read(gma->header.addonName);
	reader.read(gma->header.addonDescription);
	reader.read(gma->header.addonAuthor);
	reader.read(gma->header.addonVersion);

	std::vector<std::pair<std::string, Entry>> entries;
	while (reader.read<uint32_t>() > 0) {
		Entry entry = createNewEntry();

		auto entryPath = reader.read_string();

		entry.length = reader.read<uint64_t>();
		reader.read(entry.crc32);

		entries.emplace_back(entryPath, entry);
	}

	// At this point we've reached the file data section, calculate the offsets and then add the entries
	std::size_t offset = reader.tell_in();
	for (auto& [entryPath, entry] : entries) {
		entry.offset = offset;
		offset += entry.length;

		gma->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::vector<std::string> GMA::verifyEntryChecksums() const {
	return this->verifyEntryChecksumsUsingCRC32();
}

bool GMA::hasPackFileChecksum() const {
	return true;
}

bool GMA::verifyPackFileChecksum() const {
	auto data = fs::readFileBuffer(this->fullFilePath);
	if (data.size() <= 4) {
		return true;
	}

	const auto checksum = *(reinterpret_cast<uint32_t*>(data.data() + data.size()) - 1);
	data.pop_back();
	data.pop_back();
	data.pop_back();
	data.pop_back();
	return checksum == crypto::computeCRC32(data);
}

std::optional<std::vector<std::byte>> GMA::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

void GMA::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.length = buffer.size();
	entry.crc32 = crypto::computeCRC32(buffer);

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool GMA::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
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
			fileData.insert(fileData.end(), binData->begin(), binData->end());
		} else {
			entry->length = 0;
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// Header
		stream.write(this->header.signature);
		stream.write(this->header.version);
		stream.write(this->header.steamID);
		stream.write(this->header.timestamp);
		stream.write(this->header.requiredContent);
		stream.write(this->header.addonName);
		stream.write(this->header.addonDescription);
		stream.write(this->header.addonAuthor);
		stream.write(this->header.addonVersion);

		// File tree
		for (uint32_t i = 1; i <= entriesToBake.size(); i++) {
			stream.write(i);
			const auto& [path, entry] = entriesToBake[i - 1];
			stream.write(path);
			stream.write(entry->length);
			stream.write<uint32_t>(options.gma_writeCRCs ? entry->crc32 : 0);

			if (callback) {
				callback(path, *entry);
			}
		}
		stream.write<uint32_t>(0);

		// Fix offsets
		std::size_t offset = stream.tell_out();
		for (auto& [path, entry] : entriesToBake) {
			entry->offset = offset;
			offset += entry->length;
		}

		// File data
		stream.write(fileData);
	}

	// CRC of everything that's been written
	uint32_t crc = 0;
	if (options.gma_writeCRCs) {
		auto fileSize = std::filesystem::file_size(outputPath);
		FileStream stream{outputPath};
		stream.seek_in(0);
		crc = crypto::computeCRC32(stream.read_bytes(fileSize));
	}
	{
		FileStream stream{outputPath, FileStream::OPT_APPEND};
		stream.write(crc);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute GMA::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | CRC32;
}

GMA::operator std::string() const {
	return PackFile::operator std::string() +
		" | Version v" + std::to_string(this->header.version) +
		" | Addon Name: \"" + this->header.addonName + "\"";
}
