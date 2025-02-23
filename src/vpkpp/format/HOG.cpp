#include <vpkpp/format/HOG.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> HOG::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* hog = new HOG{path};
	auto packFile = std::unique_ptr<PackFile>(hog);

	FileStream reader{hog->fullFilePath};
	reader.seek_in(0);

	// Verify signature
	if (const auto signature = reader.read_string(3); signature != HOG_SIGNATURE) {
		return nullptr;
	}

	// Read file entries
	while (true) {
		// Create new entry
		Entry entry = createNewEntry();

		// Get file path
		const auto entryPath = hog->cleanEntryPath(reader.read_string(13));

		// Check if we're at EOF (must perform the check after reading beyond file bounds)
		if (!reader) {
			break;
		}

		// Get file size and offset
		entry.length = reader.read<uint32_t>();
		entry.offset = reader.tell_in();

		// Seek past file data
		reader.seek_in_u(entry.offset + entry.length);

		// Put it in
		hog->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> HOG::readEntry(const std::string& path_) const {
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

Attribute HOG::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
