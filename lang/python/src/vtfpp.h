#pragma once

#include <tuple>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>

namespace py = nanobind;

#include <vtfpp/vtfpp.h>

namespace vtfpp {

void register_python(py::module_& m) {
	using namespace vtfpp;
	auto vtfpp = m.def_submodule("vtfpp");

	py::enum_<ImageFormat>(vtfpp, "ImageFormat")
			.value("RGBA8888",           ImageFormat::RGBA8888)
			.value("ABGR8888",           ImageFormat::ABGR8888)
			.value("RGB888",             ImageFormat::RGB888)
			.value("BGR888",             ImageFormat::BGR888)
			.value("RGB565",             ImageFormat::RGB565)
			.value("I8",                 ImageFormat::I8)
			.value("IA88",               ImageFormat::IA88)
			.value("P8",                 ImageFormat::P8)
			.value("A8",                 ImageFormat::A8)
			.value("RGB888_BLUESCREEN",  ImageFormat::RGB888_BLUESCREEN)
			.value("BGR888_BLUESCREEN",  ImageFormat::BGR888_BLUESCREEN)
			.value("ARGB8888",           ImageFormat::ARGB8888)
			.value("BGRA8888",           ImageFormat::BGRA8888)
			.value("DXT1",               ImageFormat::DXT1)
			.value("DXT3",               ImageFormat::DXT3)
			.value("DXT5",               ImageFormat::DXT5)
			.value("BGRX8888",           ImageFormat::BGRX8888)
			.value("BGR565",             ImageFormat::BGR565)
			.value("BGRX5551",           ImageFormat::BGRX5551)
			.value("BGRA4444",           ImageFormat::BGRA4444)
			.value("DXT1_ONE_BIT_ALPHA", ImageFormat::DXT1_ONE_BIT_ALPHA)
			.value("BGRA5551",           ImageFormat::BGRA5551)
			.value("UV88",               ImageFormat::UV88)
			.value("UVWQ8888",           ImageFormat::UVWQ8888)
			.value("RGBA16161616F",      ImageFormat::RGBA16161616F)
			.value("RGBA16161616",       ImageFormat::RGBA16161616)
			.value("UVLX8888",           ImageFormat::UVLX8888)
			.value("R32F",               ImageFormat::R32F)
			.value("RGB323232F",         ImageFormat::RGB323232F)
			.value("RGBA32323232F",      ImageFormat::RGBA32323232F)
			.value("RG1616F",            ImageFormat::RG1616F)
			.value("RG3232F",            ImageFormat::RG3232F)
			.value("RGBX8888",           ImageFormat::RGBX8888)
			.value("EMPTY",              ImageFormat::EMPTY)
			.value("ATI2N",              ImageFormat::ATI2N)
			.value("ATI1N",              ImageFormat::ATI1N)
			.value("RGBA1010102",        ImageFormat::RGBA1010102)
			.value("BGRA1010102",        ImageFormat::BGRA1010102)
			.value("R16F",               ImageFormat::R16F)
			.value("R8",                 ImageFormat::R8)
			.value("BC7",                ImageFormat::BC7)
			.value("BC6H",               ImageFormat::BC6H)
			.export_values();

	{
		using namespace ImageFormatDetails;
		auto ImageFormatDetails = vtfpp.def_submodule("ImageFormatDetails");

		ImageFormatDetails.def("red",               &red,               py::arg("format"));
		ImageFormatDetails.def("decompressedRed",   &decompressedRed,   py::arg("format"));
		ImageFormatDetails.def("green",             &green,             py::arg("format"));
		ImageFormatDetails.def("decompressedGreen", &decompressedGreen, py::arg("format"));
		ImageFormatDetails.def("blue",              &blue,              py::arg("format"));
		ImageFormatDetails.def("decompressedBlue",  &decompressedBlue,  py::arg("format"));
		ImageFormatDetails.def("alpha",             &alpha,             py::arg("format"));
		ImageFormatDetails.def("decompressedAlpha", &decompressedAlpha, py::arg("format"));
		ImageFormatDetails.def("bpp",               &bpp,               py::arg("format"));
		ImageFormatDetails.def("containerFormat",   &containerFormat,   py::arg("format"));
		ImageFormatDetails.def("large",             &large,             py::arg("format"));
		ImageFormatDetails.def("decimal",           &decimal,           py::arg("format"));
		ImageFormatDetails.def("compressed",        &compressed,        py::arg("format"));
		ImageFormatDetails.def("transparent",       &transparent,       py::arg("format"));
		ImageFormatDetails.def("opaque",            &opaque,            py::arg("format"));

		ImageFormatDetails.def("get_data_length", py::overload_cast<ImageFormat, uint16_t, uint16_t, uint16_t>(&getDataLength), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("slice_count") = 1);
		ImageFormatDetails.def("get_data_length_extended", py::overload_cast<ImageFormat, uint8_t, uint16_t, uint8_t, uint16_t, uint16_t, uint16_t>(&getDataLength), py::arg("format"), py::arg("mip_count"), py::arg("frame_count"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice_count"));
		ImageFormatDetails.def("get_data_position", [](ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) -> std::pair<uint32_t, uint32_t> {
			uint32_t offset, length;
			if (getDataPosition(offset, length, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount)) {
				return {offset, length};
			}
			return {0, 0};
		}, py::arg("format"), py::arg("mip"), py::arg("mip_count"), py::arg("frame"), py::arg("frame_count"), py::arg("face"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice") = 0, py::arg("slice_count") = 1);
	}

	{
		using namespace ImageDimensions;
		auto ImageDimensions = vtfpp.def_submodule("ImageDimensions");

		ImageDimensions.def("get_mip_dim", &getMipDim, py::arg("mip"), py::arg("dim"));
		ImageDimensions.def("get_recommended_mip_count_for_dims", &getRecommendedMipCountForDims, py::arg("format"), py::arg("width"), py::arg("height"));
	}

	// Skip ImagePixel, difficult to bind

	{
		using namespace ImageConversion;
		auto ImageConversion = vtfpp.def_submodule("ImageConversion");

		// todo(python): still need to bind the following:
		ImageConversion.def("convert_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
			const auto d = convertImageDataToFormat({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, width, height);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("old_format"), py::arg("new_format"), py::arg("width"), py::arg("height"));

		ImageConversion.def("convert_several_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount) {
			const auto d = convertSeveralImageDataToFormat({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, mipCount, frameCount, faceCount, width, height, sliceCount);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("old_format"), py::arg("new_format"), py::arg("mip_count"), py::arg("frame_count"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice_count"));

		py::enum_<FileFormat>(ImageConversion, "FileFormat")
				.value("DEFAULT", FileFormat::DEFAULT)
				.value("PNG",     FileFormat::PNG)
				.value("JPEG",    FileFormat::JPEG)
				.value("BMP",     FileFormat::BMP)
				.value("TGA",     FileFormat::TGA)
				.value("HDR",     FileFormat::HDR)
				.export_values();

		ImageConversion.def("get_default_file_format_for_image_format", &getDefaultFileFormatForImageFormat, py::arg("format"));

		ImageConversion.def("convert_image_data_to_file", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat = FileFormat::DEFAULT) {
			const auto d = convertImageDataToFile({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("file_format") = FileFormat::DEFAULT);

		ImageConversion.def("convert_file_to_image_data", [](const py::bytes& fileData) -> std::tuple<py::bytes, ImageFormat, int, int, int> {
			ImageFormat format;
			int width, height, frame;
			const auto d = convertFileToImageData({reinterpret_cast<const std::byte*>(fileData.data()), fileData.size()}, format, width, height, frame);
			return {py::bytes{d.data(), d.size()}, format, width, height, frame};
		}, py::arg("file_data"));

		py::enum_<ResizeEdge>(ImageConversion, "ResizeEdge")
				.value("CLAMP",   ResizeEdge::CLAMP)
				.value("REFLECT", ResizeEdge::REFLECT)
				.value("WRAP",    ResizeEdge::WRAP)
				.value("ZERO",    ResizeEdge::ZERO)
				.export_values();

		py::enum_<ResizeFilter>(ImageConversion, "ResizeFilter")
				.value("DEFAULT",       ResizeFilter::DEFAULT)
				.value("BOX",           ResizeFilter::BOX)
				.value("BILINEAR",      ResizeFilter::BILINEAR)
				.value("CUBIC_BSPLINE", ResizeFilter::CUBIC_BSPLINE)
				.value("CATMULLROM",    ResizeFilter::CATMULLROM)
				.value("MITCHELL",      ResizeFilter::MITCHELL)
				.export_values();

		py::enum_<ResizeMethod>(ImageConversion, "ResizeMethod")
				.value("NONE",                 ResizeMethod::NONE)
				.value("POWER_OF_TWO_BIGGER",  ResizeMethod::POWER_OF_TWO_BIGGER)
				.value("POWER_OF_TWO_SMALLER", ResizeMethod::POWER_OF_TWO_SMALLER)
				.value("POWER_OF_TWO_NEAREST", ResizeMethod::POWER_OF_TWO_NEAREST)
				.export_values();

		ImageConversion.def("get_resized_dim", &getResizedDim, py::arg("n"), py::arg("resize_method"));
		ImageConversion.def("get_resized_dims", [](uint16_t width, ResizeMethod widthResize, uint16_t height, ResizeMethod heightResize) -> std::pair<uint16_t, uint16_t> {
			setResizedDims(width, widthResize, height, heightResize);
			return {width, height};
		}, py::arg("width"), py::arg("resize_width"), py::arg("height"), py::arg("resize_height"));

		ImageConversion.def("resize_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP) {
			const auto d = resizeImageData({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, height, newHeight, srgb, filter, edge);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("new_width"), py::arg("height"), py::arg("new_height"), py::arg("srgb"), py::arg("filter"), py::arg("edge") = ResizeEdge::CLAMP);

		ImageConversion.def("resize_image_data_strict", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP) -> std::tuple<py::bytes, int, int> {
			uint16_t widthOut, heightOut;
			const auto d = resizeImageDataStrict({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, widthOut, widthResize, height, newHeight, heightOut, heightResize, srgb, filter, edge);
			return {py::bytes{d.data(), d.size()}, widthOut, heightOut};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("new_width"), py::arg("width_resize"), py::arg("height"), py::arg("new_height"), py::arg("height_resize"), py::arg("srgb"), py::arg("filter"), py::arg("edge") = ResizeEdge::CLAMP);

		// Skip extractChannelFromImageData, difficult to bind
	}

	py::class_<PPL::Image>(vtfpp, "PPLImage")
			.def_ro("width",  &PPL::Image::width)
			.def_ro("height", &PPL::Image::height)
			.def_prop_ro("data", [](const PPL::Image& self) {
				return py::bytes{self.data.data(), self.data.size()};
			});

	py::class_<PPL>(vtfpp, "PPL")
			.def(py::init<uint32_t, ImageFormat, uint32_t>(), py::arg("checksum"), py::arg("format") = ImageFormat::RGB888, py::arg("version") = 0)
			.def("__init__", [](PPL* self, const py::bytes& pplData) {
				return new(self) PPL{{reinterpret_cast<const std::byte*>(pplData.data()), pplData.size()}};
			}, py::arg("ppl_data"))
			.def(py::init<const std::string&>(), py::arg("path"))
			.def("__bool__", &PPL::operator bool, py::is_operator())
			.def_prop_rw("version", &PPL::getVersion, &PPL::setVersion)
			.def_prop_rw("checksum", &PPL::getChecksum, &PPL::setChecksum)
			.def_prop_rw("format", &PPL::getFormat, &PPL::setFormat)
			.def("has_image_for_lod", &PPL::hasImageForLOD, py::arg("lod"))
			.def_prop_ro("image_lods", &PPL::getImageLODs)
			.def("get_image_raw", [](const PPL& self, uint32_t lod = 0) -> std::optional<PPL::Image> {
				const auto* image = self.getImageRaw(lod);
				if (!image) {
					return std::nullopt;
				}
				return *image;
			}, py::arg("lod"))
			.def("get_image_as", &PPL::getImageAs, py::arg("new_format"), py::arg("lod"))
			.def("get_image_as_rgb888", &PPL::getImageAsRGB888, py::arg("lod"))
			.def("set_image", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t lod = 0) {
				self.setImage({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, lod);
			}, py::arg("imageData"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("lod") = 0)
			.def("set_image_resized", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR) {
				self.setImage({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, resizedWidth, resizedHeight, lod, filter);
			}, py::arg("imageData"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("resized_width"), py::arg("resized_height"), py::arg("lod") = 0, py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR)
			.def("set_image_from_file", py::overload_cast<const std::string&, uint32_t>(&PPL::setImage), py::arg("image_path"), py::arg("lod") = 0)
			.def("set_image_resized_from_file", py::overload_cast<const std::string&, uint32_t, uint32_t, uint32_t, ImageConversion::ResizeFilter>(&PPL::setImage), py::arg("image_path"), py::arg("resized_width"), py::arg("resized_height"), py::arg("lod") = 0, py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR)
			.def("save_image", [](const PPL& self, uint32_t lod = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
				const auto d = self.saveImageToFile(lod, fileFormat);
				return py::bytes{d.data(), d.size()};
			}, py::arg("lod") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("save_image_to_file", py::overload_cast<const std::string&, uint32_t, ImageConversion::FileFormat>(&PPL::saveImageToFile, py::const_), py::arg("image_path"), py::arg("lod") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("bake", [](PPL& self) {
				const auto d = self.bake();
				return py::bytes{d.data(), d.size()};
			})
			.def("bake_to_file", py::overload_cast<const std::string&>(&PPL::bake), py::arg("ppl_path"));

	vtfpp.attr("VTF_SIGNATURE") = VTF_SIGNATURE;

	py::enum_<CompressionMethod>(vtfpp, "CompressionMethod")
			.value("DEFLATE", CompressionMethod::DEFLATE)
			.value("ZSTD",    CompressionMethod::ZSTD)
			.export_values();

	py::enum_<Resource::Type>(vtfpp, "ResourceType")
			.value("UNKNOWN",             Resource::TYPE_UNKNOWN)
			.value("THUMBNAIL_DATA",      Resource::TYPE_THUMBNAIL_DATA)
			.value("IMAGE_DATA",          Resource::TYPE_IMAGE_DATA)
			.value("PARTICLE_SHEET_DATA", Resource::TYPE_PARTICLE_SHEET_DATA)
			.value("CRC",                 Resource::TYPE_CRC)
			.value("LOD_CONTROL_INFO",    Resource::TYPE_LOD_CONTROL_INFO)
			.value("EXTENDED_FLAGS",      Resource::TYPE_EXTENDED_FLAGS)
			.value("KEYVALUES_DATA",      Resource::TYPE_KEYVALUES_DATA)
			.value("AUX_COMPRESSION",     Resource::TYPE_AUX_COMPRESSION)
			.export_values();

	py::enum_<Resource::Flags>(vtfpp, "ResourceFlags")
			.value("NONE",       Resource::FLAG_NONE)
			.value("LOCAL_DATA", Resource::FLAG_LOCAL_DATA)
			.export_values();

	// Skip Resource, mostly useless outside C++

	py::enum_<VTF::Flags>(vtfpp, "VTFFlags")
			.value("NONE",                                    VTF::FLAG_NONE)
			.value("POINT_SAMPLE",                            VTF::FLAG_POINT_SAMPLE)
			.value("TRILINEAR",                               VTF::FLAG_TRILINEAR)
			.value("CLAMP_S",                                 VTF::FLAG_CLAMP_S)
			.value("CLAMP_T",                                 VTF::FLAG_CLAMP_T)
			.value("ANISOTROPIC",                             VTF::FLAG_ANISOTROPIC)
			.value("HINT_DXT5",                               VTF::FLAG_HINT_DXT5)
			.value("SRGB",                                    VTF::FLAG_SRGB)
			.value("NO_COMPRESS",                             VTF::FLAG_NO_COMPRESS)
			.value("NORMAL",                                  VTF::FLAG_NORMAL)
			.value("NO_MIP",                                  VTF::FLAG_NO_MIP)
			.value("NO_LOD",                                  VTF::FLAG_NO_LOD)
			.value("MIN_MIP",                                 VTF::FLAG_MIN_MIP)
			.value("PROCEDURAL",                              VTF::FLAG_PROCEDURAL)
			.value("ONE_BIT_ALPHA",                           VTF::FLAG_ONE_BIT_ALPHA)
			.value("MULTI_BIT_ALPHA",                         VTF::FLAG_MULTI_BIT_ALPHA)
			.value("ENVMAP",                                  VTF::FLAG_ENVMAP)
			.value("RENDERTARGET",                            VTF::FLAG_RENDERTARGET)
			.value("DEPTH_RENDERTARGET",                      VTF::FLAG_DEPTH_RENDERTARGET)
			.value("NO_DEBUG_OVERRIDE",                       VTF::FLAG_NO_DEBUG_OVERRIDE)
			.value("SINGLE_COPY",                             VTF::FLAG_SINGLE_COPY)
			.value("ONE_OVER_MIP_LEVEL_IN_ALPHA",             VTF::FLAG_ONE_OVER_MIP_LEVEL_IN_ALPHA)
			.value("PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL", VTF::FLAG_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL)
			.value("NORMAL_TO_DUDV",                          VTF::FLAG_NORMAL_TO_DUDV)
			.value("ALPHA_TEST_MIP_GENERATION",               VTF::FLAG_ALPHA_TEST_MIP_GENERATION)
			.value("NO_DEPTH_BUFFER",                         VTF::FLAG_NO_DEPTH_BUFFER)
			.value("NICE_FILTERED",                           VTF::FLAG_NICE_FILTERED)
			.value("CLAMP_U",                                 VTF::FLAG_CLAMP_U)
			.value("VERTEX_TEXTURE",                          VTF::FLAG_VERTEX_TEXTURE)
			.value("SSBUMP",                                  VTF::FLAG_SSBUMP)
			.value("UNFILTERABLE_OK",                         VTF::FLAG_UNFILTERABLE_OK)
			.value("BORDER",                                  VTF::FLAG_BORDER)
			.value("SPECVAR_RED",                             VTF::FLAG_SPECVAR_RED)
			.value("SPECVAR_ALPHA",                           VTF::FLAG_SPECVAR_ALPHA)
			.export_values();

	py::class_<VTF::CreationOptions>(vtfpp, "VTFCreationOptions")
			.def(py::init<>())
			.def_rw("major_version",        &VTF::CreationOptions::majorVersion)
			.def_rw("minor_version",        &VTF::CreationOptions::minorVersion)
			.def_rw("output_format",        &VTF::CreationOptions::outputFormat)
			.def_rw("width_resize_method",  &VTF::CreationOptions::widthResizeMethod)
			.def_rw("height_resize_method", &VTF::CreationOptions::heightResizeMethod)
			.def_rw("filter",               &VTF::CreationOptions::filter)
			.def_rw("flags",                &VTF::CreationOptions::flags)
			.def_rw("initial_frame_count",  &VTF::CreationOptions::initialFrameCount)
			.def_rw("start_frame",          &VTF::CreationOptions::startFrame)
			.def_rw("is_cubemap",           &VTF::CreationOptions::isCubeMap)
			.def_rw("has_spheremap",        &VTF::CreationOptions::hasSphereMap)
			.def_rw("initial_slice_count",  &VTF::CreationOptions::initialSliceCount)
			.def_rw("create_mips",          &VTF::CreationOptions::createMips)
			.def_rw("create_thumbnail",     &VTF::CreationOptions::createThumbnail)
			.def_rw("create_reflectivity",  &VTF::CreationOptions::createReflectivity)
			.def_rw("compression_level",    &VTF::CreationOptions::compressionLevel)
			.def_rw("compression_method",   &VTF::CreationOptions::compressionMethod)
			.def_rw("bumpmap_scale",        &VTF::CreationOptions::bumpMapScale);

	py::class_<VTF>(vtfpp, "VTF")
			.def_ro_static("FLAG_MASK_GENERATED", &VTF::FLAG_MASK_GENERATED)
			.def_ro_static("FORMAT_UNCHANGED",    &VTF::FORMAT_UNCHANGED)
			.def_ro_static("FORMAT_DEFAULT",      &VTF::FORMAT_DEFAULT)
			.def_ro_static("MAX_RESOURCES",       &VTF::MAX_RESOURCES)
			.def(py::init<>())
			.def("__init__", [](VTF* self, const py::bytes& vtfData, bool parseHeaderOnly = false) {
				return new(self) VTF{std::span{reinterpret_cast<const std::byte*>(vtfData.data()), vtfData.size()}, parseHeaderOnly};
			}, py::arg("vtf_data"), py::arg("parse_header_only") = false)
			.def(py::init<const std::string&, bool>(), py::arg("vtf_path"), py::arg("parse_header_only") = false)
			.def("__bool__", &VTF::operator bool, py::is_operator())
			.def_static("create_and_bake", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, VTF::CreationOptions options) {
				VTF::create({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, vtfPath, options);
			}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_static("create_blank_and_bake", py::overload_cast<ImageFormat, uint16_t, uint16_t, const std::string&, VTF::CreationOptions>(&VTF::create), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_static("create", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, VTF::CreationOptions options) {
				return VTF::create({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, options);
			}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_static("create_blank", py::overload_cast<ImageFormat, uint16_t, uint16_t, VTF::CreationOptions>(&VTF::create), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_static("create_from_file_and_bake", py::overload_cast<const std::string&, const std::string&, VTF::CreationOptions>(&VTF::create), py::arg("image_path"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_static("create_from_file", py::overload_cast<const std::string&, VTF::CreationOptions>(&VTF::create), py::arg("image_path"), py::arg("creation_options") = VTF::CreationOptions{})
			.def_prop_rw("version_major", &VTF::getMajorVersion, &VTF::setMajorVersion)
			.def_prop_rw("version_minor", &VTF::getMinorVersion, &VTF::setMinorVersion)
			.def_prop_rw("image_width_resize_method", &VTF::getImageWidthResizeMethod, &VTF::setImageWidthResizeMethod)
			.def_prop_rw("image_height_resize_method", &VTF::getImageHeightResizeMethod, &VTF::setImageHeightResizeMethod)
			.def_prop_ro("width", &VTF::getWidth)
			.def("width_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getWidth(mip); }, py::arg("mip") = 0)
			.def_prop_ro("height", &VTF::getHeight)
			.def("height_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getHeight(mip); }, py::arg("mip") = 0)
			.def("set_size", &VTF::setSize, py::arg("width"), py::arg("height"), py::arg("filter"))
			.def_prop_rw("flags", &VTF::getFlags, &VTF::setFlags)
			.def("add_flags", &VTF::addFlags, py::arg("flags"))
			.def("remove_flags", &VTF::removeFlags, py::arg("flags"))
			.def_prop_ro("format", &VTF::getFormat)
			.def("set_format", &VTF::setFormat, py::arg("new_format"), py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR)
			.def_prop_rw("mip_count", &VTF::getMipCount, &VTF::setMipCount)
			.def("set_recommended_mip_count", &VTF::setRecommendedMipCount)
			.def("compute_mips", &VTF::computeMips, py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR)
			.def_prop_rw("frame_count", &VTF::getFrameCount, &VTF::setFrameCount)
			.def_prop_ro("face_count", &VTF::getFaceCount)
			.def("set_face_count", &VTF::setFaceCount, py::arg("is_cubemap"), py::arg("has_spheremap") = false)
			.def_prop_rw("slice_count", &VTF::getSliceCount, &VTF::setSliceCount)
			.def("set_frame_face_and_slice_count", &VTF::setFrameFaceAndSliceCount, py::arg("new_frame_count"), py::arg("is_cubemap"), py::arg("has_spheremap") = false, py::arg("new_slice_count") = 1)
			.def_prop_rw("start_frame", &VTF::getStartFrame, &VTF::setStartFrame)
			.def_prop_rw("reflectivity", &VTF::getReflectivity, &VTF::setReflectivity)
			.def("compute_reflectivity", &VTF::computeReflectivity)
			.def_prop_rw("bumpmap_scale", &VTF::getBumpMapScale, &VTF::setBumpMapScale)
			.def_prop_ro("thumbnail_format", &VTF::getThumbnailFormat)
			.def_prop_ro("thumbnail_width", &VTF::getThumbnailWidth)
			.def_prop_ro("thumbnail_height", &VTF::getThumbnailHeight)
			// Skip getResources
			// Skip getResource
			.def("set_particle_sheet_resource", [](VTF& self, const py::bytes& value) { return self.setParticleSheetResource({reinterpret_cast<const std::byte*>(value.data()), value.size()}); }, py::arg("value"))
			.def("remove_particle_sheet_resource", &VTF::removeParticleSheetResource)
			.def("set_crc_resource", &VTF::setCRCResource, py::arg("value"))
			.def("remove_crc_resource", &VTF::removeCRCResource)
			.def("set_lod_resource", &VTF::setLODResource, py::arg("u"), py::arg("v"))
			.def("remove_lod_resource", &VTF::removeLODResource)
			.def("set_extended_flags_resource", &VTF::setExtendedFlagsResource, py::arg("value"))
			.def("remove_extended_flags_resource", &VTF::removeExtendedFlagsResource)
			.def("set_keyvalues_data_resource", &VTF::setKeyValuesDataResource, py::arg("value"))
			.def("remove_keyvalues_data_resource", &VTF::removeKeyValuesDataResource)
			.def_prop_rw("compression_level", &VTF::getCompressionLevel, &VTF::setCompressionLevel)
			.def_prop_rw("compression_method", &VTF::getCompressionMethod, &VTF::setCompressionMethod)
			.def("has_image_data", &VTF::hasImageData)
			.def("image_data_is_srgb", &VTF::imageDataIsSRGB)
			.def("get_image_data_raw", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
				const auto d = self.getImageDataRaw(mip, frame, face, slice);
				return py::bytes{d.data(), d.size()};
			}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
			.def("get_image_data_as", [](const VTF& self, ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
				const auto d = self.getImageDataAs(newFormat, mip, frame, face, slice);
				return py::bytes{d.data(), d.size()};
			}, py::arg("new_format"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
			.def("get_image_data_as_rgba8888", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
				const auto d = self.getImageDataAsRGBA8888(mip, frame, face, slice);
				return py::bytes{d.data(), d.size()};
			}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
			.def("set_image", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
				return self.setImage({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, filter, mip, frame, face, slice);
			}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("filter"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
			.def("set_image_from_file", py::overload_cast<const std::string&, ImageConversion::ResizeFilter, uint8_t, uint16_t, uint8_t, uint16_t>(&VTF::setImage), py::arg("image_path"), py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
			.def("save_image", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
				const auto d = self.saveImageToFile(mip, frame, face, slice, fileFormat);
				return py::bytes{d.data(), d.size()};
			}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("save_image_to_file", py::overload_cast<const std::string&, uint8_t, uint16_t, uint8_t, uint16_t, ImageConversion::FileFormat>(&VTF::saveImageToFile, py::const_), py::arg("image_path"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("has_thumbnail_data", &VTF::hasThumbnailData)
			.def("get_thumbnail_data_raw", [](const VTF& self) {
				const auto d = self.getThumbnailDataRaw();
				return py::bytes{d.data(), d.size()};
			})
			.def("get_thumbnail_data_as", [](const VTF& self, ImageFormat newFormat) {
				const auto d = self.getThumbnailDataAs(newFormat);
				return py::bytes{d.data(), d.size()};
			}, py::arg("new_format"))
			.def("get_thumbnail_data_as_rgba8888", [](const VTF& self) {
				const auto d = self.getThumbnailDataAsRGBA8888();
				return py::bytes{d.data(), d.size()};
			})
			.def("set_thumbnail", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height) {
				return self.setThumbnail({reinterpret_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height);
			}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"))
			.def("compute_thumbnail", &VTF::computeThumbnail, py::arg("filter") = ImageConversion::ResizeFilter::BILINEAR)
			.def("remove_thumbnail", &VTF::removeThumbnail)
			.def("save_thumbnail", [](const VTF& self, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
				const auto d = self.saveThumbnailToFile(fileFormat);
				return py::bytes{d.data(), d.size()};
			}, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("save_thumbnail_to_file", py::overload_cast<const std::string&, ImageConversion::FileFormat>(&VTF::saveThumbnailToFile, py::const_), py::arg("image_path"), py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
			.def("bake", [](const VTF& self) {
				const auto d = self.bake();
				return py::bytes{d.data(), d.size()};
			})
			.def("bake_to_file", py::overload_cast<const std::string&>(&VTF::bake, py::const_), py::arg("vtf_path"));
}

} // namespace vtfpp
