#pragma once

#include <vector>

#include <sourcepp/Mesh.h>

#include "structs/MDL.h"
#include "structs/VTX.h"
#include "structs/VVD.h"

namespace mdlpp {

/**
 * A more accessible version of StudioModel's vertex data, so it can be rendered or converted more easily.
 */
struct BakedModel {
	struct Mesh {
		std::vector<uint16_t> indices;
		int32_t materialIndex = -1;
	};

	std::vector<sourcepp::mesh::VertexStatic> vertices;
	std::vector<Mesh> meshes;
};

struct StudioModel {
	[[nodiscard]] bool open(const std::byte* mdlData, std::size_t mdlSize,
							const std::byte* vtxData, std::size_t vtxSize,
							const std::byte* vvdData, std::size_t vvdSize);

	[[nodiscard]] bool open(const unsigned char* mdlData, std::size_t mdlSize,
							const unsigned char* vtxData, std::size_t vtxSize,
							const unsigned char* vvdData, std::size_t vvdSize);

	[[nodiscard]] bool open(const std::vector<std::byte>& mdlData,
							const std::vector<std::byte>& vtxData,
							const std::vector<std::byte>& vvdData);

	[[nodiscard]] bool open(const std::vector<unsigned char>& mdlData,
							const std::vector<unsigned char>& vtxData,
							const std::vector<unsigned char>& vvdData);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] BakedModel processModelData(int currentLOD = ROOT_LOD) const;

	MDL::MDL mdl;
	VTX::VTX vtx;
	VVD::VVD vvd;

private:
	bool opened = false;
};

} // namespace mdlpp
