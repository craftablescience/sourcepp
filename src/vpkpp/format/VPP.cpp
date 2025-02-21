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
	auto signature = reader.read<uint32_t>();
	if (signature == VPP_SIGNATURE_BIG) {
		reader.set_big_endian(true);
	} else if (signature != VPP_SIGNATURE_LIL) {
		return nullptr;
	}

	// Verify version
	auto version = reader.read<uint32_t>();
	if (version < VPP_VERSION_MIN || version > VPP_VERSION_MAX) {
		return nullptr;
	}

	// Create entries
	if (version == 1) {
		// Get header fields
		auto num_files = reader.read<uint32_t>();
		auto len_vpp = reader.read<uint32_t>();

		// Verify file size
		if (len_vpp != std::filesystem::file_size(path)) {
			return nullptr;
		}

		// Seek past header to 2048 byte boundary
		reader.seek_in(VPP_ALIGNMENT);

		// Get first file offset
		// 64 is the byte size of each file directory entry
		uint32_t entryOffset = VPP_ALIGNMENT + sourcepp::math::paddingForAlignment(VPP_ALIGNMENT, 64 * num_files);

		// Read file entries
		for (uint32_t i = 0; i < num_files; i++) {
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
