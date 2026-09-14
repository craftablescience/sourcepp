// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/GRP.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> GRP::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write(GRP_SIGNATURE, false, GRP_SIGNATURE.size())
			.write<uint32_t>(0);
	}
	return GRP::open(path);
}

std::unique_ptr<PackFile> GRP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* grp = new GRP{path};
	auto packFile = std::unique_ptr<PackFile>(grp);

	FileStream reader{grp->fullFilePath};
	reader.seek_in(0);

	if (reader.read_string(12) != GRP_SIGNATURE) {
		// File is not a GRP
		return nullptr;
	}

	const auto entryCount = reader.read<uint32_t>();

	const auto dirSize = (GRP_FILENAME_MAX_SIZE + sizeof(uint32_t)) * (entryCount + 1);
	uint32_t currentOffset = dirSize;
	for (int i = 0; i < entryCount; i++) {
		Entry entry = createNewEntry();

		auto entryPath = grp->cleanEntryPath(reader.read_string(GRP_FILENAME_MAX_SIZE));

		entry.length = reader.read<uint32_t>();

		entry.offset = currentOffset;
		currentOffset += entry.length;

		grp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> GRP::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
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

void GRP::addEntryInternal(Entry& entry, const std::string&, std::vector<std::byte>& buffer, EntryOptions) {
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool GRP::bake(const std::string& outputDir_, BakeOptions, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});

	// Read data before overwriting, we don't know if we're writing to ourself
	const auto dirSize = (GRP_FILENAME_MAX_SIZE + sizeof(uint32_t)) * (entriesToBake.size() + 1);
	std::vector<std::byte> fileData;
	for (auto& [path, entry] : entriesToBake) {
		if (auto binData = this->readEntry(path)) {
			entry->offset = fileData.size() + dirSize;

			fileData.insert(fileData.end(), binData->begin(), binData->end());
		} else {
			entry->offset = 0;
			entry->length = 0;
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// Signature + entry count
		stream
			.write(GRP_SIGNATURE, false, GRP_SIGNATURE.size())
			.write<uint32_t>(entriesToBake.size());

		// Directory
		for (const auto& [path, entry] : entriesToBake) {
			stream
				.write(path, false, GRP_FILENAME_MAX_SIZE)
				.write<uint32_t>(entry->length);

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

Attribute GRP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
