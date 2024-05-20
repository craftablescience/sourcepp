#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "MDL.h"

namespace studiomodelpp::VVD {

struct Fixup {
	int32_t LOD;
	int32_t sourceVertexID;
	int32_t vertexCount;
};

struct BoneWeight {
	//float weight[MAX_BONES_PER_VERTEX];
    std::array<float, MAX_BONES_PER_VERTEX> weight;

	//int8_t bone[MAX_BONES_PER_VERTEX];
	//int8_t boneCount;
    std::vector<int8_t> bones;
};

struct Vertex {
	BoneWeight boneWeight;

	sourcepp::Vec3f position{};
	sourcepp::Vec3f normal{};
	sourcepp::Vec2f uv{};

	sourcepp::Vec4f tangent{}; // Taken from tangents data section
};

struct VVD {
	[[nodiscard]] bool open(const std::byte* data, std::size_t size, const MDL::MDL& mdl);

	//int32_t id;
	int32_t version;
	//int32_t checksum;

	int32_t numLODs;
	//int32_t numVerticesInLOD[MAX_LOD_COUNT];
	std::array<int32_t, MAX_LOD_COUNT> numVerticesInLOD;

	//int32_t fixupsCount;
	//int32_t fixupsOffset;
	std::vector<Fixup> fixups;

	//int32_t verticesOffset;
	std::vector<Vertex> vertices;
	//int32_t tangentsOffset;
};

} // namespace studiomodelpp::VVD
