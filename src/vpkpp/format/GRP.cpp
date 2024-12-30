#include <vpkpp/format/GRP.h>

#include <filesystem>

#include <BufferStream.h>
#include <FileStream.h>
#include <miniz.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vpkpp;

GRP::GRP(const std::string& fullFilePath_)
		: PackFileReadOnly(fullFilePath_) {
	this->type = PackFileType::GRP;
}

std::unique_ptr<PackFile> GRP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* grp = new GRP{path};
	auto packFile = std::unique_ptr<PackFile>(grp);

	auto fileData = fs::readFileBuffer(grp->fullFilePath);
	BufferStream reader{fileData};
	reader.seek(0);

	if (auto signature = reader.read<uint32_t>(); signature != GRP_SIGNATURE) {
		// File is not a GRP
		return nullptr;
	}

	reader.set_big_endian(true);

	reader >> grp->version;
	auto fileCount = reader.read<uint32_t>();

	//auto unk1 = reader.read<uint32_t>(); // something to do with compression?
	reader.skip<uint32_t>();

	const auto headerSize = sizeof(uint32_t) * 4 * (fileCount + 1);

	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		auto crc = reader.read<uint32_t>();
		auto entryPath = grp->cleanEntryPath(crypto::encodeHexString({reinterpret_cast<const std::byte*>(&crc), sizeof(crc)}));

		entry.offset = reader.read<uint32_t>() + headerSize;
		entry.length = reader.read<uint32_t>();
		entry.compressedLength = reader.read<uint32_t>();

		grp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> GRP::readEntry(const std::string& path_) const {
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
	stream.seek_in_u(entry->offset);
	if (entry->compressedLength == 0) {
		return stream.read_bytes(entry->length);
	}

	// Decode
	auto compressedData = stream.read_bytes(entry->compressedLength);
	std::vector<std::byte> data(entry->length);
	mz_ulong len = entry->length;
	if (auto x = mz_uncompress(reinterpret_cast<unsigned char*>(data.data()), &len, reinterpret_cast<const unsigned char*>(compressedData.data()), entry->compressedLength); x != MZ_OK) {
		return std::nullopt;
	}
	return data;
}

Attribute GRP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
