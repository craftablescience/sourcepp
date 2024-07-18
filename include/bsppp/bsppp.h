#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace bsppp {

constexpr auto BSP_SIGNATURE = sourcepp::parser::binary::makeFourCC("VBSP");

enum class BSPLump : int32_t {
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
static_assert(static_cast<int32_t>(BSPLump::COUNT) == 64, "Incorrect lump count!");

constexpr auto BSP_LUMP_COUNT = static_cast<int32_t>(BSPLump::COUNT);

class BSP {
	struct Lump {
		/// Byte offset into file
		int32_t offset;
		/// Length of lump data
		int32_t length;
		/// Lump format version
		int32_t version;
		/// Uncompressed size, or 0
		int32_t fourCC;
	};

	struct Header {
		/// Version of the BSP file
		int32_t version;
		/// Lump metadata
		std::array<Lump, BSP_LUMP_COUNT> lumps;
		/// Map version number
		int32_t mapRevision;
	};

public:
	explicit BSP(std::string path_);

	explicit operator bool() const;

	static BSP create(std::string path, int32_t version = 21, int32_t mapRevision = 0);

	[[nodiscard]] int32_t getVersion() const;

	void setVersion(int32_t version);

	[[nodiscard]] int32_t getMapRevision() const;

	void setMapRevision(int32_t mapRevision);

	[[nodiscard]] bool hasLump(BSPLump lumpIndex) const;

	[[nodiscard]] int32_t getLumpVersion(BSPLump lumpIndex) const;

	void setLumpVersion(BSPLump lumpIndex, int32_t version);

	[[nodiscard]] std::optional<std::vector<std::byte>> readLump(BSPLump lumpIndex) const;

	void writeLump(BSPLump lumpIndex, const std::vector<std::byte>& data);

	bool applyLumpPatchFile(const std::string& lumpFilePath);

	void createLumpPatchFile(BSPLump lumpIndex) const;

protected:
	void writeHeader() const;

	/// If the lump is too big where it is, shift it to the end of the file, otherwise its fine
	void moveLumpToWritableSpace(BSPLump lumpIndex, int32_t newSize);

	std::string path;
	Header header{};

	// Slightly different header just to be quirky and special
	bool isL4D2 = false;
};

} // namespace bsppp
