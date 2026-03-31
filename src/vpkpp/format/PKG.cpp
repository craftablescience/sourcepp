// ReSharper disable CppRedundantParentheses
// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/PKG.h>

#include <filesystem>

#include <FileStream.h>
#include <GDeflate.h>
#include <sourcepp/parser/Binary.h>

using namespace sourcepp;
using namespace vpkpp;

namespace {

//NOLINTBEGIN(*-branch-clone)

// Convert some Vulkan 1.0 formats to corresponding DXGI formats
[[nodiscard]] constexpr uint32_t mapVkFormatToDXGIFormat(uint32_t format) {
	switch (format) {
		case 3 /*VK_FORMAT_B4G4R4A4_UNORM_PACK16*/:       return 115 /*DXGI_FORMAT_B4G4R4A4_UNORM*/;
		case 7 /*VK_FORMAT_B5G5R5A1_UNORM_PACK16*/:       return 86 /*DXGI_FORMAT_B5G5R5A1_UNORM*/;
		case 9 /*VK_FORMAT_R8_UNORM*/:                    return 61 /*DXGI_FORMAT_R8_UNORM*/;
		case 10 /*VK_FORMAT_R8_SNORM*/:                   return 63 /*DXGI_FORMAT_R8_SNORM*/;
		case 11 /*VK_FORMAT_R8_USCALED*/:                 return 62 /*DXGI_FORMAT_R8_UINT*/;
		case 12 /*VK_FORMAT_R8_SSCALED*/:                 return 64 /*DXGI_FORMAT_R8_SINT*/;
		case 13 /*VK_FORMAT_R8_UINT*/:                    return 62 /*DXGI_FORMAT_R8_UINT*/;
		case 14 /*VK_FORMAT_R8_SINT*/:                    return 64 /*DXGI_FORMAT_R8_SINT*/;
		case 16 /*VK_FORMAT_R8G8_UNORM*/:                 return 49 /*DXGI_FORMAT_R8G8_UNORM*/;
		case 17 /*VK_FORMAT_R8G8_SNORM*/:                 return 51 /*DXGI_FORMAT_R8G8_SNORM*/;
		case 18 /*VK_FORMAT_R8G8_USCALED*/:               return 49 /*DXGI_FORMAT_R8G8_UNORM*/;
		case 19 /*VK_FORMAT_R8G8_SSCALED*/:               return 51 /*DXGI_FORMAT_R8G8_SNORM*/;
		case 20 /*VK_FORMAT_R8G8_UINT*/:                  return 50 /*DXGI_FORMAT_R8G8_UINT*/;
		case 21 /*VK_FORMAT_R8G8_SINT*/:                  return 52 /*DXGI_FORMAT_R8G8_SINT*/;
		case 37 /*VK_FORMAT_R8G8B8A8_UNORM*/:             return 28 /*DXGI_FORMAT_R8G8B8A8_UNORM*/;
		case 38 /*VK_FORMAT_R8G8B8A8_SNORM*/:             return 31 /*DXGI_FORMAT_R8G8B8A8_SNORM*/;
		case 39 /*VK_FORMAT_R8G8B8A8_USCALED*/:           return 30 /*DXGI_FORMAT_R8G8B8A8_UINT*/;
		case 40 /*VK_FORMAT_R8G8B8A8_SSCALED*/:           return 32 /*DXGI_FORMAT_R8G8B8A8_SINT*/;
		case 41 /*VK_FORMAT_R8G8B8A8_UINT*/:              return 30 /*DXGI_FORMAT_R8G8B8A8_UINT*/;
		case 42 /*VK_FORMAT_R8G8B8A8_SINT*/:              return 32 /*DXGI_FORMAT_R8G8B8A8_SINT*/;
		case 43 /*VK_FORMAT_R8G8B8A8_SRGB*/:              return 29 /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/;
		case 44 /*VK_FORMAT_B8G8R8A8_UNORM*/:             return 87 /*DXGI_FORMAT_B8G8R8A8_UNORM*/;
		case 50 /*VK_FORMAT_B8G8R8A8_SRGB*/:              return 91 /*DXGI_FORMAT_B8G8R8A8_UNORM_SRGB*/;
		case 64 /*VK_FORMAT_A2B10G10R10_UNORM_PACK32*/:   return 24 /*DXGI_FORMAT_R10G10B10A2_UNORM*/;
		case 66 /*VK_FORMAT_A2B10G10R10_USCALED_PACK32*/: return 25 /*DXGI_FORMAT_R10G10B10A2_UINT*/;
		case 68 /*VK_FORMAT_A2B10G10R10_UINT_PACK32*/:    return 25 /*DXGI_FORMAT_R10G10B10A2_UINT*/;
		case 70 /*VK_FORMAT_R16_UNORM*/:                  return 56 /*DXGI_FORMAT_R16_UNORM*/;
		case 71 /*VK_FORMAT_R16_SNORM*/:                  return 58 /*DXGI_FORMAT_R16_SNORM*/;
		case 72 /*VK_FORMAT_R16_USCALED*/:                return 57 /*DXGI_FORMAT_R16_UINT*/;
		case 73 /*VK_FORMAT_R16_SSCALED*/:                return 59 /*DXGI_FORMAT_R16_SINT*/;
		case 74 /*VK_FORMAT_R16_UINT*/:                   return 57 /*DXGI_FORMAT_R16_UINT*/;
		case 75 /*VK_FORMAT_R16_SINT*/:                   return 59 /*DXGI_FORMAT_R16_SINT*/;
		case 76 /*VK_FORMAT_R16_SFLOAT*/:                 return 54 /*DXGI_FORMAT_R16_FLOAT*/;
		case 77 /*VK_FORMAT_R16G16_UNORM*/:               return 35 /*DXGI_FORMAT_R16G16_UNORM*/;
		case 78 /*VK_FORMAT_R16G16_SNORM*/:               return 37 /*DXGI_FORMAT_R16G16_SNORM*/;
		case 79 /*VK_FORMAT_R16G16_USCALED*/:             return 36 /*DXGI_FORMAT_R16G16_UINT*/;
		case 80 /*VK_FORMAT_R16G16_SSCALED*/:             return 38 /*DXGI_FORMAT_R16G16_SINT*/;
		case 81 /*VK_FORMAT_R16G16_UINT*/:                return 36 /*DXGI_FORMAT_R16G16_UINT*/;
		case 82 /*VK_FORMAT_R16G16_SINT*/:                return 38 /*DXGI_FORMAT_R16G16_SINT*/;
		case 83 /*VK_FORMAT_R16G16_SFLOAT*/:              return 34 /*DXGI_FORMAT_R16G16_FLOAT*/;
		case 91 /*VK_FORMAT_R16G16B16A16_UNORM*/:         return 11 /*DXGI_FORMAT_R16G16B16A16_UNORM*/;
		case 92 /*VK_FORMAT_R16G16B16A16_SNORM*/:         return 13 /*DXGI_FORMAT_R16G16B16A16_SNORM*/;
		case 93 /*VK_FORMAT_R16G16B16A16_USCALED*/:       return 12 /*DXGI_FORMAT_R16G16B16A16_UINT*/;
		case 94 /*VK_FORMAT_R16G16B16A16_SSCALED*/:       return 14 /*DXGI_FORMAT_R16G16B16A16_SINT*/;
		case 95 /*VK_FORMAT_R16G16B16A16_UINT*/:          return 12 /*DXGI_FORMAT_R16G16B16A16_UINT*/;
		case 96 /*VK_FORMAT_R16G16B16A16_SINT*/:          return 14 /*DXGI_FORMAT_R16G16B16A16_SINT*/;
		case 97 /*VK_FORMAT_R16G16B16A16_SFLOAT*/:        return 10 /*DXGI_FORMAT_R16G16B16A16_FLOAT*/;
		case 98 /*VK_FORMAT_R32_UINT*/:                   return 42 /*DXGI_FORMAT_R32_UINT*/;
		case 99 /*VK_FORMAT_R32_SINT*/:                   return 43 /*DXGI_FORMAT_R32_SINT*/;
		case 100 /*VK_FORMAT_R32_SFLOAT*/:                return 41 /*DXGI_FORMAT_R32_FLOAT*/;
		case 101 /*VK_FORMAT_R32G32_UINT*/:               return 17 /*DXGI_FORMAT_R32G32_UINT*/;
		case 102 /*VK_FORMAT_R32G32_SINT*/:               return 18 /*DXGI_FORMAT_R32G32_SINT*/;
		case 103 /*VK_FORMAT_R32G32_SFLOAT*/:             return 16 /*DXGI_FORMAT_R32G32_FLOAT*/;
		case 104 /*VK_FORMAT_R32G32B32_UINT*/:            return 7 /*DXGI_FORMAT_R32G32B32_UINT*/;
		case 105 /*VK_FORMAT_R32G32B32_SINT*/:            return 8 /*DXGI_FORMAT_R32G32B32_SINT*/;
		case 106 /*VK_FORMAT_R32G32B32_SFLOAT*/:          return 6 /*DXGI_FORMAT_R32G32B32_FLOAT*/;
		case 107 /*VK_FORMAT_R32G32B32A32_UINT*/:         return 3 /*DXGI_FORMAT_R32G32B32A32_UINT*/;
		case 108 /*VK_FORMAT_R32G32B32A32_SINT*/:         return 4 /*DXGI_FORMAT_R32G32B32A32_SINT*/;
		case 109 /*VK_FORMAT_R32G32B32A32_SFLOAT*/:       return 2 /*DXGI_FORMAT_R32G32B32A32_FLOAT*/;
		case 122 /*VK_FORMAT_B10G11R11_UFLOAT_PACK32*/:   return 26 /*DXGI_FORMAT_R11G11B10_FLOAT*/;
		case 123 /*VK_FORMAT_E5B9G9R9_UFLOAT_PACK32*/:    return 67 /*DXGI_FORMAT_R9G9B9E5_SHAREDEXP*/;
		case 124 /*VK_FORMAT_D16_UNORM*/:                 return 55 /*DXGI_FORMAT_D16_UNORM*/;
		case 126 /*VK_FORMAT_D32_SFLOAT*/:                return 40 /*DXGI_FORMAT_D32_FLOAT*/;
		case 129 /*VK_FORMAT_D24_UNORM_S8_UINT*/:         return 45 /*DXGI_FORMAT_D24_UNORM_S8_UINT*/;
		case 131 /*VK_FORMAT_BC1_RGB_UNORM_BLOCK*/:       return 71 /*DXGI_FORMAT_BC1_UNORM*/;
		case 132 /*VK_FORMAT_BC1_RGB_SRGB_BLOCK*/:        return 72 /*DXGI_FORMAT_BC1_UNORM_SRGB*/;
		case 133 /*VK_FORMAT_BC1_RGBA_UNORM_BLOCK*/:      return 71 /*DXGI_FORMAT_BC1_UNORM*/;
		case 134 /*VK_FORMAT_BC1_RGBA_SRGB_BLOCK*/:       return 72 /*DXGI_FORMAT_BC1_UNORM_SRGB*/;
		case 135 /*VK_FORMAT_BC2_UNORM_BLOCK*/:           return 74 /*DXGI_FORMAT_BC2_UNORM*/;
		case 136 /*VK_FORMAT_BC2_SRGB_BLOCK*/:            return 75 /*DXGI_FORMAT_BC2_UNORM_SRGB*/;
		case 137 /*VK_FORMAT_BC3_UNORM_BLOCK*/:           return 77 /*DXGI_FORMAT_BC3_UNORM*/;
		case 138 /*VK_FORMAT_BC3_SRGB_BLOCK*/:            return 78 /*DXGI_FORMAT_BC3_UNORM_SRGB*/;
		case 139 /*VK_FORMAT_BC4_UNORM_BLOCK*/:           return 80 /*DXGI_FORMAT_BC4_UNORM*/;
		case 140 /*VK_FORMAT_BC4_SNORM_BLOCK*/:           return 81 /*DXGI_FORMAT_BC4_SNORM*/;
		case 141 /*VK_FORMAT_BC5_UNORM_BLOCK*/:           return 83 /*DXGI_FORMAT_BC5_UNORM*/;
		case 142 /*VK_FORMAT_BC5_SNORM_BLOCK*/:           return 84 /*DXGI_FORMAT_BC5_SNORM*/;
		case 143 /*VK_FORMAT_BC6H_UFLOAT_BLOCK*/:         return 95 /*DXGI_FORMAT_BC6H_UF16*/;
		case 144 /*VK_FORMAT_BC6H_SFLOAT_BLOCK*/:         return 96 /*DXGI_FORMAT_BC6H_SF16*/;
		case 145 /*VK_FORMAT_BC7_UNORM_BLOCK*/:           return 98 /*DXGI_FORMAT_BC7_UNORM*/;
		case 146 /*VK_FORMAT_BC7_SRGB_BLOCK*/:            return 99 /*DXGI_FORMAT_BC7_UNORM_SRGB*/;
		default:                                          break;
	}
	return 0 /*DXGI_FORMAT_UNKNOWN*/;
}

[[nodiscard]] constexpr uint32_t getVkFormatBitsPerPixel(uint32_t format) {
	switch (format) {
		case 3 /*VK_FORMAT_B4G4R4A4_UNORM_PACK16*/:       return 16;
		case 7 /*VK_FORMAT_B5G5R5A1_UNORM_PACK16*/:       return 16;
		case 9 /*VK_FORMAT_R8_UNORM*/:                    return 8;
		case 10 /*VK_FORMAT_R8_SNORM*/:                   return 8;
		case 11 /*VK_FORMAT_R8_USCALED*/:                 return 8;
		case 12 /*VK_FORMAT_R8_SSCALED*/:                 return 8;
		case 13 /*VK_FORMAT_R8_UINT*/:                    return 8;
		case 14 /*VK_FORMAT_R8_SINT*/:                    return 8;
		case 16 /*VK_FORMAT_R8G8_UNORM*/:                 return 16;
		case 17 /*VK_FORMAT_R8G8_SNORM*/:                 return 16;
		case 18 /*VK_FORMAT_R8G8_USCALED*/:               return 16;
		case 19 /*VK_FORMAT_R8G8_SSCALED*/:               return 16;
		case 20 /*VK_FORMAT_R8G8_UINT*/:                  return 16;
		case 21 /*VK_FORMAT_R8G8_SINT*/:                  return 16;
		case 37 /*VK_FORMAT_R8G8B8A8_UNORM*/:             return 32;
		case 38 /*VK_FORMAT_R8G8B8A8_SNORM*/:             return 32;
		case 39 /*VK_FORMAT_R8G8B8A8_USCALED*/:           return 32;
		case 40 /*VK_FORMAT_R8G8B8A8_SSCALED*/:           return 32;
		case 41 /*VK_FORMAT_R8G8B8A8_UINT*/:              return 32;
		case 42 /*VK_FORMAT_R8G8B8A8_SINT*/:              return 32;
		case 43 /*VK_FORMAT_R8G8B8A8_SRGB*/:              return 32;
		case 44 /*VK_FORMAT_B8G8R8A8_UNORM*/:             return 32;
		case 50 /*VK_FORMAT_B8G8R8A8_SRGB*/:              return 32;
		case 64 /*VK_FORMAT_A2B10G10R10_UNORM_PACK32*/:   return 32;
		case 66 /*VK_FORMAT_A2B10G10R10_USCALED_PACK32*/: return 32;
		case 68 /*VK_FORMAT_A2B10G10R10_UINT_PACK32*/:    return 32;
		case 70 /*VK_FORMAT_R16_UNORM*/:                  return 16;
		case 71 /*VK_FORMAT_R16_SNORM*/:                  return 16;
		case 72 /*VK_FORMAT_R16_USCALED*/:                return 16;
		case 73 /*VK_FORMAT_R16_SSCALED*/:                return 16;
		case 74 /*VK_FORMAT_R16_UINT*/:                   return 16;
		case 75 /*VK_FORMAT_R16_SINT*/:                   return 16;
		case 76 /*VK_FORMAT_R16_SFLOAT*/:                 return 16;
		case 77 /*VK_FORMAT_R16G16_UNORM*/:               return 32;
		case 78 /*VK_FORMAT_R16G16_SNORM*/:               return 32;
		case 79 /*VK_FORMAT_R16G16_USCALED*/:             return 32;
		case 80 /*VK_FORMAT_R16G16_SSCALED*/:             return 32;
		case 81 /*VK_FORMAT_R16G16_UINT*/:                return 32;
		case 82 /*VK_FORMAT_R16G16_SINT*/:                return 32;
		case 83 /*VK_FORMAT_R16G16_SFLOAT*/:              return 32;
		case 91 /*VK_FORMAT_R16G16B16A16_UNORM*/:         return 64;
		case 92 /*VK_FORMAT_R16G16B16A16_SNORM*/:         return 64;
		case 93 /*VK_FORMAT_R16G16B16A16_USCALED*/:       return 64;
		case 94 /*VK_FORMAT_R16G16B16A16_SSCALED*/:       return 64;
		case 95 /*VK_FORMAT_R16G16B16A16_UINT*/:          return 64;
		case 96 /*VK_FORMAT_R16G16B16A16_SINT*/:          return 64;
		case 97 /*VK_FORMAT_R16G16B16A16_SFLOAT*/:        return 64;
		case 98 /*VK_FORMAT_R32_UINT*/:                   return 32;
		case 99 /*VK_FORMAT_R32_SINT*/:                   return 32;
		case 100 /*VK_FORMAT_R32_SFLOAT*/:                return 32;
		case 101 /*VK_FORMAT_R32G32_UINT*/:               return 64;
		case 102 /*VK_FORMAT_R32G32_SINT*/:               return 64;
		case 103 /*VK_FORMAT_R32G32_SFLOAT*/:             return 64;
		case 104 /*VK_FORMAT_R32G32B32_UINT*/:            return 96;
		case 105 /*VK_FORMAT_R32G32B32_SINT*/:            return 96;
		case 106 /*VK_FORMAT_R32G32B32_SFLOAT*/:          return 96;
		case 107 /*VK_FORMAT_R32G32B32A32_UINT*/:         return 128;
		case 108 /*VK_FORMAT_R32G32B32A32_SINT*/:         return 128;
		case 109 /*VK_FORMAT_R32G32B32A32_SFLOAT*/:       return 128;
		case 122 /*VK_FORMAT_B10G11R11_UFLOAT_PACK32*/:   return 32;
		case 123 /*VK_FORMAT_E5B9G9R9_UFLOAT_PACK32*/:    return 32;
		case 124 /*VK_FORMAT_D16_UNORM*/:                 return 16;
		case 126 /*VK_FORMAT_D32_SFLOAT*/:                return 32;
		case 129 /*VK_FORMAT_D24_UNORM_S8_UINT*/:         return 32;
		case 131 /*VK_FORMAT_BC1_RGB_UNORM_BLOCK*/:       return 4;
		case 132 /*VK_FORMAT_BC1_RGB_SRGB_BLOCK*/:        return 4;
		case 133 /*VK_FORMAT_BC1_RGBA_UNORM_BLOCK*/:      return 4;
		case 134 /*VK_FORMAT_BC1_RGBA_SRGB_BLOCK*/:       return 4;
		case 135 /*VK_FORMAT_BC2_UNORM_BLOCK*/:           return 8;
		case 136 /*VK_FORMAT_BC2_SRGB_BLOCK*/:            return 8;
		case 137 /*VK_FORMAT_BC3_UNORM_BLOCK*/:           return 8;
		case 138 /*VK_FORMAT_BC3_SRGB_BLOCK*/:            return 8;
		case 139 /*VK_FORMAT_BC4_UNORM_BLOCK*/:           return 4;
		case 140 /*VK_FORMAT_BC4_SNORM_BLOCK*/:           return 4;
		case 141 /*VK_FORMAT_BC5_UNORM_BLOCK*/:           return 8;
		case 142 /*VK_FORMAT_BC5_SNORM_BLOCK*/:           return 8;
		case 143 /*VK_FORMAT_BC6H_UFLOAT_BLOCK*/:         return 8;
		case 144 /*VK_FORMAT_BC6H_SFLOAT_BLOCK*/:         return 8;
		case 145 /*VK_FORMAT_BC7_UNORM_BLOCK*/:           return 8;
		case 146 /*VK_FORMAT_BC7_SRGB_BLOCK*/:            return 8;
		default:                                          break;
	}
	return 0 /*DXGI_FORMAT_UNKNOWN*/;
}

[[nodiscard]] constexpr bool isVkFormatCompressed(uint32_t format) {
	switch (format) {
		case 131 /*VK_FORMAT_BC1_RGB_UNORM_BLOCK*/:
		case 132 /*VK_FORMAT_BC1_RGB_SRGB_BLOCK*/:
		case 133 /*VK_FORMAT_BC1_RGBA_UNORM_BLOCK*/:
		case 134 /*VK_FORMAT_BC1_RGBA_SRGB_BLOCK*/:
		case 135 /*VK_FORMAT_BC2_UNORM_BLOCK*/:
		case 136 /*VK_FORMAT_BC2_SRGB_BLOCK*/:
		case 137 /*VK_FORMAT_BC3_UNORM_BLOCK*/:
		case 138 /*VK_FORMAT_BC3_SRGB_BLOCK*/:
		case 139 /*VK_FORMAT_BC4_UNORM_BLOCK*/:
		case 140 /*VK_FORMAT_BC4_SNORM_BLOCK*/:
		case 141 /*VK_FORMAT_BC5_UNORM_BLOCK*/:
		case 142 /*VK_FORMAT_BC5_SNORM_BLOCK*/:
		case 143 /*VK_FORMAT_BC6H_UFLOAT_BLOCK*/:
		case 144 /*VK_FORMAT_BC6H_SFLOAT_BLOCK*/:
		case 145 /*VK_FORMAT_BC7_UNORM_BLOCK*/:
		case 146 /*VK_FORMAT_BC7_SRGB_BLOCK*/:
			return true;
		default:
			break;
	}
	return false;
}

//NOLINTEND(*-branch-clone)

} // namespace

uint32_t PKG::Asset::getBlobIndex(int frame, int face, int mip) const {
	if (this->type == Type::UNKNOWN || this->type == Type::BUFFER) {
		return this->startBlobIndex;
	}
	if (this->type == Type::IMAGE_CUBE) {
		frame = frame * 6 + face;
	}
	const auto looseMipCount = this->mipCount - this->tailMipCount;
	return (mip >= looseMipCount ? this->endBlobIndex : mip + this->startBlobIndex) + frame * looseMipCount;
}

std::unique_ptr<PackFile> PKG::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* pkg = new PKG{path};
	auto packFile = std::unique_ptr<PackFile>(pkg);

	FileStream reader{pkg->fullFilePath};
	reader.seek_in(0);

	if (path.length() >= 7 && string::matches(path.substr(path.length() - 7, path.length()), "_%d%d.pkg")) {
		for (int i = 0; true; i++) {
			if (
				const auto numberedPath = pkg->getTruncatedFilepath() + "_" + string::padNumber(i, 2) + PKG_EXTENSION.data();
				!pkg->openNumbered(i, numberedPath, callback)
			) {
				if (i == 0) {
					return nullptr;
				}
				break;
			}
		}
	} else if (!pkg->openNumbered(~0, std::string{pkg->getFilepath()}, callback)) {
		// Non-numbered fallback
		return nullptr;
	}

	return packFile;
}

bool PKG::openNumbered(uint16_t archiveIndex, const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return false;
	}

	FileStream reader{std::string{path}};
	reader.seek_in(0);

	if (const auto signature = reader.read<uint32_t>(); signature != PKG_SIGNATURE) {
		// File is not a PKG
		return false;
	}

	if (const auto version = reader.read<uint32_t>(); version != 1) {
		// File is an unsupported version
		return false;
	}

	// Tree offset
	reader.seek_in_u(reader.read<uint64_t>());

	this->metadata[archiveIndex] = {};
	auto& [assets, blobs] = this->metadata.at(archiveIndex);

	const auto assetCount = reader.read<uint16_t>();
	const auto blobCount = reader.read<uint16_t>();

	assets.reserve(assetCount);
	for (int i = 0; i < assetCount; i++) {
		auto& [
			nameIndex,
			type,
			format,
			width,
			height,
			depth,
			mipCount,
			tailMipCount,
			arraySize,
			startBlobIndex,
			endBlobIndex
		] = assets.emplace_back();
		reader >> nameIndex >> type >> format >> width >> height >> depth >> mipCount >> tailMipCount >> arraySize >> startBlobIndex >> endBlobIndex;
	}

	blobs.reserve(blobCount);
	for (int i = 0; i < blobCount; i++) {
		auto& [
			offset,
			compression,
			flags,
			size,
			crc32
		] = blobs.emplace_back();
		const auto offsetCompressionFlags = reader.read<uint64_t>();
		offset = offsetCompressionFlags & 0x000000ffffffffffull;
		compression = (offsetCompressionFlags & 0x0000ff0000000000ull) == 0 ? Blob::Compression::NONE : Blob::Compression::GDEFLATE;
		flags = offsetCompressionFlags & 0x00ff000000000000ull;
		reader >> size >> crc32;
	}

	std::vector<std::string> names;
	names.reserve(assetCount);
	for (uint32_t i = 0; i < assetCount; i++) {
		names.push_back(this->cleanEntryPath(reader.read_string()));
	}

	for (uint32_t i = 0; i < assetCount; i++) {
		Entry entry = createNewEntry();

		entry.archiveIndex = archiveIndex;
		entry.offset = i; // Not storing the offset! This is asset index!
		entry.length = 148; // DDS header size
		for (int j = assets[i].startBlobIndex; j <= assets[i].endBlobIndex; j++) {
			entry.length += blobs[i].size;
		}

		this->entries.emplace(names[assets[i].nameIndex], entry);

		if (callback) {
			callback(names[assets[i].nameIndex], entry);
		}
	}
	return true;
}

std::optional<std::vector<std::byte>> PKG::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->getTruncatedFilepath() + "_" + string::padNumber(entry->archiveIndex, 2) + PKG_EXTENSION.data()};
	if (!stream) {
		return std::nullopt;
	}

	const auto& [assets, blobs] = this->metadata.at(entry->archiveIndex);
	const auto& asset = assets[entry->offset];

	std::vector<std::byte> out;
	BufferStream writer{out};

	// Zeroed DDS header
	writer.pad(148);

	// Add blobs
	const auto readBlob = [&stream, &blobs, &writer](uint32_t i, uint64_t blobUncompressedSize) {
		switch (const auto& blob = blobs[i]; blob.compression) {
			case Blob::Compression::NONE: {
				writer << stream.seek_in_u(blob.offset).read_bytes(blob.size);
				break;
			}
			case Blob::Compression::GDEFLATE: {
				if (!blob.size) {
					return false;
				}
				const auto blobCompressed = stream.seek_in_u(blob.offset).read_bytes(blob.size);
				std::vector<std::byte> blobUncompressed;
				blobUncompressed.resize(blobUncompressedSize);
				if (!GDeflate::Decompress(reinterpret_cast<uint8_t*>(blobUncompressed.data()), blobUncompressed.size(), reinterpret_cast<const uint8_t*>(blobCompressed.data()), blobCompressed.size(), 1)) {
					return false;
				}
				writer << blobUncompressed;
				break;
			}
			default:
				return false;
		}
		return true;
	};

	const auto frameCount = asset.arraySize > 0 ? asset.arraySize : 1;
	const auto faceCount = asset.type == Asset::Type::IMAGE_CUBE ? 6 : 1;
	const auto looseMipCount = asset.mipCount - asset.tailMipCount;

	const auto getMipSize = [&asset](int mip) {
		if (::isVkFormatCompressed(asset.format)) {
			return (((std::max(asset.width / (1 << mip), 4) + 3) / 4) * 4) * (((std::max(asset.height / (1 << mip), 4) + 3) / 4) * 4) * std::max(asset.depth / (1 << mip), 1) * ::getVkFormatBitsPerPixel(asset.format) / 8;
		}
		return std::max(asset.width / (1 << mip), 1) * std::max(asset.height / (1 << mip), 1) * std::max(asset.depth / (1 << mip), 1) * ::getVkFormatBitsPerPixel(asset.format) / 8;
	};
	const auto getTailMipSize = [&asset, &getMipSize] {
		uint32_t count = 0;
		for (int mip = 0; mip < asset.tailMipCount; mip++) {
			count += getMipSize(asset.mipCount - mip - 1);
		}
		return count;
	};

	for (int frame = 0; frame < frameCount; frame++) {
		for (int face = 0; face < faceCount; face++) {
			for (int mip = 0; mip < looseMipCount; mip++) {
				if (!readBlob(asset.getBlobIndex(frame, face, mip), getMipSize(mip))) {
					return std::nullopt;
				}
			}
			if (asset.tailMipCount > 0 && !readBlob(asset.getBlobIndex(frame, face, looseMipCount), getTailMipSize())) {
				return std::nullopt;
			}
		}
	}
	out.resize(writer.size());

	// DDS header
	writer
		.seek(0)
		.write<uint32_t>(parser::binary::makeFourCC("DDS "))
		.write<uint32_t>(124)
		.write<uint32_t>(0x1 | 0x2 | 0x4 | 0x1000 | (asset.mipCount > 1 ? 0x20000 : 0x0) | (::isVkFormatCompressed(asset.format) ? 0x80000 : 0x8) | (asset.depth > 1 ? 0x800000 : 0))
		.write<uint32_t>(asset.height)
		.write<uint32_t>(asset.width)
		.write<uint32_t>(::isVkFormatCompressed(asset.format) ? getMipSize(0) : asset.width * ::getVkFormatBitsPerPixel(asset.format) / 8)
		.write<uint32_t>(asset.depth)
		.write<uint32_t>(asset.mipCount)
		.pad<uint32_t>(11)
		.write<uint32_t>(32)
		.write<uint32_t>(0x4)
		.write<uint32_t>(parser::binary::makeFourCC("DX10"))
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.write<uint32_t>(0)
		.pad<uint32_t>()
		.write<uint32_t>(::mapVkFormatToDXGIFormat(asset.format))
		.write<uint32_t>(asset.type == Asset::Type::IMAGE_2D || asset.type == Asset::Type::IMAGE_CUBE ? 3 : asset.type == Asset::Type::IMAGE_3D ? 4 : asset.type == Asset::Type::IMAGE_1D ? 2 : asset.type == Asset::Type::BUFFER ? 1 : 0)
		.write<uint32_t>(0)
		.write<uint32_t>(1)
		.write<uint32_t>(1);

	return out;
}

std::string PKG::getTruncatedFilestem() const {
	std::string stem = this->getFilestem();
	if (stem.length() >= 3) {
		stem = stem.substr(0, stem.length() - 3);
	}
	return stem;
}

Attribute PKG::getSupportedEntryAttributes() const {
	using enum Attribute;
	return ARCHIVE_INDEX | LENGTH;
}
