/*
 * The PKG::Asset and PKG::Blob structs, originally titled AssetDesc and BlobDesc,
 * are both taken from the RTX Remix source code:
 * https://github.com/NVIDIAGameWorks/dxvk-remix/tree/main/src/dxvk/rtx_render/rtx_asset_package.h
 * The appropriate license for these structs is as follows:
 *
 * Copyright (c) 2021-2023, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr uint32_t PKG_SIGNATURE = 0xbaadd00d; // bad dude?
constexpr std::string_view PKG_EXTENSION = ".pkg";

class PKG : public PackFileReadOnly {
protected:
	struct Header {
		uint32_t signature;
		uint32_t version;
		uint64_t treeOffset;
	};

public:
	struct Asset {
		enum class Type : uint8_t {
			UNKNOWN,
			IMAGE_1D,
			IMAGE_2D,
			IMAGE_3D,
			IMAGE_CUBE,
			BUFFER,
		};
		uint16_t nameIdx;
		Type type;
		uint8_t format;
		union {
			uint32_t size;
			struct {
				uint16_t width;
				uint16_t height;
			};
		};
		uint16_t depth;
		uint16_t numMips;
		uint16_t numTailMips;
		uint16_t arraySize;
		uint16_t baseBlobIdx;
		uint16_t tailBlobIdx;
	};

	struct Blob {
		uint64_t offset : 40;
		uint64_t compression : 8;
		uint64_t flags : 8;
		uint32_t size;
		uint32_t crc32;
	};

	struct File {
		Header header;
		std::vector<Asset> assets;
		std::vector<Blob> blobs;
	};

	/// Open a PKG file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const Callback& callback = nullptr);

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const Entry& entry) const override;

	[[nodiscard]] std::string getTruncatedFilestem() const override;

	[[nodiscard]] std::vector<Attribute> getSupportedEntryAttributes() const override;

	/// Get raw, unbaked file data
	[[nodiscard]] const std::unordered_map<uint16_t, File>& getRawFileData() const;

protected:
	PKG(const std::string& fullFilePath_, PackFileOptions options_);

	static void openNumbered(PKG* pkg, uint16_t index, std::string_view path, const Callback& callback);

	std::unordered_map<uint16_t, File> files;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PKG_EXTENSION, &PKG::open);
};

} // namespace vpkpp
