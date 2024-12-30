#include <vpkpp/format/ORE.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> ORE::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write<uint32_t>(sizeof(uint32_t) * 3)
			.write<uint32_t>(0)
			.write<uint32_t>(0);
	}
	return ORE::open(path);
}

std::unique_ptr<PackFile> ORE::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* ore = new ORE{path};
	auto packFile = std::unique_ptr<PackFile>(ore);

	FileStream reader{ore->fullFilePath};
	reader.seek_in(0);

	auto headerSize = reader.read<uint32_t>();
	auto dirCount = reader.read<uint32_t>();
	std::vector<std::pair<std::string, uint32_t>> dirInfos;
	for (uint32_t i = 0; i < dirCount; i++) {
		std::pair<std::string, uint32_t> dirInfo;
		dirInfo.first = reader.read_string();
		dirInfo.second = reader.read<uint32_t>();
		dirInfos.push_back(dirInfo);
	}
	if (reader.tell_in() != headerSize) {
		return nullptr;
	}

	auto looseFileCount = reader.read<uint32_t>();
	const auto addOreFiles = [&callback, ore, &reader](const std::string& dirName, uint32_t dirOffset, uint32_t fileCount) {
		for (uint32_t i = 0; i < fileCount; i++) {
			Entry entry = createNewEntry();

			auto entryPath = ore->cleanEntryPath(dirName + (dirName.empty() ? "" : "/") + reader.read_string());

			entry.offset = reader.read<uint32_t>() + dirOffset;
			entry.length = reader.read<uint32_t>();

			ore->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
	};
	addOreFiles("", headerSize, looseFileCount);

	for (const auto& [dirName, dirOffset] : dirInfos) {
		reader.seek_in(dirOffset);
		if (reader.read<uint64_t>() != 8) {
			return nullptr;
		}
		addOreFiles(dirName, dirOffset + sizeof(uint64_t), reader.read<uint32_t>());
	}

	return packFile;
}

std::optional<std::vector<std::byte>> ORE::readEntry(const std::string& path_) const {
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

Attribute ORE::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
