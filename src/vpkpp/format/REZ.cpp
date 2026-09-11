#include <vpkpp/format/REZ.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> REZ::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* rez = new REZ{path};
	auto packFile = std::unique_ptr<PackFile>(rez);

	FileStream reader{rez->fullFilePath};
	reader.seek_in(0);

	if (const auto c = reader.read<uint8_t>(); c != '\r' && c != '&') {
		return nullptr;
	}
	if (const auto c = reader.read<uint8_t>(); c != '\n' && c != '#') {
		return nullptr;
	}
	reader.read(rez->fileType, 60, false);
	string::rtrim(rez->fileType);

	if (const auto c = reader.read<uint8_t>(); c != '\r' && c != '!') {
		return nullptr;
	}
	if (const auto c = reader.read<uint8_t>(); c != '\n' && c != '\"') {
		return nullptr;
	}
	reader.read(rez->userTitle, 60, false);
	string::rtrim(rez->userTitle);

	if (const auto c = reader.read<uint8_t>(); c != '\r' && c != '%') {
		return nullptr;
	}
	if (const auto c = reader.read<uint8_t>(); c != '\n' && c != '\'') {
		return nullptr;
	}
	rez->version = reader.read<uint8_t>();

	switch (rez->version) {
		case 0x1A: // EOF (1)
			reader.read(rez->version);
			if (rez->version != 1) {
				reader.skip_in(7).read(rez->version);
				if (rez->version != 2) {
					return nullptr;
				}
			}
			break;
		case 0x2A: { // EOF (2)
			reader.skip_in(68).read(rez->version);
			if (rez->version != 1) {
				return nullptr;
			}
		}
		default:
			return nullptr;
	}

	const auto rootDirOffset = reader.read<uint32_t>();
	const auto rootDirSize = reader.read<uint32_t>();
	reader.skip_in<uint32_t>(); // rootDirTime
	reader.skip_in<uint32_t>(); // nextWriteOffset
	reader.skip_in<uint32_t>(); // time
	reader.skip_in<uint32_t>(); // largestKeyArray
	reader.skip_in<uint32_t>(); // largestDirNameSize
	reader.skip_in<uint32_t>(); // largestFileNameSize
	reader.skip_in<uint32_t>(); // largestCommentSize
	reader.skip_in<uint8_t>();  // isSorted

	// Read the file tree recursively
	std::function<bool(const std::string&, std::span<const std::byte>)> readBlock;
	readBlock = [&callback, &rez, &reader, &readBlock](const std::string& parentPath, std::span<const std::byte> blockData) {
		BufferStreamReadOnly blockStream{blockData.data(), blockData.size()};

		while (blockStream.tell() != blockStream.size()) {
			const auto blockType = blockStream.read<uint32_t>();
			const auto blockOffset = blockStream.read<uint32_t>();
			const auto blockLength = blockStream.read<uint32_t>();
			blockStream.skip<uint32_t>(); // blockTime

			if (blockType == 0) {
				// File
				Entry entry = createNewEntry();

				//const auto resourceID = blockStream.read<uint32_t>();
				blockStream.skip<uint32_t>();
				auto resourceExtension = blockStream.read_string(4);
				const auto resourceKeyCount = blockStream.read<uint32_t>();
				const auto resourceName = blockStream.read_string();
				const auto resourceDesc = blockStream.read_string();
				blockStream.skip<uint32_t>(resourceKeyCount);

				auto entryPath = parentPath;
				entryPath += '/' + resourceName;
				if (!resourceExtension.empty()) {
					std::ranges::reverse(resourceExtension);
					entryPath += '.' + resourceExtension;
				}
				entryPath = rez->cleanEntryPath(entryPath);

				entry.length = blockLength;
				entry.offset = blockOffset;

				rez->entries.emplace(entryPath, entry);

				if (callback) {
					callback(entryPath, entry);
				}
			} else if (blockType == 1) {
				// Directory
				const auto dirName = blockStream.read_string();
				if (blockLength) {
					std::string newParentPath;
					if (!parentPath.empty()) {
						newParentPath = parentPath;
						newParentPath += '/' + dirName;
					} else {
						newParentPath = dirName;
					}
					readBlock(newParentPath, reader.seek_in(blockOffset).read_bytes(blockLength));
				}
			} else {
				// Invalid
				return false;
			}
		}
		return true;
	};
	if (!readBlock("", reader.seek_in(rootDirOffset).read_bytes(rootDirSize))) {
		return nullptr;
	}

	return packFile;
}

std::optional<std::vector<std::byte>> REZ::readEntry(const std::string& path_) const {
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

Attribute REZ::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}

std::string_view REZ::getFileType() const {
	return this->fileType;
}

std::string_view REZ::getUserTitle() const {
	return this->userTitle;
}

uint32_t REZ::getVersion() const {
	return this->version;
}
