#pragma once

#include <array>

#include "ZIP.h"

namespace vpkpp {

constexpr int32_t BSP_SIGNATURE = 'V' + ('B' << 8) + ('S' << 16) + ('P' << 24);
constexpr std::string_view BSP_EXTENSION = ".bsp";

constexpr std::string_view BSP_ENTITY_LUMP_NAME = "entities.kv";
constexpr std::string_view BSP_LUMP_NAME_FORMAT = "lmp_%d.bin";

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

class BSP : public ZIP {
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
		/// BSP_ID
		int32_t signature;
		/// Version of the BSP file
		int32_t version;
		/// Lump metadata
		std::array<Lump, static_cast<uint32_t>(BSPLump::COUNT)> lumps;
		/// Map version number
		int32_t mapRevision;
	};

public:
	/// Open a BSP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const Callback& callback = nullptr);

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return false;
	}

	bool bake(const std::string& outputDir_ /*= ""*/, const Callback& callback /*= nullptr*/) override;

	[[nodiscard]] explicit operator std::string() const override;

	[[nodiscard]] std::optional<std::vector<std::byte>> readLump(BSPLump lumpIndex) const;

	void writeLump(BSPLump lumpIndex, const std::vector<std::byte>& data);

protected:
	BSP(const std::string& fullFilePath_, PackFileOptions options_);

	/// If the lump is too big where it is, shift it to the end of the file, otherwise its fine
	void moveLumpToWritableSpace(BSPLump lumpIndex, int newSize);

	static const std::string TEMP_ZIP_PATH;

	Header header{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BSP_EXTENSION, &BSP::open);
};

} // namespace vpkpp
