#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/vector.h>

namespace py = nanobind;
using namespace py::literals;

#include <bsppp/bsppp.h>

namespace bsppp {

inline void register_python(py::module_& m) {
	auto bsppp = m.def_submodule("bsppp");
	using namespace bsppp;

	auto cBSP = py::class_<BSP>(bsppp, "BSP");

	auto cBSPEntityKeyValues = py::class_<BSPEntityKeyValues>(cBSP, "EntityKeyValues");

	py::class_<BSPEntityKeyValues::Element>(cBSPEntityKeyValues, "Element")
		.def_prop_rw("key", &BSPEntityKeyValues::Element::getKey, &BSPEntityKeyValues::Element::setKey)
		.def_prop_rw("value", &BSPEntityKeyValues::Element::getValue<std::string_view>, &BSPEntityKeyValues::Element::setValue<std::string_view>)
		.def_prop_ro("invalid", &BSPEntityKeyValues::Element::isInvalid);

	cBSPEntityKeyValues
		.def(py::init<>())
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

	py::class_<BSPPlane_v0>(cBSP, "Plane_v0")
		.def(py::init<>())
		.def_rw("normal", &BSPPlane_v0::normal)
		.def_rw("dist", &BSPPlane_v0::dist)
		.def_rw("type", &BSPPlane_v0::type);

	py::class_<BSPTextureData_v0>(cBSP, "TextureData_v0")
		.def(py::init<>())
		.def_rw("reflectivity", &BSPTextureData_v0::reflectivity)
		.def_rw("name_string_table_id", &BSPTextureData_v0::nameStringTableID)
		.def_rw("width", &BSPTextureData_v0::width)
		.def_rw("height", &BSPTextureData_v0::height)
		.def_rw("view_width", &BSPTextureData_v0::width)
		.def_rw("view_height", &BSPTextureData_v0::height);

	py::class_<BSPVertex_v0>(cBSP, "Vertex_v0")
		.def(py::init<>())
		.def_rw("position", &BSPVertex_v0::position);

	py::class_<BSPNode_v0>(cBSP, "Node_v0")
		.def(py::init<>())
		.def_rw("plane_num", &BSPNode_v0::planeNum)
		.def_rw("children", &BSPNode_v0::children)
		.def_rw("mins", &BSPNode_v0::mins)
		.def_rw("maxs", &BSPNode_v0::maxs)
		.def_rw("first_face", &BSPNode_v0::firstFace)
		.def_rw("num_faces", &BSPNode_v0::numFaces)
		.def_rw("area", &BSPNode_v0::area);

	py::class_<BSPNode_v1>(cBSP, "Node_v1")
		.def(py::init<>())
		.def_rw("plane_num", &BSPNode_v1::planeNum)
		.def_rw("children", &BSPNode_v1::children)
		.def_rw("mins", &BSPNode_v1::mins)
		.def_rw("maxs", &BSPNode_v1::maxs)
		.def_rw("first_face", &BSPNode_v1::firstFace)
		.def_rw("num_faces", &BSPNode_v1::numFaces)
		.def_rw("area", &BSPNode_v1::area)
		.def_static("upgrade", &BSPNode_v1::upgrade, "old"_a);

	py::class_<BSPTextureInfo_v0>(cBSP, "TextureInfo_v0")
		.def(py::init<>())
		.def_rw("texture_vector1", &BSPTextureInfo_v0::textureVector1)
		.def_rw("texture_vector2", &BSPTextureInfo_v0::textureVector2)
		.def_rw("lightmap_vector1", &BSPTextureInfo_v0::lightmapVector1)
		.def_rw("lightmap_vector2", &BSPTextureInfo_v0::lightmapVector2)
		.def_rw("flags", &BSPTextureInfo_v0::flags)
		.def_rw("texture_data", &BSPTextureInfo_v0::textureData);

	py::class_<BSPFace_v1>(cBSP, "Face_v1")
		.def(py::init<>())
		.def_rw("plane_num", &BSPFace_v1::planeNum)
		.def_rw("side", &BSPFace_v1::side)
		.def_rw("on_node", &BSPFace_v1::onNode)
		.def_rw("first_edge", &BSPFace_v1::firstEdge)
		.def_rw("num_edges", &BSPFace_v1::numEdges)
		.def_rw("tex_info", &BSPFace_v1::texInfo)
		.def_rw("disp_info", &BSPFace_v1::dispInfo)
		.def_rw("surface_fog_volume_id", &BSPFace_v1::surfaceFogVolumeID)
		.def_rw("styles", &BSPFace_v1::styles)
		.def_rw("light_offset", &BSPFace_v1::lightOffset)
		.def_rw("area", &BSPFace_v1::area)
		.def_rw("lightmap_texture_mins_in_luxels", &BSPFace_v1::lightmapTextureMinsInLuxels)
		.def_rw("lightmap_texture_size_in_luxels", &BSPFace_v1::lightmapTextureSizeInLuxels)
		.def_rw("original_face", &BSPFace_v1::originalFace)
		.def_rw("num_prims", &BSPFace_v1::numPrims)
		.def_rw("first_prim_id", &BSPFace_v1::firstPrimID)
		.def_rw("smoothing_groups", &BSPFace_v1::smoothingGroups);

	py::class_<BSPFace_v2>(cBSP, "Face_v2")
		.def(py::init<>())
		.def_rw("plane_num", &BSPFace_v2::planeNum)
		.def_rw("side", &BSPFace_v2::side)
		.def_rw("on_node", &BSPFace_v2::onNode)
		.def_rw("first_edge", &BSPFace_v2::firstEdge)
		.def_rw("num_edges", &BSPFace_v2::numEdges)
		.def_rw("tex_info", &BSPFace_v2::texInfo)
		.def_rw("disp_info", &BSPFace_v2::dispInfo)
		.def_rw("surface_fog_volume_id", &BSPFace_v2::surfaceFogVolumeID)
		.def_rw("styles", &BSPFace_v2::styles)
		.def_rw("light_offset", &BSPFace_v2::lightOffset)
		.def_rw("area", &BSPFace_v2::area)
		.def_rw("lightmap_texture_mins_in_luxels", &BSPFace_v2::lightmapTextureMinsInLuxels)
		.def_rw("lightmap_texture_size_in_luxels", &BSPFace_v2::lightmapTextureSizeInLuxels)
		.def_rw("original_face", &BSPFace_v2::originalFace)
		.def_prop_rw("enable_shadows", [](const BSPFace_v2& self) -> bool { return self.enableShadows; }, [](BSPFace_v2& self, bool enableShadows) { self.enableShadows = enableShadows; })
		.def_prop_rw("num_prims", [](const BSPFace_v2& self) { return self.numPrims; }, [](BSPFace_v2& self, bool numPrims) { self.numPrims = numPrims; })
		.def_rw("first_prim_id", &BSPFace_v2::firstPrimID)
		.def_rw("smoothing_groups", &BSPFace_v2::smoothingGroups)
		.def_static("upgrade", &BSPFace_v2::upgrade);

	py::class_<BSPEdge_v0>(cBSP, "Edge_v0")
		.def(py::init<>())
		.def_rw("v0", &BSPEdge_v0::v0)
		.def_rw("v1", &BSPEdge_v0::v1);

	py::class_<BSPEdge_v1>(cBSP, "Edge_v1")
		.def(py::init<>())
		.def_rw("v0", &BSPEdge_v1::v0)
		.def_rw("v1", &BSPEdge_v1::v1)
		.def_static("upgrade", &BSPEdge_v1::upgrade);

	py::class_<BSPSurfEdge_v0>(cBSP, "SurfEdge_v0")
		.def(py::init<>())
		.def_rw("surf_edge", &BSPSurfEdge_v0::surfEdge);

	py::class_<BSPBrushModel_v0>(cBSP, "BrushModel_v0")
		.def(py::init<>())
		.def_rw("min", &BSPBrushModel_v0::min)
		.def_rw("max", &BSPBrushModel_v0::max)
		.def_rw("origin", &BSPBrushModel_v0::origin)
		.def_rw("head_node", &BSPBrushModel_v0::headNode)
		.def_rw("first_face", &BSPBrushModel_v0::firstFace)
		.def_rw("num_faces", &BSPBrushModel_v0::numFaces);

	auto cBSPGameLump = py::class_<BSPGameLump>(cBSP, "GameLump");

	py::enum_<BSPGameLump::Signature>(cBSPGameLump, "Signature")
		.value("STATIC_PROPS",             BSPGameLump::SIGNATURE_STATIC_PROPS)
		.value("DETAIL_PROPS",             BSPGameLump::SIGNATURE_DETAIL_PROPS)
		.value("DETAIL_PROP_LIGHTING_LDR", BSPGameLump::SIGNATURE_DETAIL_PROP_LIGHTING_LDR)
		.value("DETAIL_PROP_LIGHTING_HDR", BSPGameLump::SIGNATURE_DETAIL_PROP_LIGHTING_HDR);

	cBSPGameLump
		.def_rw("signature", &BSPGameLump::signature)
		.def_rw("is_compressed", &BSPGameLump::isCompressed)
		.def_rw("version", &BSPGameLump::version)
		.def_rw("offset", &BSPGameLump::offset)
		.def_rw("uncompressed_length", &BSPGameLump::uncompressedLength)
		.def_prop_rw("data", [](const BSPGameLump& self) {
			return py::bytes{self.data.data(), self.data.size()};
		}, [](BSPGameLump& self, const py::bytes& data) {
			self.data = {static_cast<const std::byte*>(data.data()), static_cast<const std::byte*>(data.data()) + data.size()};
		});

	bsppp.attr("BSP_SIGNATURE") = BSP_SIGNATURE;
	bsppp.attr("BSP_LUMP_COUNT") = BSP_LUMP_COUNT;
	bsppp.attr("BSP_LUMP_ORDER") = BSP_LUMP_ORDER;

	py::enum_<BSPLump>(cBSP, "Lump")
		.value("UNKNOWN",                        BSPLump::UNKNOWN)
		.value("ENTITIES",                       BSPLump::ENTITIES)
		.value("PLANES",                         BSPLump::PLANES)
		.value("TEXDATA",                        BSPLump::TEXDATA)
		.value("VERTEXES",                       BSPLump::VERTEXES)
		.value("VISIBILITY",                     BSPLump::VISIBILITY)
		.value("NODES",                          BSPLump::NODES)
		.value("TEXINFO",                        BSPLump::TEXINFO)
		.value("FACES",                          BSPLump::FACES)
		.value("LIGHTING",                       BSPLump::LIGHTING)
		.value("OCCLUSION",                      BSPLump::OCCLUSION)
		.value("LEAFS",                          BSPLump::LEAFS)
		.value("FACEIDS",                        BSPLump::FACEIDS)
		.value("EDGES",                          BSPLump::EDGES)
		.value("SURFEDGES",                      BSPLump::SURFEDGES)
		.value("MODELS",                         BSPLump::MODELS)
		.value("WORLDLIGHTS",                    BSPLump::WORLDLIGHTS)
		.value("LEAFFACES",                      BSPLump::LEAFFACES)
		.value("LEAFBRUSHES",                    BSPLump::LEAFBRUSHES)
		.value("BRUSHES",                        BSPLump::BRUSHES)
		.value("BRUSHSIDES",                     BSPLump::BRUSHSIDES)
		.value("AREAS",                          BSPLump::AREAS)
		.value("AREAPORTALS",                    BSPLump::AREAPORTALS)
		.value("S2004_PORTALS",                  BSPLump::S2004_PORTALS)
		.value("UNUSED0",                        BSPLump::UNUSED0)
		.value("SL4D2_PROPCOLLISION",            BSPLump::SL4D2_PROPCOLLISION)
		.value("S2004_CLUSTERS",                 BSPLump::S2004_CLUSTERS)
		.value("UNUSED1",                        BSPLump::UNUSED1)
		.value("SL4D2_PROPHULLS",                BSPLump::SL4D2_PROPHULLS)
		.value("S2004_PORTALVERTS",              BSPLump::S2004_PORTALVERTS)
		.value("UNUSED2",                        BSPLump::UNUSED2)
		.value("SL4D2_PROPHULLVERTS",            BSPLump::SL4D2_PROPHULLVERTS)
		.value("S2004_CLUSTERPORTALS",           BSPLump::S2004_CLUSTERPORTALS)
		.value("UNUSED3",                        BSPLump::UNUSED3)
		.value("SL4D2_PROPTRIS",                 BSPLump::SL4D2_PROPTRIS)
		.value("DISPINFO",                       BSPLump::DISPINFO)
		.value("ORIGINALFACES",                  BSPLump::ORIGINALFACES)
		.value("PHYSDISP",                       BSPLump::PHYSDISP)
		.value("PHYSCOLLIDE",                    BSPLump::PHYSCOLLIDE)
		.value("VERTNORMALS",                    BSPLump::VERTNORMALS)
		.value("VERTNORMALINDICES",              BSPLump::VERTNORMALINDICES)
		.value("S2004_DISP_LIGHTMAP_ALPHAS",     BSPLump::S2004_DISP_LIGHTMAP_ALPHAS)
		.value("UNUSED4",                        BSPLump::UNUSED4)
		.value("DISP_VERTS",                     BSPLump::DISP_VERTS)
		.value("DISP_LIGHTMAP_SAMPLE_POSITIONS", BSPLump::DISP_LIGHTMAP_SAMPLE_POSITIONS)
		.value("GAME_LUMP",                      BSPLump::GAME_LUMP)
		.value("LEAFWATERDATA",                  BSPLump::LEAFWATERDATA)
		.value("PRIMITIVES",                     BSPLump::PRIMITIVES)
		.value("PRIMVERTS",                      BSPLump::PRIMVERTS)
		.value("PRIMINDICES",                    BSPLump::PRIMINDICES)
		.value("PAKFILE",                        BSPLump::PAKFILE)
		.value("CLIPPORTALVERTS",                BSPLump::CLIPPORTALVERTS)
		.value("CUBEMAPS",                       BSPLump::CUBEMAPS)
		.value("TEXDATA_STRING_DATA",            BSPLump::TEXDATA_STRING_DATA)
		.value("TEXDATA_STRING_TABLE",           BSPLump::TEXDATA_STRING_TABLE)
		.value("OVERLAYS",                       BSPLump::OVERLAYS)
		.value("LEAFMINDISTTOWATER",             BSPLump::LEAFMINDISTTOWATER)
		.value("FACE_MACRO_TEXTURE_INFO",        BSPLump::FACE_MACRO_TEXTURE_INFO)
		.value("DISP_TRIS",                      BSPLump::DISP_TRIS)
		.value("S2004_PHYSCOLLIDESURFACE",       BSPLump::S2004_PHYSCOLLIDESURFACE)
		.value("UNUSED5",                        BSPLump::UNUSED5)
		.value("SL4D2_PROP_BLOB",                BSPLump::SL4D2_PROP_BLOB)
		.value("WATEROVERLAYS",                  BSPLump::WATEROVERLAYS)
		.value("S2006_XBOX_LIGHTMAPPAGES",       BSPLump::S2006_XBOX_LIGHTMAPPAGES)
		.value("LEAF_AMBIENT_INDEX_HDR",         BSPLump::LEAF_AMBIENT_INDEX_HDR)
		.value("S2006_XBOX_LIGHTMAPPAGEINFOS",   BSPLump::S2006_XBOX_LIGHTMAPPAGEINFOS)
		.value("LEAF_AMBIENT_INDEX",             BSPLump::LEAF_AMBIENT_INDEX)
		.value("LIGHTING_HDR",                   BSPLump::LIGHTING_HDR)
		.value("WORLDLIGHTS_HDR",                BSPLump::WORLDLIGHTS_HDR)
		.value("LEAF_AMBIENT_LIGHTING_HDR",      BSPLump::LEAF_AMBIENT_LIGHTING_HDR)
		.value("LEAF_AMBIENT_LIGHTING",          BSPLump::LEAF_AMBIENT_LIGHTING)
		.value("XBOX_XZIPPAKFILE",               BSPLump::XBOX_XZIPPAKFILE)
		.value("FACES_HDR",                      BSPLump::FACES_HDR)
		.value("MAP_FLAGS",                      BSPLump::MAP_FLAGS)
		.value("OVERLAY_FADES",                  BSPLump::OVERLAY_FADES)
		.value("L4D_OVERLAY_SYSTEM_LEVELS",      BSPLump::L4D_OVERLAY_SYSTEM_LEVELS)
		.value("UNUSED6",                        BSPLump::UNUSED6)
		.value("L4D2_PHYSLEVEL",                 BSPLump::L4D2_PHYSLEVEL)
		.value("UNUSED7",                        BSPLump::UNUSED7)
		.value("ASW_DISP_MULTIBLEND",            BSPLump::ASW_DISP_MULTIBLEND)
		.value("UNUSED8",                        BSPLump::UNUSED8);

	cBSP
		.def(py::init<std::string, bool>(), "path"_a, "load_patch_files"_a = true)
		.def("__bool__", &BSP::operator bool, py::is_operator())
		.def_static("create", &BSP::create, "path"_a, "version"_a = 21, "map_revision"_a = 0)
		.def_prop_rw("version", &BSP::getVersion, &BSP::setVersion)
		.def_prop_rw("map_revision", &BSP::getMapRevision, &BSP::setMapRevision)
		.def_prop_rw("l4d2", &BSP::isL4D2, &BSP::setL4D2)
		.def_prop_rw("console", &BSP::isConsole, &BSP::setConsole)
		.def("has_lump", &BSP::hasLump, "lump_index"_a)
		.def("is_lump_compressed", &BSP::isLumpCompressed, "lump_index"_a)
		.def("get_lump_version", &BSP::getLumpVersion, "lump_index"_a)
		.def("get_lump_data", [](const BSP& self, BSPLump lumpIndex, bool noDecompression = false) -> py::object {
			const auto d = self.getLumpData(lumpIndex, noDecompression);
			return d ? py::bytes{d->data(), d->size()} : py::none();
		}, "lump_index"_a, "no_decompression"_a = false)
		.def("get_lump_data_for_entities", &BSP::getLumpData<BSPLump::ENTITIES>)
		.def("get_lump_data_for_planes", &BSP::getLumpData<BSPLump::PLANES>)
		.def("get_lump_data_for_texdata", &BSP::getLumpData<BSPLump::TEXDATA>)
		.def("get_lump_data_for_vertexes", &BSP::getLumpData<BSPLump::VERTEXES>)
		.def("get_lump_data_for_nodes", &BSP::getLumpData<BSPLump::NODES>)
		.def("get_lump_data_for_texinfo", &BSP::getLumpData<BSPLump::TEXINFO>)
		.def("get_lump_data_for_faces", &BSP::getLumpData<BSPLump::FACES>)
		.def("get_lump_data_for_edges", &BSP::getLumpData<BSPLump::EDGES>)
		.def("get_lump_data_for_surfedges", &BSP::getLumpData<BSPLump::SURFEDGES>)
		.def("get_lump_data_for_models", &BSP::getLumpData<BSPLump::MODELS>)
		.def("get_lump_data_for_originalfaces", &BSP::getLumpData<BSPLump::ORIGINALFACES>)
		.def("get_lump_data_for_game_lump", &BSP::getLumpData<BSPLump::GAME_LUMP>)
		.def("set_lump", [](BSP& self, BSPLump lumpIndex, uint32_t version, const py::bytes& data, uint8_t compressLevel = 0) {
			self.setLump(lumpIndex, version, {static_cast<const std::byte*>(data.data()), data.size()}, compressLevel);
		}, "lump_index"_a, "version"_a, "data"_a, "compress_level"_a = 0)
		.def("set_lump_for_entities", [](BSP& self, uint32_t version, const std::vector<BSPEntityKeyValues>& data, uint8_t compressLevel = 0) {
			self.setLump(version, data, compressLevel);
		}, "version"_a, "data"_a, "compress_level"_a)
		.def("is_game_lump_compressed", &BSP::isGameLumpCompressed, "signature"_a)
		.def("get_game_lump_data", [](const BSP& self, BSPGameLump::Signature signature) -> py::object {
			const auto d = self.getGameLumpData(signature);
			return d ? py::bytes{d->data(), d->size()} : py::none();
		}, "signature"_a)
		.def("set_game_lump", &BSP::setGameLump, "signature"_a, "version"_a, "data"_a, "compress_level"_a = 0)
		.def("reset_lump", &BSP::resetLump, "lump_index"_a)
		.def("reset", &BSP::reset)
		.def("create_lump_patch_file", &BSP::createLumpPatchFile, "lump_index"_a)
		.def("set_lump_from_patch_file", &BSP::setLumpFromPatchFile, "lump_file_path"_a)
		.def("bake", &BSP::bake, "output_path"_a = "");

	bsppp.attr("BSP_EXTENSION") = BSP_EXTENSION;

	py::class_<PakLump, vpkpp::PackFile>(bsppp, "PakLump")
		.def_static("open", &PakLump::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &PakLump::GUID);
}

} // namespace bsppp
