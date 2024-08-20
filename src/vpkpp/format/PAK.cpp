#include <vpkpp/format/PAK.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

PAK::PAK(const std::string& fullFilePath_, PackFileOptions options_)
		: PackFile(fullFilePath_, options_) {
	this->type = PackFileType::PAK;
}

std::unique_ptr<PackFile> PAK::open(const std::string& path, PackFileOptions options, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* pak = new PAK{path, options};
	auto packFile = std::unique_ptr<PackFile>(pak);

	FileStream reader{pak->fullFilePath};
	reader.seek_in(0);

	if (auto signature = reader.read<uint32_t>(); signature != PAK_SIGNATURE) {
		// File is not a PAK
		return nullptr;
	}

	auto directoryOffset = reader.read<uint32_t>();
	// Directory size / file entry size
	auto fileCount = reader.read<uint32_t>() / 64;

	reader.seek_in(directoryOffset);
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		auto entryPath = pak->cleanEntryPath(reader.read_string(PAK_FILENAME_MAX_SIZE));

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		pak->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> PAK::readEntry(const std::string& path_) const {
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

void PAK::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) {
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool PAK::bake(const std::string& outputDir_, const EntryCallback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntries([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	for (auto& [path, entry] : entriesToBake) {
		if (auto binData = this->readEntry(path)) {
			entry->offset = fileData.size();

			fileData.insert(fileData.end(), binData->begin(), binData->end());
		} else {
			entry->offset = 0;
			entry->length = 0;
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// Signature
		stream.write(PAK_SIGNATURE);

		// Index and size of directory
		const uint32_t directoryIndex = sizeof(PAK_SIGNATURE) + sizeof(uint32_t) * 2;
		stream.write(directoryIndex);
		const uint32_t directorySize = entriesToBake.size() * 64;
		stream.write(directorySize);

		// Directory
		for (const auto& [path, entry] : entriesToBake) {
			stream.write(path, false, PAK_FILENAME_MAX_SIZE);
			stream.write(static_cast<uint32_t>(entry->offset + directoryIndex + directorySize));
			stream.write(static_cast<uint32_t>(entry->length));

			if (callback) {
				callback(path, *entry);
			}
		}

		// File data
		stream.write(fileData);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute PAK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
