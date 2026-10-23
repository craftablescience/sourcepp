// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>

namespace py = nanobind;
using namespace py::literals;

#include <bsppp/bsppp.h>

namespace bsppp {

inline void register_python(py::module_& m) {
	auto bsppp = m.def_submodule("bsppp");
	using namespace bsppp;

	auto cVBSP = py::class_<VBSP>(bsppp, "VBSP");

	auto cBSPEntityKeyValues = py::class_<BSPEntityKeyValues>(cVBSP, "EntityKeyValues");

	py::class_<BSPEntityKeyValues::Element>(cBSPEntityKeyValues, "Element")
		.def_prop_rw("key", &BSPEntityKeyValues::Element::getKey, &BSPEntityKeyValues::Element::setKey)
		.def_prop_rw("value", &BSPEntityKeyValues::Element::getValue<std::string_view>, &BSPEntityKeyValues::Element::setValue<std::string_view>)
		.def_prop_ro("invalid", &BSPEntityKeyValues::Element::isInvalid);

	cBSPEntityKeyValues
		.def(py::init())
		.def("has_child", &BSPEntityKeyValues::hasChild, "child_key"_a)
		.def_prop_ro("keyvalues_count", py::overload_cast<>(&BSPEntityKeyValues::getKeyValuesCount, py::const_))
		.def("get_keyvalues_count_for_key", py::overload_cast<std::string_view>(&BSPEntityKeyValues::getKeyValuesCount, py::const_), "child_key"_a)
		.def_prop_ro("keyvalues", &BSPEntityKeyValues::getKeyValues, py::rv_policy::reference_internal)
		.def("at", py::overload_cast<unsigned int>(&BSPEntityKeyValues::operator[]), "n"_a, py::rv_policy::reference_internal)
		.def("get", py::overload_cast<std::string_view>(&BSPEntityKeyValues::operator[]), "child_key"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&BSPEntityKeyValues::operator[]), "child_key"_a, py::rv_policy::reference_internal, py::is_operator())
		.def("get_n", py::overload_cast<std::string_view, unsigned int>(&BSPEntityKeyValues::operator()), "child_key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("add_keyvalue", &BSPEntityKeyValues::addKeyValue<std::string_view>, "child_key"_a, "value"_a)
		.def("remove_keyvalue", &BSPEntityKeyValues::removeKeyValue, "child_key"_a, "n"_a = -1)
		.def("bake", &BSPEntityKeyValues::bake, "use_escapes"_a);

	py::class_<VBSPPlane_v0>(cVBSP, "Plane_v0")
		.def(py::init())
		.def_rw("normal", &VBSPPlane_v0::normal)
		.def_rw("dist", &VBSPPlane_v0::dist)
		.def_rw("type", &VBSPPlane_v0::type);

	py::class_<VBSPTextureData_v0>(cVBSP, "TextureData_v0")
		.def(py::init())
		.def_rw("reflectivity", &VBSPTextureData_v0::reflectivity)
		.def_rw("name_string_table_id", &VBSPTextureData_v0::nameStringTableID)
		.def_rw("width", &VBSPTextureData_v0::width)
		.def_rw("height", &VBSPTextureData_v0::height)
		.def_rw("view_width", &VBSPTextureData_v0::width)
		.def_rw("view_height", &VBSPTextureData_v0::height);

	py::class_<VBSPVertex_v0>(cVBSP, "Vertex_v0")
		.def(py::init())
		.def_rw("position", &VBSPVertex_v0::position);

	py::class_<VBSPNode_v0>(cVBSP, "Node_v0")
		.def(py::init())
		.def_rw("plane_num", &VBSPNode_v0::planeNum)
		.def_rw("children", &VBSPNode_v0::children)
		.def_rw("mins", &VBSPNode_v0::mins)
		.def_rw("maxs", &VBSPNode_v0::maxs)
		.def_rw("first_face", &VBSPNode_v0::firstFace)
		.def_rw("num_faces", &VBSPNode_v0::numFaces)
		.def_rw("area", &VBSPNode_v0::area);

	py::class_<VBSPNode_v1>(cVBSP, "Node_v1")
		.def(py::init())
		.def_rw("plane_num", &VBSPNode_v1::planeNum)
		.def_rw("children", &VBSPNode_v1::children)
		.def_rw("mins", &VBSPNode_v1::mins)
		.def_rw("maxs", &VBSPNode_v1::maxs)
		.def_rw("first_face", &VBSPNode_v1::firstFace)
		.def_rw("num_faces", &VBSPNode_v1::numFaces)
		.def_rw("area", &VBSPNode_v1::area)
		.def_static("upgrade", &VBSPNode_v1::upgrade, "old"_a);

	py::class_<VBSPTextureInfo_v0>(cVBSP, "TextureInfo_v0")
		.def(py::init())
		.def_rw("texture_vector1", &VBSPTextureInfo_v0::textureVector1)
		.def_rw("texture_vector2", &VBSPTextureInfo_v0::textureVector2)
		.def_rw("lightmap_vector1", &VBSPTextureInfo_v0::lightmapVector1)
		.def_rw("lightmap_vector2", &VBSPTextureInfo_v0::lightmapVector2)
		.def_rw("flags", &VBSPTextureInfo_v0::flags)
		.def_rw("texture_data", &VBSPTextureInfo_v0::textureData);

	py::class_<VBSPFace_v1>(cVBSP, "Face_v1")
		.def(py::init())
		.def_rw("plane_num", &VBSPFace_v1::planeNum)
		.def_rw("side", &VBSPFace_v1::side)
		.def_rw("on_node", &VBSPFace_v1::onNode)
		.def_rw("first_edge", &VBSPFace_v1::firstEdge)
		.def_rw("num_edges", &VBSPFace_v1::numEdges)
		.def_rw("tex_info", &VBSPFace_v1::texInfo)
		.def_rw("disp_info", &VBSPFace_v1::dispInfo)
		.def_rw("surface_fog_volume_id", &VBSPFace_v1::surfaceFogVolumeID)
		.def_rw("styles", &VBSPFace_v1::styles)
		.def_rw("light_offset", &VBSPFace_v1::lightOffset)
		.def_rw("area", &VBSPFace_v1::area)
		.def_rw("lightmap_texture_mins_in_luxels", &VBSPFace_v1::lightmapTextureMinsInLuxels)
		.def_rw("lightmap_texture_size_in_luxels", &VBSPFace_v1::lightmapTextureSizeInLuxels)
		.def_rw("original_face", &VBSPFace_v1::originalFace)
		.def_rw("num_prims", &VBSPFace_v1::numPrims)
		.def_rw("first_prim_id", &VBSPFace_v1::firstPrimID)
		.def_rw("smoothing_groups", &VBSPFace_v1::smoothingGroups);

	py::class_<VBSPFace_v2>(cVBSP, "Face_v2")
		.def(py::init())
		.def_rw("plane_num", &VBSPFace_v2::planeNum)
		.def_rw("side", &VBSPFace_v2::side)
		.def_rw("on_node", &VBSPFace_v2::onNode)
		.def_rw("first_edge", &VBSPFace_v2::firstEdge)
		.def_rw("num_edges", &VBSPFace_v2::numEdges)
		.def_rw("tex_info", &VBSPFace_v2::texInfo)
		.def_rw("disp_info", &VBSPFace_v2::dispInfo)
		.def_rw("surface_fog_volume_id", &VBSPFace_v2::surfaceFogVolumeID)
		.def_rw("styles", &VBSPFace_v2::styles)
		.def_rw("light_offset", &VBSPFace_v2::lightOffset)
		.def_rw("area", &VBSPFace_v2::area)
		.def_rw("lightmap_texture_mins_in_luxels", &VBSPFace_v2::lightmapTextureMinsInLuxels)
		.def_rw("lightmap_texture_size_in_luxels", &VBSPFace_v2::lightmapTextureSizeInLuxels)
		.def_rw("original_face", &VBSPFace_v2::originalFace)
		.def_prop_rw("enable_shadows", [](const VBSPFace_v2& self) -> bool { return self.enableShadows; }, [](VBSPFace_v2& self, bool enableShadows) { self.enableShadows = enableShadows; })
		.def_prop_rw("num_prims", [](const VBSPFace_v2& self) { return self.numPrims; }, [](VBSPFace_v2& self, bool numPrims) { self.numPrims = numPrims; })
		.def_rw("first_prim_id", &VBSPFace_v2::firstPrimID)
		.def_rw("smoothing_groups", &VBSPFace_v2::smoothingGroups)
		.def_static("upgrade", &VBSPFace_v2::upgrade);

	py::class_<VBSPEdge_v0>(cVBSP, "Edge_v0")
		.def(py::init())
		.def_rw("v0", &VBSPEdge_v0::v0)
		.def_rw("v1", &VBSPEdge_v0::v1);

	py::class_<VBSPEdge_v1>(cVBSP, "Edge_v1")
		.def(py::init())
		.def_rw("v0", &VBSPEdge_v1::v0)
		.def_rw("v1", &VBSPEdge_v1::v1)
		.def_static("upgrade", &VBSPEdge_v1::upgrade);

	py::class_<VBSPSurfEdge_v0>(cVBSP, "SurfEdge_v0")
		.def(py::init())
		.def_rw("surf_edge", &VBSPSurfEdge_v0::surfEdge);

	py::class_<VBSPBrushModel_v0>(cVBSP, "BrushModel_v0")
		.def(py::init())
		.def_rw("min", &VBSPBrushModel_v0::min)
		.def_rw("max", &VBSPBrushModel_v0::max)
		.def_rw("origin", &VBSPBrushModel_v0::origin)
		.def_rw("head_node", &VBSPBrushModel_v0::headNode)
		.def_rw("first_face", &VBSPBrushModel_v0::firstFace)
		.def_rw("num_faces", &VBSPBrushModel_v0::numFaces);

	auto cVBSPGameLump = py::class_<VBSPGameLump>(cVBSP, "GameLump");

	py::enum_<VBSPGameLump::Signature>(cVBSPGameLump, "Signature")
		.value("STATIC_PROPS",             VBSPGameLump::SIGNATURE_STATIC_PROPS)
		.value("DETAIL_PROPS",             VBSPGameLump::SIGNATURE_DETAIL_PROPS)
		.value("DETAIL_PROP_LIGHTING_LDR", VBSPGameLump::SIGNATURE_DETAIL_PROP_LIGHTING_LDR)
		.value("DETAIL_PROP_LIGHTING_HDR", VBSPGameLump::SIGNATURE_DETAIL_PROP_LIGHTING_HDR);

	cVBSPGameLump
		.def_rw("signature", &VBSPGameLump::signature)
		.def_rw("is_compressed", &VBSPGameLump::isCompressed)
		.def_rw("version", &VBSPGameLump::version)
		.def_rw("offset", &VBSPGameLump::offset)
		.def_rw("uncompressed_length", &VBSPGameLump::uncompressedLength)
		.def_prop_rw("data", [](const VBSPGameLump& self) {
			return py::bytes{self.data.data(), self.data.size()};
		}, [](VBSPGameLump& self, const py::bytes& data) {
			self.data = {static_cast<const std::byte*>(data.data()), static_cast<const std::byte*>(data.data()) + data.size()};
		});

	bsppp.attr("VBSP_SIGNATURE") = VBSP_SIGNATURE;
	bsppp.attr("VBSP_LUMP_COUNT") = VBSP_LUMP_COUNT;
	bsppp.attr("VBSP_LUMP_ORDER") = VBSP_LUMP_ORDER;

	py::enum_<VBSPLump>(cVBSP, "Lump")
		.value("UNKNOWN",                        VBSPLump::UNKNOWN)
		.value("ENTITIES",                       VBSPLump::ENTITIES)
		.value("PLANES",                         VBSPLump::PLANES)
		.value("TEXDATA",                        VBSPLump::TEXDATA)
		.value("VERTEXES",                       VBSPLump::VERTEXES)
		.value("VISIBILITY",                     VBSPLump::VISIBILITY)
		.value("NODES",                          VBSPLump::NODES)
		.value("TEXINFO",                        VBSPLump::TEXINFO)
		.value("FACES",                          VBSPLump::FACES)
		.value("LIGHTING",                       VBSPLump::LIGHTING)
		.value("OCCLUSION",                      VBSPLump::OCCLUSION)
		.value("LEAFS",                          VBSPLump::LEAFS)
		.value("FACEIDS",                        VBSPLump::FACEIDS)
		.value("EDGES",                          VBSPLump::EDGES)
		.value("SURFEDGES",                      VBSPLump::SURFEDGES)
		.value("MODELS",                         VBSPLump::MODELS)
		.value("WORLDLIGHTS",                    VBSPLump::WORLDLIGHTS)
		.value("LEAFFACES",                      VBSPLump::LEAFFACES)
		.value("LEAFBRUSHES",                    VBSPLump::LEAFBRUSHES)
		.value("BRUSHES",                        VBSPLump::BRUSHES)
		.value("BRUSHSIDES",                     VBSPLump::BRUSHSIDES)
		.value("AREAS",                          VBSPLump::AREAS)
		.value("AREAPORTALS",                    VBSPLump::AREAPORTALS)
		.value("S2004_PORTALS",                  VBSPLump::S2004_PORTALS)
		.value("UNUSED0",                        VBSPLump::UNUSED0)
		.value("SL4D2_PROPCOLLISION",            VBSPLump::SL4D2_PROPCOLLISION)
		.value("S2004_CLUSTERS",                 VBSPLump::S2004_CLUSTERS)
		.value("UNUSED1",                        VBSPLump::UNUSED1)
		.value("SL4D2_PROPHULLS",                VBSPLump::SL4D2_PROPHULLS)
		.value("S2004_PORTALVERTS",              VBSPLump::S2004_PORTALVERTS)
		.value("UNUSED2",                        VBSPLump::UNUSED2)
		.value("SL4D2_PROPHULLVERTS",            VBSPLump::SL4D2_PROPHULLVERTS)
		.value("S2004_CLUSTERPORTALS",           VBSPLump::S2004_CLUSTERPORTALS)
		.value("UNUSED3",                        VBSPLump::UNUSED3)
		.value("SL4D2_PROPTRIS",                 VBSPLump::SL4D2_PROPTRIS)
		.value("DISPINFO",                       VBSPLump::DISPINFO)
		.value("ORIGINALFACES",                  VBSPLump::ORIGINALFACES)
		.value("PHYSDISP",                       VBSPLump::PHYSDISP)
		.value("PHYSCOLLIDE",                    VBSPLump::PHYSCOLLIDE)
		.value("VERTNORMALS",                    VBSPLump::VERTNORMALS)
		.value("VERTNORMALINDICES",              VBSPLump::VERTNORMALINDICES)
		.value("S2004_DISP_LIGHTMAP_ALPHAS",     VBSPLump::S2004_DISP_LIGHTMAP_ALPHAS)
		.value("UNUSED4",                        VBSPLump::UNUSED4)
		.value("DISP_VERTS",                     VBSPLump::DISP_VERTS)
		.value("DISP_LIGHTMAP_SAMPLE_POSITIONS", VBSPLump::DISP_LIGHTMAP_SAMPLE_POSITIONS)
		.value("GAME_LUMP",                      VBSPLump::GAME_LUMP)
		.value("LEAFWATERDATA",                  VBSPLump::LEAFWATERDATA)
		.value("PRIMITIVES",                     VBSPLump::PRIMITIVES)
		.value("PRIMVERTS",                      VBSPLump::PRIMVERTS)
		.value("PRIMINDICES",                    VBSPLump::PRIMINDICES)
		.value("PAKFILE",                        VBSPLump::PAKFILE)
		.value("CLIPPORTALVERTS",                VBSPLump::CLIPPORTALVERTS)
		.value("CUBEMAPS",                       VBSPLump::CUBEMAPS)
		.value("TEXDATA_STRING_DATA",            VBSPLump::TEXDATA_STRING_DATA)
		.value("TEXDATA_STRING_TABLE",           VBSPLump::TEXDATA_STRING_TABLE)
		.value("OVERLAYS",                       VBSPLump::OVERLAYS)
		.value("LEAFMINDISTTOWATER",             VBSPLump::LEAFMINDISTTOWATER)
		.value("FACE_MACRO_TEXTURE_INFO",        VBSPLump::FACE_MACRO_TEXTURE_INFO)
		.value("DISP_TRIS",                      VBSPLump::DISP_TRIS)
		.value("S2004_PHYSCOLLIDESURFACE",       VBSPLump::S2004_PHYSCOLLIDESURFACE)
		.value("UNUSED5",                        VBSPLump::UNUSED5)
		.value("SL4D2_PROP_BLOB",                VBSPLump::SL4D2_PROP_BLOB)
		.value("WATEROVERLAYS",                  VBSPLump::WATEROVERLAYS)
		.value("S2006_XBOX_LIGHTMAPPAGES",       VBSPLump::S2006_XBOX_LIGHTMAPPAGES)
		.value("LEAF_AMBIENT_INDEX_HDR",         VBSPLump::LEAF_AMBIENT_INDEX_HDR)
		.value("S2006_XBOX_LIGHTMAPPAGEINFOS",   VBSPLump::S2006_XBOX_LIGHTMAPPAGEINFOS)
		.value("LEAF_AMBIENT_INDEX",             VBSPLump::LEAF_AMBIENT_INDEX)
		.value("LIGHTING_HDR",                   VBSPLump::LIGHTING_HDR)
		.value("WORLDLIGHTS_HDR",                VBSPLump::WORLDLIGHTS_HDR)
		.value("LEAF_AMBIENT_LIGHTING_HDR",      VBSPLump::LEAF_AMBIENT_LIGHTING_HDR)
		.value("LEAF_AMBIENT_LIGHTING",          VBSPLump::LEAF_AMBIENT_LIGHTING)
		.value("XBOX_XZIPPAKFILE",               VBSPLump::XBOX_XZIPPAKFILE)
		.value("FACES_HDR",                      VBSPLump::FACES_HDR)
		.value("MAP_FLAGS",                      VBSPLump::MAP_FLAGS)
		.value("OVERLAY_FADES",                  VBSPLump::OVERLAY_FADES)
		.value("L4D_OVERLAY_SYSTEM_LEVELS",      VBSPLump::L4D_OVERLAY_SYSTEM_LEVELS)
		.value("UNUSED6",                        VBSPLump::UNUSED6)
		.value("L4D2_PHYSLEVEL",                 VBSPLump::L4D2_PHYSLEVEL)
		.value("UNUSED7",                        VBSPLump::UNUSED7)
		.value("ASW_DISP_MULTIBLEND",            VBSPLump::ASW_DISP_MULTIBLEND)
		.value("UNUSED8",                        VBSPLump::UNUSED8);

	cVBSP
		.def(py::init<std::string, bool>(), "path"_a, "load_patch_files"_a = true)
		.def("__bool__", &VBSP::operator bool, py::is_operator())
		.def_static("create", &VBSP::create, "path"_a, "version"_a = 21, "map_revision"_a = 0)
		.def_prop_rw("version", &VBSP::getVersion, &VBSP::setVersion)
		.def_prop_rw("map_revision", &VBSP::getMapRevision, &VBSP::setMapRevision)
		.def_prop_rw("l4d2", &VBSP::isL4D2, &VBSP::setL4D2)
		.def_prop_rw("console", &VBSP::isConsole, &VBSP::setConsole)
		.def("has_lump", &VBSP::hasLump, "lump_index"_a)
		.def("is_lump_compressed", &VBSP::isLumpCompressed, "lump_index"_a)
		.def("get_lump_version", &VBSP::getLumpVersion, "lump_index"_a)
		.def("get_lump_data", [](const VBSP& self, VBSPLump lumpIndex, bool noDecompression = false) -> py::object {
			const auto d = self.getLumpData(lumpIndex, noDecompression);
			if (d) return py::bytes{d->data(), d->size()};
			return py::none{};
		}, "lump_index"_a, "no_decompression"_a = false)
		.def("get_lump_data_for_entities", &VBSP::getLumpData<VBSPLump::ENTITIES>)
		.def("get_lump_data_for_planes", &VBSP::getLumpData<VBSPLump::PLANES>)
		.def("get_lump_data_for_texdata", &VBSP::getLumpData<VBSPLump::TEXDATA>)
		.def("get_lump_data_for_vertexes", &VBSP::getLumpData<VBSPLump::VERTEXES>)
		.def("get_lump_data_for_nodes", &VBSP::getLumpData<VBSPLump::NODES>)
		.def("get_lump_data_for_texinfo", &VBSP::getLumpData<VBSPLump::TEXINFO>)
		.def("get_lump_data_for_faces", &VBSP::getLumpData<VBSPLump::FACES>)
		.def("get_lump_data_for_edges", &VBSP::getLumpData<VBSPLump::EDGES>)
		.def("get_lump_data_for_surfedges", &VBSP::getLumpData<VBSPLump::SURFEDGES>)
		.def("get_lump_data_for_models", &VBSP::getLumpData<VBSPLump::MODELS>)
		.def("get_lump_data_for_originalfaces", &VBSP::getLumpData<VBSPLump::ORIGINALFACES>)
		.def("get_lump_data_for_game_lump", &VBSP::getLumpData<VBSPLump::GAME_LUMP>)
		.def("set_lump", [](VBSP& self, VBSPLump lumpIndex, uint32_t version, const py::bytes& data, uint8_t compressLevel = 0) {
			self.setLump(lumpIndex, version, {static_cast<const std::byte*>(data.data()), data.size()}, compressLevel);
		}, "lump_index"_a, "version"_a, "data"_a, "compress_level"_a = 0)
		.def("set_lump_for_entities", [](VBSP& self, uint32_t version, const std::vector<BSPEntityKeyValues>& data, uint8_t compressLevel = 0) {
			self.setLump(version, data, compressLevel);
		}, "version"_a, "data"_a, "compress_level"_a)
		.def("is_game_lump_compressed", &VBSP::isGameLumpCompressed, "signature"_a)
		.def("get_game_lump_data", [](const VBSP& self, VBSPGameLump::Signature signature) -> py::object {
			const auto d = self.getGameLumpData(signature);
			if (d) return py::bytes{d->data(), d->size()};
			return py::none{};
		}, "signature"_a)
		.def("set_game_lump", &VBSP::setGameLump, "signature"_a, "version"_a, "data"_a, "compress_level"_a = 0)
		.def("reset_lump", &VBSP::resetLump, "lump_index"_a)
		.def("reset", &VBSP::reset)
		.def("create_lump_patch_file", &VBSP::createLumpPatchFile, "lump_index"_a)
		.def("set_lump_from_patch_file", &VBSP::setLumpFromPatchFile, "lump_file_path"_a)
		.def("bake", &VBSP::bake, "output_path"_a = "");

	bsppp.attr("VBSP_EXTENSION") = VBSP_EXTENSION;

	py::class_<PakLump, vpkpp::PackFile>(bsppp, "PakLump")
		.def_static("open", &PakLump::open, "path"_a, "callback"_a = nullptr);
}

} // namespace bsppp
