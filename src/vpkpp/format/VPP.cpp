#include <vpkpp/format/VPP.h>

#include <filesystem>

#include <FileStream.h>

#include <miniz.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> VPP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* vpp = new VPP{path};
	auto packFile = std::unique_ptr<PackFile>(vpp);

	FileStream reader{vpp->fullFilePath};
	reader.seek_in(0);

	// Verify signature
	if (const auto signature = reader.read<uint32_t>(); signature == VPP_SIGNATURE_BIG) {
		reader.set_big_endian(true);
	} else if (signature != VPP_SIGNATURE_LIL) {
		return nullptr;
	}

	// Create entries
	const auto version = reader.read<uint32_t>();
	if (version == 1) {
		// Get number of entries
		const auto entryCount = reader.read<uint32_t>();

		// Verify file size
		if (reader.read<uint32_t>() != std::filesystem::file_size(path)) {
			return nullptr;
		}

		// Get file table offset
		static constexpr uint32_t headerSize = sizeof(uint32_t) * 4;
		reader.seek_in(headerSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, headerSize));

		// Get base file offset
		const uint32_t fileTableSize = (60 + sizeof(uint32_t)) * entryCount;
		vpp->entryBaseOffset = headerSize    + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, headerSize)
		                     + fileTableSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, fileTableSize);

		// Get first file offset
		uint32_t entryOffset = 0;

		// Read file entries
		for (uint32_t i = 0; i < entryCount; i++) {
			Entry entry = createNewEntry();

			// Get file path
			const auto entryPath = vpp->cleanEntryPath(reader.read_string(60));

			// Get file size
			entry.length = reader.read<uint32_t>();

			// Calculate file offset
			entry.offset = entryOffset;
			entryOffset += entry.length + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entry.length);

			// Put it in
			vpp->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
	} else if (version == 2) {
		// Get number of entries
		const auto entryCount = reader.read<uint32_t>();

		// Verify file size
		if (reader.read<uint32_t>() != std::filesystem::file_size(path)) {
			return nullptr;
		}

		// Get file table offset
		static constexpr uint32_t headerSize = sizeof(uint32_t) * 4;
		reader.seek_in(headerSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, headerSize));

		// Get base file offset
		const uint32_t fileTableSize = (24 + sizeof(uint32_t) * 2) * entryCount;
		vpp->entryBaseOffset = headerSize    + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, headerSize)
		                     + fileTableSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, fileTableSize);

		// Get first file offset
		uint32_t entryOffset = 0;

		// Read file entries
		for (uint32_t i = 0; i < entryCount; i++) {
			Entry entry = createNewEntry();

			// Get file path
			const auto entryPath = vpp->cleanEntryPath(reader.read_string(24));

			// Get file size
			entry.length = reader.read<uint32_t>();

			// There's a duplicate size field here
			// Probably one means compressed, the other uncompressed
			// Just bail out for now
			if (reader.read<uint32_t>() != entry.length) {
				return nullptr;
			}

			// Calculate file offset
			entry.offset = entryOffset;
			entryOffset += entry.length + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entry.length);

			// Put it in
			vpp->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
	} else if (version == 3 || version == 4) {
		// Skip unused header data
		reader.skip_in(64 + 256 + sizeof(uint32_t));

		// Get package flags
		reader >> vpp->flags;

		// Remove compressed flag if we're also condensed
		if (vpp->flags & FLAG_CONDENSED) {
			vpp->flags &= ~FLAG_COMPRESSED;
		}

		// ??
		reader.skip_in<uint32_t>();

		// Get number of entries
		const auto entryCount = reader.read<uint32_t>();

		// Verify file size
		if (reader.read<uint32_t>() != std::filesystem::file_size(path)) {
			return nullptr;
		}

		// Get sizes
		const auto entryDirectorySize = reader.read<uint32_t>();
		const auto entryNamesSize = reader.read<uint32_t>();
		uint32_t entryExtensionsSize = 0;

		if (version == 4) {
			entryExtensionsSize = reader.read<uint32_t>();
		}

		// Check if we have compression
		const auto entryDataSizeUncompressed = reader.read<uint32_t>();
		const auto entryDataSizeCompressed = reader.read<uint32_t>();

		// Set base data offset
		vpp->entryBaseOffset = VPP_ALIGNMENT
		                     + entryDirectorySize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryDirectorySize)
		                     + entryNamesSize     + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryNamesSize);

		if (version == 4) {
			vpp->entryBaseOffset += entryExtensionsSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryExtensionsSize);
		}

		// Seek to file directory (alignment boundary)
		reader.seek_in(VPP_ALIGNMENT);

		// Read file entries
		for (uint32_t i = 0; i < entryCount; i++) {
			Entry entry = createNewEntry();

			// Get file name offset
			const auto entryNameOffset = reader.read<uint32_t>();
			uint32_t entryExtensionOffset = 0;
			if (version == 4) {
				entryExtensionOffset = reader.read<uint32_t>();
			}

			// ??
			reader.skip_in<uint32_t>();

			// Get file offset
			entry.offset = reader.read<uint32_t>();

			// ??
			if (version == 3) {
				reader.skip_in<uint32_t>();
			}

			// Get file size
			entry.length = reader.read<uint32_t>();
			entry.compressedLength = reader.read<uint32_t>();

			// Not compressed
			if (entry.compressedLength == 0xFFFFFFFF || !(vpp->flags & FLAG_COMPRESSED)) {
				entry.compressedLength = 0;
			}

			// ??
			reader.skip_in<uint32_t>();

			// Get file name
			const auto lastPos = reader.tell_in();

			std::string entryPath;

			if (version == 4) {
				// Version 4
				reader.seek_in(
					VPP_ALIGNMENT +
					entryDirectorySize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryDirectorySize) +
					entryNameOffset
				);

				std::string entryName = reader.read_string(entryNamesSize - entryNameOffset);

				reader.seek_in(
					VPP_ALIGNMENT +
					entryDirectorySize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryDirectorySize) +
					entryNamesSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryNamesSize) +
					entryExtensionOffset
				);

				std::string entryExtension = reader.read_string(entryExtensionsSize - entryExtensionOffset);

				entryPath = vpp->cleanEntryPath(entryName + "." + entryExtension);
			} else {
				// Version 3
				reader.seek_in(VPP_ALIGNMENT + entryDirectorySize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, entryDirectorySize) + entryNameOffset);
				entryPath = vpp->cleanEntryPath(reader.read_string(entryNamesSize - entryNameOffset));
			}

			reader.seek_in_u(lastPos);

			// Put it in
			vpp->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}

		// Uncondense data
		if (vpp->flags & FLAG_CONDENSED) {
			reader.seek_in(vpp->entryBaseOffset);
			vpp->uncondensedData.resize(entryDataSizeUncompressed);
			auto compressedData = reader.read_bytes(entryDataSizeCompressed);
			mz_ulong uncompressedLength = entryDataSizeUncompressed;
			mz_uncompress(reinterpret_cast<unsigned char*>(vpp->uncondensedData.data()), &uncompressedLength, reinterpret_cast<const unsigned char*>(compressedData.data()), entryDataSizeCompressed);
		}
	} else {
		return nullptr;
	}

	return packFile;
}

std::optional<std::vector<std::byte>> VPP::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	if (this->flags & FLAG_CONDENSED) {
		// Condensed entry
		BufferStreamReadOnly stream{this->uncondensedData.data(), this->uncondensedData.size()};
		stream.seek_u(entry->offset);
		return stream.read_bytes(entry->length);
	} else if (this->flags & FLAG_COMPRESSED) {
		// Compressed entry
		if (!entry->compressedLength) {
			return std::nullopt;
		}
		FileStream stream{this->fullFilePath};
		if (!stream) {
			return std::nullopt;
		}
		stream.seek_in_u(this->entryBaseOffset + entry->offset);
		auto compressedData = stream.read_bytes(entry->compressedLength);
		mz_ulong uncompressedLength = entry->length;
		std::vector<std::byte> uncompressedData(uncompressedLength);
		mz_uncompress(reinterpret_cast<unsigned char*>(uncompressedData.data()), &uncompressedLength, reinterpret_cast<const unsigned char*>(compressedData.data()), entry->compressedLength);
		return uncompressedData;
	} else {
		// Uncompressed entry
		FileStream stream{this->fullFilePath};
		if (!stream) {
			return std::nullopt;
		}
		stream.seek_in_u(this->entryBaseOffset + entry->offset);
		return stream.read_bytes(entry->length);
	}
}

Attribute VPP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
