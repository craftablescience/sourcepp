/*
 * AssetDesc and BlobDesc structs are taken from the RTX Remix source code.
 * https://github.com/NVIDIAGameWorks/dxvk-remix/tree/main/src/dxvk/rtx_render/rtx_asset_package.h
 * See THIRDPARTY_LEGAL_NOTICES.txt for more information.
 */

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr uint32_t PKG_SIGNATURE = 0xbaadd00d;
constexpr std::string_view PKG_EXTENSION = ".pkg";

class PKG : public PackFileReadOnly {
protected:
	struct Asset {
		uint16_t nameIndex;
		enum class Type : uint8_t {
			UNKNOWN,
			IMAGE_1D,
			IMAGE_2D,
			IMAGE_3D,
			IMAGE_CUBE,
			BUFFER,
		} type;
		uint8_t format; // VkFormat
		uint16_t width;
		uint16_t height;
		uint16_t depth;
		uint16_t mipCount;
		uint16_t tailMipCount;
		uint16_t arraySize;
		uint16_t startBlobIndex;
		uint16_t endBlobIndex;

		[[nodiscard]] uint32_t getBlobIndex(int frame, int face, int mip) const;
	};

	struct Blob {
		uint64_t offset;
		enum class Compression : uint8_t {
			NONE,
			GDEFLATE,
		} compression;
		uint8_t flags;
		uint32_t size;
		uint32_t crc32;
	};

	struct Archive {
		std::vector<Asset> assets;
		std::vector<Blob> blobs;
	};

public:
	/// Open a PKG file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] constexpr bool isCaseSensitive() const override {
		return true;
	}

	[[nodiscard]] std::string getTruncatedFilestem() const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	[[nodiscard]] bool openNumbered(uint16_t archiveIndex, const std::string& path, const EntryCallback& callback);

	std::unordered_map<uint16_t, Archive> metadata;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PKG_EXTENSION, &PKG::open);
};

} // namespace vpkpp
