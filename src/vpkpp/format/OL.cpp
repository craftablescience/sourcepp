#include <vpkpp/format/OL.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

namespace {

enum class OLEntryType : uint32_t {
	RMF = 0,
	MAP = 1,
};

} // namespace

std::unique_ptr<PackFile> OL::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* ol = new OL{path};
	auto packFile = std::unique_ptr<PackFile>(ol);

	FileStream reader{ol->fullFilePath};
	reader.seek_in(0);

	if (reader.read_string(OL_SIGNATURE.size(), false) != OL_SIGNATURE) {
		// File is not an OL
		return nullptr;
	}

	if (auto version = reader.read<float>(); version < 0.05 || version > 0.15) {
		// Version must be 0.1
		return nullptr;
	}

	auto entryOffset = reader.read<uint32_t>();
	auto entryCount = reader.read<uint32_t>();

	reader.read(ol->notes);

	reader.seek_in(entryOffset);
	for (uint32_t i = 0; i < entryCount; i++) {
		Entry entry = createNewEntry();

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		const auto baseEntryPath = ol->cleanEntryPath(reader.read_string(31));
		auto entryPath = baseEntryPath;

		auto notes = reader.read_string(501);

		std::string extension;
		auto type = reader.read<OLEntryType>();
		switch (type) {
			case OLEntryType::RMF:
				extension = ".rmf";
				break;
			case OLEntryType::MAP:
				extension = ".map";
				break;
		}
		entryPath += extension;

		// Entries can have the same name, but our map can't handle non-unique keys!
		for (int j = 1; ol->entries.count(entryPath) > 0; j++) {
			entryPath = baseEntryPath;
			entryPath
				.append(" (")
				.append(std::to_string(j))
				.append(")")
				.append(extension);
		}

		if (!notes.empty()) {
			entry.extraData = {reinterpret_cast<const std::byte*>(notes.data()), reinterpret_cast<const std::byte*>(notes.data() + notes.size())};
		}

		ol->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> OL::readEntry(const std::string& path_) const {
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

Attribute OL::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}

const std::string& OL::getNotes() const {
	return this->notes;
}

std::optional<std::string> OL::getEntryNotes(const std::string& path) const {
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	return std::string{reinterpret_cast<const char*>(entry->extraData.data()), entry->extraData.size()};
}
