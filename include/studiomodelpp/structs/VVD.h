#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "../math/Vector.h"
#include "Generic.h"
#include "MDL.h"

namespace studiomodelpp::VVD {

struct Fixup {
	int LOD;
	int sourceVertexID;
	int vertexCount;
};

struct BoneWeight {
	//float weight[MAX_BONES_PER_VERTEX];
    std::array<float, MAX_BONES_PER_VERTEX> weight;

	//char bone[MAX_BONES_PER_VERTEX];
	//char boneCount;
    std::vector<char> bones;
};

struct Vertex {
	BoneWeight boneWeight;

	Vector3 position{};
	Vector3 normal{};
	Vector2 uv{};

	Vector4 tangent{}; // Taken from tangents data section
};

struct VVD {
	[[nodiscard]] bool open(const std::byte* data, std::size_t size, const MDL::MDL& mdl);

	//int id;
	int version;
	//int checksum;

	int numLODs;
	//int numVerticesInLOD[MAX_LOD_COUNT];
	std::array<int, MAX_LOD_COUNT> numVerticesInLOD;

	//int fixupsCount;
	//int fixupsOffset;
	std::vector<Fixup> fixups;

	//int verticesOffset;
	std::vector<Vertex> vertices;
	//int tangentsOffset;
};

} // namespace studiomodelpp::VVD
