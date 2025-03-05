#include <vpkpp/format/SPK.h>

#include <filesystem>

#include <FileStream.h>
#include <sourcepp/String.h>
#include <zstd.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> SPK::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};

		stream
			.write(SPK_SIGNATURE)
			.write<uint32_t>(0x1405)
			.write<uint32_t>(0)
			.write<uint16_t>(0)
			.write<uint16_t>(0);
	}
	return SPK::open(path);
}

std::unique_ptr<PackFile> SPK::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* spk = new SPK{path};
	auto packFile = std::unique_ptr<PackFile>{spk};

	FileStream reader{spk->fullFilePath};
	reader.seek_in(0);
	if (reader.read<uint32_t>() != SPK_SIGNATURE) {
		// File is not an SPK
		return nullptr;
	}

	if (reader.read<uint32_t>() != 0x1405) {
		// Only supported version is 5125
		return nullptr;
	}

	const auto dictSize = reader.read<uint32_t>();
	const auto entryCount = reader.read<uint32_t>();

	// Reserved
	reader.skip_in<uint64_t>(2);

	if (dictSize > 0) {
		const auto dictData = reader.read_bytes(dictSize);
		spk->decompressionDict = {
			ZSTD_createDDict(dictData.data(), dictData.size()),
			[](void* dDict) { (void) ZSTD_freeDDict(static_cast<ZSTD_DDict*>(dDict)); },
		};
		reader.seek_in_u(reader.tell_in() + math::paddingForAlignment(16, reader.tell_in()));
	}

	std::vector<Entry> unnamedEntries;
	for (uint32_t i = 0; i < entryCount; i++) {
		// (u64) 64-bit filename hash
		// (u32) file contents crc32
		// (u32) data offset
		// (u32) data length uncompressed
		// (u32) data length compressed (0 if uncompressed)
		// (u64) reserved

		unnamedEntries.push_back(createNewEntry());
		auto& entry = unnamedEntries.back();

		reader.skip_in<uint64_t>();
		entry.crc32 = reader.read<uint32_t>();
		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();
		entry.compressedLength = reader.read<uint32_t>();
		reader.skip_in<uint64_t>();
	}

	for (const auto& entry : unnamedEntries) {
		// (u16) string length plus null terminator (not counting padding)
		// string mapping to that entry's actual filename

		auto entryPath = spk->cleanEntryPath(reader.read_string(reader.read<uint16_t>()));
		reader.seek_in_u(reader.tell_in() + math::paddingForAlignment(16, reader.tell_in()));

		spk->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::vector<std::string> SPK::verifyEntryChecksums() const {
	return this->verifyEntryChecksumsUsingCRC32();
}

// NOLINTNEXTLINE(*-no-recursion)
std::optional<std::vector<std::byte>> SPK::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	if (entry->length == 0 || entry->compressedLength == 0) {
		return std::vector<std::byte>{};
	}

	std::vector<std::byte> storedData;
	const auto storedDataLength = entry->compressedLength ? entry->compressedLength : entry->length;
	{
		FileStream stream{this->fullFilePath};
		if (!stream) {
			return std::nullopt;
		}
		stream.seek_in_u(entry->offset);
		storedData = stream.read_bytes(storedDataLength);
	}

	if (!entry->compressedLength) {
		return storedData;
	}

	if (!this->decompressionDict) {
		return std::nullopt;
	}

	std::unique_ptr<ZSTD_DCtx, void(*)(void*)> dctx{
		ZSTD_createDCtx(),
		[](void* dCtx) { (void) ZSTD_freeDCtx(static_cast<ZSTD_DCtx*>(dCtx)); },
	};
	if (!dctx) {
		return std::nullopt;
	}

	std::vector<std::byte> decompressedData;
	decompressedData.resize(entry->length);

	if (ZSTD_isError(ZSTD_decompress_usingDDict(dctx.get(), decompressedData.data(), decompressedData.size(), storedData.data(), storedData.size(), this->decompressionDict.get()))) {
		return std::nullopt;
	}
	return decompressedData;
}

Attribute SPK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | CRC32;
}
