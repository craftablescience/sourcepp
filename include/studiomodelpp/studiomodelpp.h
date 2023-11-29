#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "structs/MDL.h"
#include "structs/VTX.h"
#include "structs/VVD.h"

namespace studiomodelpp {

struct StudioModel {
	bool open(const std::byte* mdlData, std::size_t mdlSize,
	          const std::byte* vtxData, std::size_t vtxSize,
	          const std::byte* vvdData, std::size_t vvdSize);

	bool open(const unsigned char* mdlData, std::size_t mdlSize,
	          const unsigned char* vtxData, std::size_t vtxSize,
	          const unsigned char* vvdData, std::size_t vvdSize);

	bool open(const std::vector<std::byte>& mdlData,
	          const std::vector<std::byte>& vtxData,
	          const std::vector<std::byte>& vvdData);

	bool open(const std::vector<unsigned char>& mdlData,
	          const std::vector<unsigned char>& vtxData,
	          const std::vector<unsigned char>& vvdData);

	MDL::MDL mdl;
	VTX::VTX vtx;
	VVD::VVD vvd;
};

} // namespace studiomodelpp
