#include <vpkpp/format/XZP.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> XZP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* xzp = new XZP{path};
	auto packFile = std::unique_ptr<PackFile>(xzp);

	FileStream reader{xzp->fullFilePath};
	reader.seek_in(0);

	if (reader.read<uint32_t>() != XZP_HEADER_SIGNATURE) {
		// File is not an XZP
		return nullptr;
	}

	if (reader.read<uint32_t>() != 6) {
		// Invalid version - check around for earlier formats eventually
		return nullptr;
	}

	const auto preloadDirectoryEntryCount = reader.read<uint32_t>();
	const auto directoryEntryCount = reader.read<uint32_t>();
	reader.skip_in<uint32_t>(); // preloadBytes

	if (reader.read<uint32_t>() != sizeof(uint32_t) * 9) {
		// Header size - should always be 9 uints for v6
		return nullptr;
	}

	if (const auto filepathEntryCount = reader.read<uint32_t>(); filepathEntryCount != directoryEntryCount) {
		// We can't reverse a hash! Just bail
		return nullptr;
	}

	const auto filepathStringsOffset = reader.read<uint32_t>();
	reader.skip_in<uint32_t>(); // filepathStringsLength

	// Add directory entries
	std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> stagedEntryChunks;
	for (uint32_t i = 0; i < directoryEntryCount; i++) {
		const auto filepathCRC = reader.read<uint32_t>();
		const auto chunkLength = reader.read<uint32_t>();
		const auto chunkOffset = reader.read<uint32_t>();

		if (!stagedEntryChunks.contains(filepathCRC)) {
			stagedEntryChunks[filepathCRC] = {};
			stagedEntryChunks[filepathCRC].emplace_back(chunkOffset, chunkLength);
		} else if (stagedEntryChunks[filepathCRC].back().first + stagedEntryChunks[filepathCRC].back().second == chunkOffset) {
			stagedEntryChunks[filepathCRC].back().second += chunkLength;
		} else {
			stagedEntryChunks[filepathCRC].emplace_back(chunkOffset, chunkLength);
		}
	}

	// Add preload entries
	std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> stagedEntryPreloads;
	for (uint32_t i = 0; i < preloadDirectoryEntryCount; i++) {
		const auto filepathCRC = reader.read<uint32_t>();
		const auto preloadLength = reader.read<uint32_t>();
		const auto preloadOffset = reader.read<uint32_t>();

		stagedEntryPreloads[filepathCRC] = {preloadLength, preloadOffset};
	}

	// Preload size per entry
	reader.skip_in<uint16_t>(directoryEntryCount);

	// filepaths, and put it all together simultaneously
	reader.seek_in(filepathStringsOffset);
	std::unordered_map<uint32_t, std::string> stagedEntryFilepaths;
	for (uint32_t i = 0; i < directoryEntryCount; i++) {
		const auto filepathCRC = reader.read<uint32_t>();
		const auto filepathOffset = reader.read<uint32_t>();
		reader.skip_in<uint32_t>(); // timestamp

		const auto readerPos = reader.tell_in();
		reader.seek_in_u(filepathOffset);
		stagedEntryFilepaths[filepathCRC] = reader.read_string();
		reader.seek_in_u(readerPos);
	}

	// Put it all together
	for (const auto& [filepathCRC, filepath] : stagedEntryFilepaths) {
		Entry entry = createNewEntry();

		auto entryPath = xzp->cleanEntryPath(filepath);

		BufferStream stream{entry.extraData};

		const auto& chunks = stagedEntryChunks[filepathCRC];
		stream.write<uint32_t>(chunks.size());

		entry.length = 0;
		for (const auto& chunk : chunks) {
			entry.length += chunk.second;
			stream << chunk.first << chunk.second;
		}

		if (stagedEntryPreloads.contains(filepathCRC)) {
			const auto& preload = stagedEntryPreloads[filepathCRC];
			stream.write<uint32_t>(true);
			stream << preload.first << preload.second;
		} else {
			stream.write<uint32_t>(false);
		}

		xzp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> XZP::readEntry(const std::string& path_) const {
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
	std::vector<std::byte> out;
	BufferStreamReadOnly entryDataStream{entry->extraData};
	const auto chunks = entryDataStream.read<uint32_t>();
	for (uint32_t i = 0; i < chunks; i++) {
		const auto chunkOffset = entryDataStream.read<uint32_t>();
		const auto chunkLength = entryDataStream.read<uint32_t>();
		const auto chunkData = stream.seek_in(chunkOffset).read_bytes(chunkLength);
		out.insert(out.end(), chunkData.begin(), chunkData.end());
	}
	return out;
}

Attribute XZP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
