#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

#include <sourcepp/parser/Binary.h>

#include "../EntityLump.h"
#include "../PakLump.h"

namespace bsppp {

constexpr auto VBSP_SIGNATURE = sourcepp::parser::binary::makeFourCC("VBSP");
constexpr auto VBSP_CONSOLE_SIGNATURE = sourcepp::parser::binary::makeFourCC("PSBV");

enum class VBSPLump : int32_t {
	UNKNOWN = -1,
	ENTITIES = 0,
	PLANES,
	TEXDATA,
	VERTEXES,
	VISIBILITY,
	NODES,
	TEXINFO,
	FACES,
	LIGHTING,
	OCCLUSION,
	LEAFS,
	FACEIDS,
	EDGES,
	SURFEDGES,
	MODELS,
	WORLDLIGHTS,
	LEAFFACES,
	LEAFBRUSHES,
	BRUSHES,
	BRUSHSIDES,
	AREAS,
	AREAPORTALS,
	S2004_PORTALS,
	UNUSED0 = S2004_PORTALS,
	SL4D2_PROPCOLLISION = S2004_PORTALS,
	S2004_CLUSTERS,
	UNUSED1 = S2004_CLUSTERS,
	SL4D2_PROPHULLS = S2004_CLUSTERS,
	S2004_PORTALVERTS,
	UNUSED2 = S2004_PORTALVERTS,
	SL4D2_PROPHULLVERTS = S2004_PORTALVERTS,
	S2004_CLUSTERPORTALS,
	UNUSED3 = S2004_CLUSTERPORTALS,
	SL4D2_PROPTRIS = S2004_CLUSTERPORTALS,
	DISPINFO,
	ORIGINALFACES,
	PHYSDISP,
	PHYSCOLLIDE,
	VERTNORMALS,
	VERTNORMALINDICES,
	S2004_DISP_LIGHTMAP_ALPHAS,
	UNUSED4 = S2004_DISP_LIGHTMAP_ALPHAS,
	DISP_VERTS,
	DISP_LIGHTMAP_SAMPLE_POSITIONS,
	GAME_LUMP,
	LEAFWATERDATA,
	PRIMITIVES,
	PRIMVERTS,
	PRIMINDICES,
	PAKFILE,
	CLIPPORTALVERTS,
	CUBEMAPS,
	TEXDATA_STRING_DATA,
	TEXDATA_STRING_TABLE,
	OVERLAYS,
	LEAFMINDISTTOWATER,
	FACE_MACRO_TEXTURE_INFO,
	DISP_TRIS,
	S2004_PHYSCOLLIDESURFACE,
	UNUSED5 = S2004_PHYSCOLLIDESURFACE,
	SL4D2_PROP_BLOB = S2004_PHYSCOLLIDESURFACE,
	WATEROVERLAYS,
	S2006_XBOX_LIGHTMAPPAGES,
	LEAF_AMBIENT_INDEX_HDR = S2006_XBOX_LIGHTMAPPAGES,
	S2006_XBOX_LIGHTMAPPAGEINFOS,
	LEAF_AMBIENT_INDEX = S2006_XBOX_LIGHTMAPPAGEINFOS,
	LIGHTING_HDR,
	WORLDLIGHTS_HDR,
	LEAF_AMBIENT_LIGHTING_HDR,
	LEAF_AMBIENT_LIGHTING,
	XBOX_XZIPPAKFILE,
	FACES_HDR,
	MAP_FLAGS,
	OVERLAY_FADES,
	L4D_OVERLAY_SYSTEM_LEVELS,
	UNUSED6 = L4D_OVERLAY_SYSTEM_LEVELS,
	L4D2_PHYSLEVEL,
	UNUSED7 = L4D2_PHYSLEVEL,
	ASW_DISP_MULTIBLEND,
	UNUSED8 = ASW_DISP_MULTIBLEND,

	COUNT,
};
constexpr int32_t VBSP_LUMP_COUNT = 64;
static_assert(static_cast<std::underlying_type_t<VBSPLump>>(VBSPLump::COUNT) == VBSP_LUMP_COUNT, "Incorrect lump count for VBSP!");

/// Pulled from Portal 2, map e1912. This is not a given for every game or even map, and obviously lump order doesn't
/// matter one bit, but we do at least want the paklump to be at the end since it commonly grows and shrinks.
/// (Modified so lump 57 is always at the end, this is the Xbox paklump and overrides the regular paklump if present.)
constexpr std::array<uint32_t, VBSP_LUMP_COUNT> VBSP_LUMP_ORDER{
	25, 24, 32, 49, 59,  6,  2, 43,
	44, 10, 17,  1, 18, 19, 14,  5,
	20, 21,  4,  0, 29, 26, 62,  3,
	12, 13,  7, 58, 33, 48, 63, 28,
	9,  8,  53, 37, 38, 39, 30, 31,
	56, 52, 51, 55, 16, 36, 45, 50,
	60, 61, 46, 42, 41, 54, 15, 34,
	47, 11, 22, 23, 27, 35, 40, 57,
};

//region Lump 1 (Planes)

struct VBSPPlane_v0 {
	sourcepp::math::Vec3f normal;
	float dist;
	int32_t type;
};

using VBSPPlane = VBSPPlane_v0;

//endregion

//region Lump 2 (Texture Data)

struct VBSPTextureData_v0 {
	sourcepp::math::Vec3f reflectivity;
	int32_t nameStringTableID;
	int32_t width;
	int32_t height;
	int32_t viewWidth;
	int32_t viewHeight;
};

using VBSPTextureData = VBSPTextureData_v0;

//endregion

//region Lump 3 (Vertices)

struct VBSPVertex_v0 {
	sourcepp::math::Vec3f position;
};

using VBSPVertex = VBSPVertex_v0;

//endregion

//region Lump 5 (Nodes)

struct VBSPNode_v0 {
	uint32_t planeNum;
	sourcepp::math::Vec2i children;
	sourcepp::math::Vec3i16 mins;
	sourcepp::math::Vec3i16 maxs;
	uint16_t firstFace;
	uint16_t numFaces;
	int16_t area;
	uint16_t _unused0;
};

struct VBSPNode_v1 {
	uint32_t planeNum;
	sourcepp::math::Vec2i children;
	sourcepp::math::Vec3f mins;
	sourcepp::math::Vec3f maxs;
	uint32_t firstFace;
	uint32_t numFaces;
	int16_t area;

	[[nodiscard]] static VBSPNode_v1 upgrade(const VBSPNode_v0& old) {
		return {
			old.planeNum,
			{
				old.children[0],
				old.children[1],
			},
			{
				static_cast<float>(old.mins[0]),
				static_cast<float>(old.mins[1]),
				static_cast<float>(old.mins[2]),
			},
			{
				static_cast<float>(old.maxs[0]),
				static_cast<float>(old.maxs[1]),
				static_cast<float>(old.maxs[2]),
			},
			old.firstFace,
			old.numFaces,
			old.area,
		};
	}
};

using VBSPNode = VBSPNode_v1;

//endregion

//region Lump 6 (Texture Info)

struct VBSPTextureInfo_v0 {
	sourcepp::math::Vec4f textureVector1;
	sourcepp::math::Vec4f textureVector2;
	sourcepp::math::Vec4f lightmapVector1;
	sourcepp::math::Vec4f lightmapVector2;
	int32_t flags;
	int32_t textureData;
};

using VBSPTextureInfo = VBSPTextureInfo_v0;

//endregion

//region Lump 7/27 (Faces/Original Faces)

struct VBSPFace_v1 {
	uint16_t planeNum;
	uint8_t side;
	uint8_t onNode;
	int32_t firstEdge;
	int16_t numEdges;
	int16_t texInfo;
	int16_t dispInfo;
	int16_t surfaceFogVolumeID;
	sourcepp::math::Vec4ui8 styles;
	int32_t lightOffset;
	float area;
	sourcepp::math::Vec2i lightmapTextureMinsInLuxels;
	sourcepp::math::Vec2i lightmapTextureSizeInLuxels;
	int32_t originalFace;
	uint16_t numPrims;
	uint16_t firstPrimID;
	uint32_t smoothingGroups;
};

struct VBSPFace_v2 {
	uint32_t planeNum;
	uint8_t side;
	uint8_t onNode;
	int32_t firstEdge;
	int32_t numEdges;
	int32_t texInfo;
	int32_t dispInfo;
	int32_t surfaceFogVolumeID;
	sourcepp::math::Vec4ui8 styles;
	int32_t lightOffset;
	float area;
	sourcepp::math::Vec2i lightmapTextureMinsInLuxels;
	sourcepp::math::Vec2i lightmapTextureSizeInLuxels;
	int32_t originalFace;
	uint32_t enableShadows : 1;
	uint32_t numPrims : 31;
	uint32_t firstPrimID;
	uint32_t smoothingGroups;

	[[nodiscard]] static VBSPFace_v2 upgrade(const VBSPFace_v1& old) {
		return {
			old.planeNum,
			old.side,
			old.onNode,
			old.firstEdge,
			old.numEdges,
			old.texInfo,
			old.dispInfo,
			old.surfaceFogVolumeID,
			{
				old.styles[0],
				old.styles[1],
				old.styles[2],
				old.styles[3],
			},
			old.lightOffset,
			old.area,
			{
				old.lightmapTextureMinsInLuxels[0],
				old.lightmapTextureMinsInLuxels[1],
			},
			{
				old.lightmapTextureSizeInLuxels[0],
				old.lightmapTextureSizeInLuxels[1],
			},
			old.originalFace,
			1,
			old.numPrims,
			old.firstPrimID,
			old.smoothingGroups,
		};
	}
};

using VBSPFace = VBSPFace_v2;

//endregion

//region Lump 12 (Edges)

struct VBSPEdge_v0 {
	uint16_t v0;
	uint16_t v1;
};

struct VBSPEdge_v1 {
	uint32_t v0;
	uint32_t v1;

	[[nodiscard]] static VBSPEdge_v1 upgrade(const VBSPEdge_v0& old) {
		return {
			old.v0,
			old.v1,
		};
	}
};

using VBSPEdge = VBSPEdge_v1;

//endregion

//region Lump 13 (SurfEdges)

struct VBSPSurfEdge_v0 {
	int32_t surfEdge;
};

using VBSPSurfEdge = VBSPSurfEdge_v0;

//endregion

//region Lump 14 (Brush Models)

struct VBSPBrushModel_v0 {
	sourcepp::math::Vec3f min;
	sourcepp::math::Vec3f max;
	sourcepp::math::Vec3f origin;
	int32_t headNode;
	int32_t firstFace;
	int32_t numFaces;
};

using VBSPBrushModel = VBSPBrushModel_v0;

//endregion

//region Lump 35 (Game Lump)

struct VBSPGameLump {
	enum Signature : uint32_t {
		SIGNATURE_STATIC_PROPS = sourcepp::parser::binary::makeFourCC("sprp"),
		SIGNATURE_DETAIL_PROPS = sourcepp::parser::binary::makeFourCC("dprp"),
		SIGNATURE_DETAIL_PROP_LIGHTING_LDR = sourcepp::parser::binary::makeFourCC("dplt"),
		SIGNATURE_DETAIL_PROP_LIGHTING_HDR = sourcepp::parser::binary::makeFourCC("dplh"),
	} signature;
	uint16_t isCompressed;
	uint16_t version;
	// Offset from beginning of file, not game lump (except Portal 2 on console but we'll ignore that for now)
	uint32_t offset;
	// Compressed size is determined by subtracting the next entry's offset with this one like VTF resources
	uint32_t uncompressedLength;

	std::vector<std::byte> data;
};

//endregion

class VBSP {
	struct Lump {
		/// Byte offset into file
		uint32_t offset;
		/// Length of lump data
		uint32_t length;
		/// Lump format version
		uint32_t version;
		/// Uncompressed length if lump is compressed, else 0
		uint32_t uncompressedLength;
	};

	struct Header {
		/// Version of the BSP file
		uint32_t version;
		/// Lump metadata
		std::array<Lump, VBSP_LUMP_COUNT> lumps;
		/// Map version number
		uint32_t mapRevision;
	};

public:
	explicit VBSP(std::string path_, bool loadPatchFiles = true);

	explicit operator bool() const;

	static VBSP create(std::string path, uint32_t version = 21, uint32_t mapRevision = 0);

	[[nodiscard]] uint32_t getVersion() const;

	void setVersion(uint32_t version);

	[[nodiscard]] uint32_t getMapRevision() const;

	void setMapRevision(uint32_t mapRevision);

	[[nodiscard]] bool isL4D2() const;

	void setL4D2(bool isL4D2);

	[[nodiscard]] bool isConsole() const;

	void setConsole(bool isConsole);

	[[nodiscard]] bool hasLump(VBSPLump lumpIndex) const;

	[[nodiscard]] bool isLumpCompressed(VBSPLump lumpIndex) const;

	[[nodiscard]] uint32_t getLumpVersion(VBSPLump lumpIndex) const;

	[[nodiscard]] std::optional<std::vector<std::byte>> getLumpData(VBSPLump lumpIndex, bool noDecompression = false) const;

	template<VBSPLump Lump>
	[[nodiscard]] auto getLumpData() const {
		if constexpr (Lump == VBSPLump::ENTITIES) {
			return this->parseEntities();
		} else if constexpr (Lump == VBSPLump::PLANES) {
			return this->parsePlanes();
		} else if constexpr (Lump == VBSPLump::TEXDATA) {
			return this->parseTextureData();
		} else if constexpr (Lump == VBSPLump::VERTEXES) {
			return this->parseVertices();
		} else if constexpr (Lump == VBSPLump::NODES) {
			return this->parseNodes();
		} else if constexpr (Lump == VBSPLump::TEXINFO) {
			return this->parseTextureInfo();
		} else if constexpr (Lump == VBSPLump::FACES) {
			return this->parseFaces();
		} else if constexpr (Lump == VBSPLump::EDGES) {
			return this->parseEdges();
		} else if constexpr (Lump == VBSPLump::SURFEDGES) {
			return this->parseSurfEdges();
		} else if constexpr (Lump == VBSPLump::MODELS) {
			return this->parseBrushModels();
		} else if constexpr (Lump == VBSPLump::ORIGINALFACES) {
			return this->parseOriginalFaces();
		} else if constexpr (Lump == VBSPLump::GAME_LUMP) {
			return this->parseGameLumps(true);
		} else {
			return this->getLumpData(Lump);
		}
	}

	/// BSP::setGameLump should be used for writing game lumps as they need special handling. Paklump can be
	/// written here but compression is unsupported, prefer using bsppp::PakLump or your favorite zip library
	/// instead. Valid compressLevel range is 0 to 9, 0 is considered off, 9 the slowest and most compressiest
	bool setLump(VBSPLump lumpIndex, uint32_t version, std::span<const std::byte> data, uint8_t compressLevel = 0);

	bool setLump(uint32_t version, std::span<const BSPEntityKeyValues> data, uint8_t compressLevel = 0);

	[[nodiscard]] bool isGameLumpCompressed(VBSPGameLump::Signature signature) const;

	[[nodiscard]] uint16_t getGameLumpVersion(VBSPGameLump::Signature signature) const;

	[[nodiscard]] std::optional<std::vector<std::byte>> getGameLumpData(VBSPGameLump::Signature signature) const;

	bool setGameLump(VBSPGameLump::Signature signature, uint16_t version, std::span<const std::byte> data, uint8_t compressLevel = 0);

	/// Reset changes made to a lump before they're written to disk
	void resetLump(VBSPLump lumpIndex);

	/// Resets ALL in-memory modifications (version, all lumps including game lumps, map revision)
	void reset();

	void createLumpPatchFile(VBSPLump lumpIndex) const;

	bool setLumpFromPatchFile(const std::string& lumpFilePath);

	bool bake(std::string_view outputPath = "");

protected:
	bool readHeader();

	[[nodiscard]] std::vector<BSPEntityKeyValues> parseEntities() const;

	[[nodiscard]] std::vector<VBSPPlane> parsePlanes() const;

	[[nodiscard]] std::vector<VBSPTextureData> parseTextureData() const;

	[[nodiscard]] std::vector<VBSPVertex> parseVertices() const;

	[[nodiscard]] std::vector<VBSPNode> parseNodes() const;

	[[nodiscard]] std::vector<VBSPTextureInfo> parseTextureInfo() const;

	[[nodiscard]] std::vector<VBSPFace> parseFaces() const;

	[[nodiscard]] std::vector<VBSPEdge> parseEdges() const;

	[[nodiscard]] std::vector<VBSPSurfEdge> parseSurfEdges() const;

	[[nodiscard]] std::vector<VBSPBrushModel> parseBrushModels() const;

	[[nodiscard]] std::vector<VBSPFace> parseOriginalFaces() const;

	[[nodiscard]] std::vector<VBSPGameLump> parseGameLumps(bool decompress) const;

	std::string path;
	Header header{};

	uint32_t stagedVersion{};
	std::unordered_map<uint32_t, std::pair<Lump, std::vector<std::byte>>> stagedLumps;
	std::vector<VBSPGameLump> stagedGameLumps;
	uint32_t stagedMapRevision{};

	// Slightly different header despite using the same version just to be quirky
	bool l4d2 = false;
	// BSP is mostly big-endian
	bool console = false;
};

} // namespace bsppp
