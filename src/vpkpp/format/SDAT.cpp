// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/SDAT.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> SDAT::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write(SDAT_SIGNATURE)
			.write<uint32_t>(0)
			.write<uint32_t>(0);
	}
	return SDAT::open(path);
}

std::unique_ptr<PackFile> SDAT::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* sdat = new SDAT{path};
	auto packFile = std::unique_ptr<PackFile>(sdat);

	FileStream reader{sdat->fullFilePath};
	reader.seek_in(0);

	if (reader.read<uint32_t>() != SDAT_SIGNATURE) {
		// File is not an SDAT
		return nullptr;
	}
	reader.set_big_endian(true);

	// dirSize does not include the signature or itself
	reader.skip_in<uint32_t>(); //const auto dirSize = reader.read<uint32_t>();

	const auto entryCount = reader.read<uint32_t>();

	for (uint32_t i = 0; i < entryCount; i++) {
		Entry entry = createNewEntry();

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		const auto entryPathOffset = reader.read<uint32_t>();
		const auto oldPos = reader.tell_in();
		const auto entryPath = sdat->cleanEntryPath(reader.seek_in_u(entryPathOffset + sizeof(uint32_t) * 2).read_string());
		reader.seek_in_u(oldPos);

		sdat->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> SDAT::readEntry(const std::string& path_) const {
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

void SDAT::addEntryInternal(Entry& entry, const std::string&, std::vector<std::byte>& buffer, EntryOptions) {
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool SDAT::bake(const std::string& outputDir_, BakeOptions, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	uint32_t pathBlockSize = 0;
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&pathBlockSize, &entriesToBake](const std::string& path, Entry& entry) {
		pathBlockSize += path.size() + 1;
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

		const auto dirSize = sizeof(uint32_t) * 3 * (entriesToBake.size() + 1) + pathBlockSize;

		// Signature + dir size + entry count
		stream
			.write(SDAT_SIGNATURE)
			.set_big_endian(true)
			.write<uint32_t>(dirSize - 8)
			.write<uint32_t>(entriesToBake.size())
			.pad(dirSize - sizeof(uint32_t))
			.seek_out(sizeof(uint32_t) * 3);

		// Directory
		auto currentPathOffset = dirSize - pathBlockSize;
		for (const auto& [path, entry] : entriesToBake) {
			stream
				.write<uint32_t>(entry->offset + dirSize)
				.write<uint32_t>(entry->length)
				.write<uint32_t>(currentPathOffset - sizeof(uint32_t) * 2);

			const auto oldPos = stream.tell_out();
			stream.seek_out_u(currentPathOffset).write(path);
			currentPathOffset = stream.tell_out();
			stream.seek_out_u(oldPos);

			if (callback) {
				callback(path, *entry);
			}
		}

		// File data
		stream.seek_out_u(dirSize).write(fileData);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute SDAT::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
