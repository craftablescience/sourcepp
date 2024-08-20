#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace vpkpp {

/// This class represents the metadata that a file has inside a PackFile.
/// It is used to access the file's data and read its properties
class Entry {
	friend class PackFile;

public:
	/// Format-specific flags (PCK: File flags, VPK: Internal parser state)
	uint32_t flags = 0;

	/// Which external archive this entry is in.
	/// If this file format does not support external archives, this will always be 0
	uint32_t archiveIndex = 0;

	/// Length in bytes (in formats with compression, this is the uncompressed length)
	uint64_t length = 0;

	/// If the format supports compression, this is the compressed length.
	/// If compression is not supported or unused, this will remain 0
	uint64_t compressedLength = 0;

	/// Offset, format-specific meaning - 0 if unused, or if the offset genuinely is 0
	uint64_t offset = 0;

	/// Format-specific (PCK: MD5 hash, VPK: Preloaded data)
	std::vector<std::byte> extraData;

	/// CRC32 checksum - 0 if unused.
	/// Note that for GCF, this is actually an index into a checksum array and NOT a checksum
	uint32_t crc32 = 0;

	/// Used to check if entry is saved to disk
	bool unbaked = false;

private:
	/// The data attached to the unbaked entry, or the path to the file containing the unbaked entry's data
	std::variant<std::string, std::vector<std::byte>> unbakedData;
	/// Which one?
	bool unbakedUsingByteBuffer = false;

	Entry() = default;
};

} // namespace vpkpp
