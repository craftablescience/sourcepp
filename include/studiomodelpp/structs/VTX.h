#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "MDL.h"

namespace studiomodelpp::VTX {

struct Vertex {
	//uint8_t boneWeightIndex[3];
	//uint8_t boneCount;
	uint16_t meshVertexID;
	//int8_t boneID[3];
};

struct Strip {
	enum Flags : uint8_t {
		FLAG_NONE = 0,
		FLAG_IS_TRILIST = 1 << 0,
		FLAG_IS_TRISTRIP = 1 << 1,
	};

	//int32_t indicesCount;
	int32_t indicesOffset = 0; // Offset from the start of the strip group indices
	std::span<uint16_t> indices;

	//int32_t verticesCount;
	int32_t verticesOffset = 0; // Offset from the start of the strip group vertices
	std::span<Vertex> vertices;

	int16_t boneCount = 0;
	Flags flags = FLAG_NONE;

	//int32_t boneStateChangeCount;
	//int32_t boneStateChangeOffset;

	// On MDL version >= 49:
	//int32_t numTopologyIndices;
	//int32_t topologyOffset;
};

struct StripGroup {
	enum Flags : uint8_t {
		FLAG_NONE = 0,
		FLAG_IS_FLEXED = 1 << 0,
		FLAG_IS_HW_SKINNED = 1 << 1,
		FLAG_IS_DELTA_FLEXED = 1 << 2,
		FLAG_SUPPRESS_HW_MORPH = 1 << 3,
	};

	//int32_t vertexCount;
	//int32_t vertexOffset;
	std::vector<Vertex> vertices;

	//int32_t indexCount;
	//int32_t indexOffset;
	std::vector<uint16_t> indices;

	//int32_t stripCount;
	//int32_t stripOffset;
	std::vector<Strip> strips;

	Flags flags;

	// on MDL version >= 49:
	//int32_t numTopologyIndices;
	//int32_t topologyOffset;
};

struct Mesh {
	enum Flags : uint8_t {
		FLAG_NONE = 0,
		FLAG_IS_TEETH = 1 << 0,
		FLAG_IS_EYES = 1 << 1,
	};

	//int32_t stripGroupCount;
	//int32_t stripGroupHeaderOffset;
	std::vector<StripGroup> stripGroups;

	Flags flags;
};

struct ModelLOD {
	//int32_t meshCount;
	//int32_t meshOffset;
	std::vector<Mesh> meshes;

	float switchDistance;
};

struct Model {
	//int32_t LODCount;
	//int32_t LODOffset;
	std::vector<ModelLOD> modelLODs;
};

struct BodyPart {
	//int32_t modelCount;
	//int32_t modelOffset;
	std::vector<Model> models;
};

struct VTX {
	[[nodiscard]] bool open(const std::byte* data, std::size_t size, const MDL::MDL& mdl);

	int32_t version;
	int32_t vertexCacheSize;
	uint16_t maxBonesPerStrip;
	uint16_t maxBonesPerTriangle;
	int32_t maxBonesPerVertex;
	//int32_t checksum;
	int32_t numLODs;

	//int32_t materialReplacementListOffset;

	//int32_t bodyPartCount;
	//int32_t bodyPartOffset;
	std::vector<BodyPart> bodyParts;
};

} // namespace studiomodelpp::VTX
