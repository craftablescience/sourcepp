#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "../math/Vector.h"

namespace studiomodelpp::VTX {

struct Vertex {
	//unsigned char boneWeightIndex[3];
	//unsigned char boneCount;
	unsigned short meshVertexID;
	//char boneID[3];
};

struct Strip {
	enum Flags : unsigned char {
		FLAG_NONE = 0,
		FLAG_IS_TRILIST = 1 << 0,
		FLAG_IS_TRISTRIP = 1 << 1,
	};

	//int indicesCount;
	//int indicesOffset;
	std::span<unsigned short> indices;

	//int verticesCount;
	//int verticesOffset;
	std::span<Vertex> vertices;

	short boneCount = 0;
	Flags flags = FLAG_NONE;

	//int boneStateChangeCount;
	//int boneStateChangeOffset;

	// On MDL version >= 49:
	//int numTopologyIndices;
	//int topologyOffset;
};

struct StripGroup {
	enum Flags : unsigned char {
		FLAG_NONE = 0,
		FLAG_IS_FLEXED = 1 << 0,
		FLAG_IS_HW_SKINNED = 1 << 1,
		FLAG_IS_DELTA_FLEXED = 1 << 2,
		FLAG_SUPPRESS_HW_MORPH = 1 << 3,
	};

	int vertexCount;
	int vertexOffset;
	std::vector<Vertex> vertices;

	int indexCount;
	int indexOffset;
	std::vector<unsigned short> indices;

	int stripCount;
	int stripOffset;
	std::vector<Strip> strips;

	Flags flags;

	// on MDL version >= 49:
	//int numTopologyIndices;
	//int topologyOffset;
};

struct Mesh {
	enum Flags : unsigned char {
		FLAG_NONE = 0,
		FLAG_IS_TEETH = 1 << 0,
		FLAG_IS_EYES = 1 << 1,
	};

	//int stripGroupCount;
	//int stripGroupHeaderOffset;
	std::vector<StripGroup> stripGroups;

	Flags flags;
};

struct ModelLOD {
	//int meshCount;
	//int meshOffset;
	std::vector<Mesh> meshes;

	float switchDistance;
};

struct Model {
	//int LODCount;
	//int LODOffset;
	std::vector<ModelLOD> modelLODs;
};

struct BodyPart {
	//int modelCount;
	//int modelOffset;
	std::vector<Model> models;
};

struct VTX {
	[[nodiscard]] bool open(const std::byte* data, std::size_t size, int mdlVersion, int mdlChecksum);

	int version;
	int vertexCacheSize;
	unsigned short maxBonesPerStrip;
	unsigned short maxBonesPerTriangle;
	int maxBonesPerVertex;
	//int checksum;
	int numLODs;

	//int materialReplacementListOffset;

	//int bodyPartCount;
	//int bodyPartOffset;
	std::vector<BodyPart> bodyParts;
};

} // namespace studiomodelpp::VTX
