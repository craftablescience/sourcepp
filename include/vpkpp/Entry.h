#pragma once

#include <array>
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
	/// Path to this entry (e.g. "materials/cable.vmt")
	std::string path;
	/// Format-specific flags (PCK: File flags, VPK: Internal parser state)
	uint32_t flags = 0;
	/// Length in bytes (in formats with compression, this is the uncompressed length)
	uint64_t length = 0;
	/// Offset, format-specific meaning - may be unused
	uint64_t offset = 0;
	/// If the format supports compression, this is the compressed length
	/// If compression is not supported or unused, this will remain 0
	uint64_t compressedLength = 0;
	/// Which external archive this entry is in
	/// If this file format does not support external archives, this will always be 0
	uint16_t archiveIndex = 0;
	/// CRC32 checksum - 0 if unused.
	/// Note that for GCF, this is actually an index into a checksum array and NOT a checksum
	uint32_t crc32 = 0;
	/// Used to check if entry is saved to disk
	bool unbaked = false;

	/// PCK - Each file has a 16-byte MD5 hash
	std::array<std::byte, 16> pck_md5{};

	/// VPK - Preloaded data
	std::vector<std::byte> vpk_preloadedData;

	/// Returns the parent directory's path (e.g. "materials/cable.vmt" -> "materials")
	[[nodiscard]] std::string getParentPath() const;

	/// Returns the filename (e.g. "materials/cable.vmt" -> "cable.vmt")
	[[nodiscard]] std::string getFilename() const;

	/// Returns the file stem (e.g. "materials/cable.vmt" -> "cable")
	[[nodiscard]] std::string getStem() const;

	/// Returns the file extension without a period (e.g. "materials/cable.vmt" -> "vmt")
	[[nodiscard]] std::string getExtension() const;

private:
	/// The data attached to the unbaked entry, or the path to the file containing the unbaked entry's data
	std::variant<std::string, std::vector<std::byte>> unbakedData;
	/// Which one?
	bool unbakedUsingByteBuffer = false;

	Entry() = default;
};

struct VirtualEntry {
	/// Filename of this entry (e.g. "cable.vmt")
	std::string name;

	/// Read-only, or writable?
	bool writable;
};

} // namespace vpkpp
