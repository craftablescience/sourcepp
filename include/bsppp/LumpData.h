#pragma once

#include <sourcepp/math/Vector.h>

namespace bsppp {

//region Lump 1 (Planes)

struct BSPPlane_v0 {
	sourcepp::math::Vec3f normal;
	float dist;
	int32_t type;
};

using BSPPlane = BSPPlane_v0;

//endregion

//region Lump 2 (Texture Data)

struct BSPTextureData_v0 {
	sourcepp::math::Vec3f reflectivity;
	int32_t nameStringTableID;
	int32_t width;
	int32_t height;
	int32_t viewWidth;
	int32_t viewHeight;
};

using BSPTextureData = BSPTextureData_v0;

//endregion

//region Lump 3 (Vertices)

struct BSPVertex_v0 {
	sourcepp::math::Vec3f position;
};

using BSPVertex = BSPVertex_v0;

//endregion

//region Lump 6 (Texture Info)

struct BSPTextureInfo_v0 {
	sourcepp::math::Vec4f textureVectors[2];
	sourcepp::math::Vec4f lightmapVectors[2];
	int32_t flags;
	int32_t textureData;
};

using BSPTextureInfo = BSPTextureInfo_v0;

//endregion

//region Lump 7/27 (Faces/Original Faces)

struct BSPFace_v1 {
	uint16_t planeNum;
	uint8_t side;
	uint8_t onNode;
	int32_t firstEdge;
	int16_t numEdges;
	int16_t texInfo;
	int16_t dispInfo;
	int16_t surfaceFogVolumeID;
	uint8_t styles[4];
	int32_t lightOffset;
	float area;
	int32_t lightmapTextureMinsInLuxels[2];
	int32_t lightmapTextureSizeInLuxels[2];
	int32_t originalFace;
	uint16_t numPrims;
	uint16_t firstPrimID;
	uint32_t smoothingGroups;
};

struct BSPFace_v2 {
	uint32_t planeNum;
	uint8_t side;
	uint8_t onNode;
	int32_t firstEdge;
	int32_t numEdges;
	int32_t texInfo;
	int32_t dispInfo;
	int32_t surfaceFogVolumeID;
	uint8_t styles[4];
	int32_t lightOffset;
	float area;
	int32_t lightmapTextureMinsInLuxels[2];
	int32_t lightmapTextureSizeInLuxels[2];
	int32_t originalFace;
	uint32_t enableShadows : 1;
	uint32_t numPrims : 31;
	uint32_t firstPrimID;
	uint32_t smoothingGroups;

	[[nodiscard]] static BSPFace_v2 upgrade(const BSPFace_v1& old) {
		return {
			old.planeNum,
			old.side,
			old.onNode,
			old.firstEdge,
			old.numEdges,
			old.texInfo,
			old.dispInfo,
			old.surfaceFogVolumeID,
			old.styles[0],
			old.styles[1],
			old.styles[2],
			old.styles[3],
			old.lightOffset,
			old.area,
			old.lightmapTextureMinsInLuxels[0],
			old.lightmapTextureMinsInLuxels[1],
			old.lightmapTextureSizeInLuxels[0],
			old.lightmapTextureSizeInLuxels[1],
			old.originalFace,
			1,
			old.numPrims,
			old.firstPrimID,
			old.smoothingGroups,
		};
	}
};

using BSPFace = BSPFace_v2;

//endregion

//region Lump 12 (Edges)

struct BSPEdge_v0 {
	uint16_t v0;
	uint16_t v1;
};

struct BSPEdge_v1 {
	uint32_t v0;
	uint32_t v1;

	[[nodiscard]] static BSPEdge_v1 upgrade(const BSPEdge_v0& old) {
		return {
			old.v0,
			old.v1,
		};
	}
};

using BSPEdge = BSPEdge_v1;

//endregion

//region Lump 13 (SurfEdges)

struct BSPSurfEdge_v0 {
	int32_t surfEdge;
};

using BSPSurfEdge = BSPSurfEdge_v0;

//endregion

//region Lump 14 (Brush Models)

struct BSPBrushModel_v0 {
	sourcepp::math::Vec3f min;
	sourcepp::math::Vec3f max;
	sourcepp::math::Vec3f origin;
	int32_t headNode;
	int32_t firstFace;
	int32_t numFaces;
};

using BSPBrushModel = BSPBrushModel_v0;

//endregion

} // namespace bsppp
