// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/APK.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> APK::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write(APK_SIGNATURE)
			.write<uint32_t>(sizeof(uint32_t) * 4)
			.write<uint32_t>(0)
			.write<uint32_t>(sizeof(uint32_t) * 4)
			.pad<uint32_t>();
	}
	return APK::open(path);
}

std::unique_ptr<PackFile> APK::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* apk = new APK{path};
	auto packFile = std::unique_ptr<PackFile>(apk);

	FileStream reader{apk->fullFilePath};
	reader.seek_in(0);

	if (
		reader.read<uint32_t>() != APK_SIGNATURE ||
		reader.read<uint32_t>() != sizeof(uint32_t) * 4
	) {
		// File is not an APK
		return nullptr;
	}

	const auto entryCount = reader.read<uint32_t>();

	auto nextEntryOffset = reader.read<uint32_t>();
	for (uint32_t i = 0; i < entryCount; i++) {
		reader.seek_in(nextEntryOffset);

		Entry entry = createNewEntry();

		auto entryPath = apk->cleanEntryPath(reader.read_string(reader.read<uint32_t>() + 1));

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		reader
			.read(nextEntryOffset)
			.skip_in<uint32_t>();

		apk->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> APK::readEntry(const std::string& path_) const {
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

void APK::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool APK::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
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

		// Signature + header size
		stream
			.write(APK_SIGNATURE)
			.write<uint32_t>(sizeof(uint32_t) * 4);

		// Offset and size of directory
		static constexpr auto HEADER_OFFSET = sizeof(uint32_t) * 5;
		stream
			.write<uint32_t>(entriesToBake.size())
			.write<uint32_t>(HEADER_OFFSET + fileData.size())
			.pad<uint32_t>();

		// File data
		stream.write(fileData);

		// Directory
		for (const auto& [path, entry] : entriesToBake) {
			stream
				.write<uint32_t>(path.size())
				.write(path)
				.write<uint32_t>(entry->offset + HEADER_OFFSET)
				.write<uint32_t>(entry->length)
				.write<uint32_t>(stream.tell_out() + sizeof(uint32_t) * 2)
				.pad<uint32_t>();

			if (callback) {
				callback(path, *entry);
			}
		}
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute APK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
