#include <vpkpp/format/VPP.h>

#include <filesystem>

#include <FileStream.h>

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

		// Get first file offset
		const uint32_t fileTableSize = (60 + sizeof(uint32_t)) * entryCount;
		uint32_t entryOffset = reader.tell_in() + fileTableSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, fileTableSize);

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

		// Get first file offset
		const uint32_t fileTableSize = (24 + sizeof(uint32_t) * 2) * entryCount;
		uint32_t entryOffset = reader.tell_in() + fileTableSize + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, fileTableSize);

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

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

Attribute VPP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
