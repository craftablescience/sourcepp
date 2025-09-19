#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>

namespace py = nanobind;
using namespace py::literals;

#include <vtfpp/vtfpp.h>

namespace vtfpp {

inline void register_python(py::module_& m) {
	using namespace vtfpp;
	auto vtfpp = m.def_submodule("vtfpp");

	auto cHOT = py::class_<HOT>(vtfpp, "HOT");
	auto cHOTRect = py::class_<HOT::Rect>(cHOT, "Rect");

	py::enum_<HOT::Rect::Flags>(cHOTRect, "Flags", py::is_flag())
		.value("NONE",              HOT::Rect::FLAG_NONE)
		.value("RANDOM_ROTATION",   HOT::Rect::FLAG_RANDOM_ROTATION)
		.value("RANDOM_REFLECTION", HOT::Rect::FLAG_RANDOM_REFLECTION)
		.value("IS_ALTERNATE",      HOT::Rect::FLAG_IS_ALTERNATE);

	cHOTRect
		.def_rw("flags", &HOT::Rect::flags)
		.def_rw("x1",    &HOT::Rect::x1)
		.def_rw("y1",    &HOT::Rect::y1)
		.def_rw("x2",    &HOT::Rect::x2)
		.def_rw("y2",    &HOT::Rect::y2);

	cHOT
		.def(py::init<>())
		.def("__init__", [](HOT* self, const py::bytes& hotData) {
			return new(self) HOT{{static_cast<const std::byte*>(hotData.data()), hotData.size()}};
		}, "hot_data"_a)
		.def(py::init<const std::string&>(), "hot_path"_a)
		.def("__bool__", &HOT::operator bool, py::is_operator())
		.def_prop_rw("version", &HOT::getVersion, &HOT::setVersion)
		.def_prop_rw("flags", &HOT::getFlags, &HOT::setFlags)
		.def_prop_rw("rects", [](const HOT& self) -> std::vector<HOT::Rect> { return self.getRects(); }, [](HOT& self, const std::vector<HOT::Rect>& rects) { self.getRects() = rects; })
		.def("bake", [](const HOT& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&HOT::bake, py::const_), "hot_path"_a);

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
		.value("BC6H",               ImageFormat::BC6H);

	{
		using namespace ImageFormatDetails;
		auto ImageFormatDetails = vtfpp.def_submodule("ImageFormatDetails");

		ImageFormatDetails
			.def("red",               &red,               "format"_a)
			.def("decompressedRed",   &decompressedRed,   "format"_a)
			.def("green",             &green,             "format"_a)
			.def("decompressedGreen", &decompressedGreen, "format"_a)
			.def("blue",              &blue,              "format"_a)
			.def("decompressedBlue",  &decompressedBlue,  "format"_a)
			.def("alpha",             &alpha,             "format"_a)
			.def("decompressedAlpha", &decompressedAlpha, "format"_a)
			.def("bpp",               &bpp,               "format"_a)
			.def("containerFormat",   &containerFormat,   "format"_a)
			.def("large",             &large,             "format"_a)
			.def("decimal",           &decimal,           "format"_a)
			.def("compressed",        &compressed,        "format"_a)
			.def("transparent",       &transparent,       "format"_a)
			.def("opaque",            &opaque,            "format"_a)
			.def("console",           &console,           "format"_a);

		ImageFormatDetails
			.def("get_data_length", py::overload_cast<ImageFormat, uint16_t, uint16_t, uint16_t>(&getDataLength), "format"_a, "width"_a, "height"_a, "depth"_a = 1)
			.def("get_data_length_extended", py::overload_cast<ImageFormat, uint8_t, uint16_t, uint8_t, uint16_t, uint16_t, uint16_t>(&getDataLength), "format"_a, "mip_count"_a, "frame_count"_a, "face_count"_a, "width"_a, "height"_a, "depth"_a = 1)
			.def("get_data_length_xbox", &getDataLengthXBOX, "padded"_a, "format"_a, "mip_count"_a, "frame_count"_a, "face_count"_a, "width"_a, "height"_a, "depth"_a = 1);

		ImageFormatDetails.def("get_data_position", [](ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t depth = 1) -> std::pair<uint32_t, uint32_t> {
			uint32_t offset, length;
			if (getDataPosition(offset, length, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth)) {
				return {offset, length};
			}
			return {0, 0};
		}, "format"_a, "mip"_a, "mip_count"_a, "frame"_a, "frame_count"_a, "face"_a, "face_count"_a, "width"_a, "height"_a, "slice"_a = 0, "depth"_a = 1);

		ImageFormatDetails.def("get_data_position_xbox", [](bool padded, ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t depth = 1) -> std::pair<uint32_t, uint32_t> {
			uint32_t offset, length;
			if (getDataPositionXbox(offset, length, padded, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth)) {
					return {offset, length};
				}
				return {0, 0};
		}, "padded"_a, "format"_a, "mip"_a, "mip_count"_a, "frame"_a, "frame_count"_a, "face"_a, "face_count"_a, "width"_a, "height"_a, "slice"_a = 0, "depth"_a = 1);
	}

	{
		using namespace ImageDimensions;
		auto ImageDimensions = vtfpp.def_submodule("ImageDimensions");

		ImageDimensions
			.def("get_mip_dim", &getMipDim, "mip"_a, "dim"_a)
			.def("get_recommended_mip_count_for_dims", &getRecommendedMipCountForDims, "format"_a, "width"_a, "height"_a)
			.def("get_actual_mip_count_for_dims_on_console", &getActualMipCountForDimsOnConsole, "width"_a, "height"_a, "depth"_a = 1);
	}

	// Skip ImagePixel, difficult and pointless to bind

	{
		using namespace ImageConversion;
		auto ImageConversion = vtfpp.def_submodule("ImageConversion");

		ImageConversion.attr("DEFAULT_COMPRESSED_QUALITY") = DEFAULT_COMPRESSED_QUALITY;

		ImageConversion.def("convert_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height, float quality = DEFAULT_COMPRESSED_QUALITY) {
			const auto d = convertImageDataToFormat({static_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, width, height, quality);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "old_format"_a, "new_format"_a, "width"_a, "height"_a, "quality"_a = DEFAULT_COMPRESSED_QUALITY);

		ImageConversion.def("convert_several_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t depth, float quality = DEFAULT_COMPRESSED_QUALITY) {
			const auto d = convertSeveralImageDataToFormat({static_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, mipCount, frameCount, faceCount, width, height, depth, quality);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "old_format"_a, "new_format"_a, "mip_count"_a, "frame_count"_a, "face_count"_a, "width"_a, "height"_a, "depth"_a, "quality"_a = DEFAULT_COMPRESSED_QUALITY);

		ImageConversion.def("convert_hdri_to_cubemap", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution = 0, bool bilinear = true) -> std::tuple<py::bytes, py::bytes, py::bytes, py::bytes, py::bytes, py::bytes> {
			const auto ds = convertHDRIToCubeMap({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, resolution, bilinear);
			return {py::bytes{ds[0].data(), ds[0].size()}, py::bytes{ds[1].data(), ds[1].size()}, py::bytes{ds[2].data(), ds[2].size()}, py::bytes{ds[3].data(), ds[3].size()}, py::bytes{ds[4].data(), ds[4].size()}, py::bytes{ds[5].data(), ds[5].size()}};
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "resolution"_a = 0, "bilinear"_a = true);

		py::enum_<FileFormat>(ImageConversion, "FileFormat")
			.value("DEFAULT", FileFormat::DEFAULT)
			.value("PNG",     FileFormat::PNG)
			.value("JPG",     FileFormat::JPG)
			.value("BMP",     FileFormat::BMP)
			.value("TGA",     FileFormat::TGA)
			.value("WEBP",    FileFormat::WEBP)
			.value("QOI",     FileFormat::QOI)
			.value("HDR",     FileFormat::HDR)
			.value("EXR",     FileFormat::EXR);

		ImageConversion.def("get_default_file_format_for_image_format", &getDefaultFileFormatForImageFormat, "format"_a);

		ImageConversion.def("convert_image_data_to_file", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat = FileFormat::DEFAULT) {
			const auto d = convertImageDataToFile({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "file_format"_a = FileFormat::DEFAULT);

		ImageConversion.def("convert_file_to_image_data", [](const py::bytes& fileData) -> std::tuple<py::bytes, ImageFormat, int, int, int> {
			ImageFormat format;
			int width, height, frame;
			const auto d = convertFileToImageData({static_cast<const std::byte*>(fileData.data()), fileData.size()}, format, width, height, frame);
			return {py::bytes{d.data(), d.size()}, format, width, height, frame};
		}, "file_data"_a);

		py::enum_<ResizeEdge>(ImageConversion, "ResizeEdge")
			.value("CLAMP",   ResizeEdge::CLAMP)
			.value("REFLECT", ResizeEdge::REFLECT)
			.value("WRAP",    ResizeEdge::WRAP)
			.value("ZERO",    ResizeEdge::ZERO);

		py::enum_<ResizeFilter>(ImageConversion, "ResizeFilter")
			.value("DEFAULT",       ResizeFilter::DEFAULT)
			.value("BOX",           ResizeFilter::BOX)
			.value("BILINEAR",      ResizeFilter::BILINEAR)
			.value("CUBIC_BSPLINE", ResizeFilter::CUBIC_BSPLINE)
			.value("CATMULL_ROM",   ResizeFilter::CATMULL_ROM)
			.value("MITCHELL",      ResizeFilter::MITCHELL)
			.value("POINT_SAMPLE",  ResizeFilter::POINT_SAMPLE)
			.value("KAISER",        ResizeFilter::KAISER)
			.value("NICE",          ResizeFilter::NICE);

		py::enum_<ResizeMethod>(ImageConversion, "ResizeMethod")
			.value("NONE",                 ResizeMethod::NONE)
			.value("POWER_OF_TWO_BIGGER",  ResizeMethod::POWER_OF_TWO_BIGGER)
			.value("POWER_OF_TWO_SMALLER", ResizeMethod::POWER_OF_TWO_SMALLER)
			.value("POWER_OF_TWO_NEAREST", ResizeMethod::POWER_OF_TWO_NEAREST);

		ImageConversion.def("get_resized_dim", &getResizedDim, "n"_a, "resize_method"_a);
		ImageConversion.def("get_resized_dims", [](uint16_t width, ResizeMethod widthResize, uint16_t height, ResizeMethod heightResize) -> std::pair<uint16_t, uint16_t> {
			setResizedDims(width, widthResize, height, heightResize);
			return {width, height};
		}, "width"_a, "resize_width"_a, "height"_a, "resize_height"_a);

		ImageConversion.def("resize_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP) {
			const auto d = resizeImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, height, newHeight, srgb, filter, edge);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "format"_a, "width"_a, "new_width"_a, "height"_a, "new_height"_a, "srgb"_a, "filter"_a, "edge"_a = ResizeEdge::CLAMP);

		ImageConversion.def("resize_image_data_strict", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP) -> std::tuple<py::bytes, int, int> {
			uint16_t widthOut, heightOut;
			const auto d = resizeImageDataStrict({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, widthOut, widthResize, height, newHeight, heightOut, heightResize, srgb, filter, edge);
			return {py::bytes{d.data(), d.size()}, widthOut, heightOut};
		}, "image_data"_a, "format"_a, "width"_a, "new_width"_a, "width_resize"_a, "height"_a, "new_height"_a, "height_resize"_a, "srgb"_a, "filter"_a, "edge"_a = ResizeEdge::CLAMP);

		ImageConversion.def("crop_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset) {
			const auto d = cropImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, xOffset, height, newHeight, yOffset);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "format"_a, "width"_a, "new_width"_a, "x_offset"_a, "height"_a, "new_height"_a, "y_offset"_a);

		ImageConversion.def("gamma_correct_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, float gamma) {
			const auto d = gammaCorrectImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, gamma);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "gamma"_a);

		ImageConversion.def("invert_green_channel_for_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height) {
			const auto d = invertGreenChannelForImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height);
			return py::bytes{d.data(), d.size()};
		}, "image_data"_a, "format"_a, "width"_a, "height"_a);

		// Skip extractChannelFromImageData, difficult to bind
	}

	{
		using namespace ImageQuantize;
		auto ImageQuantize = vtfpp.def_submodule("ImageQuantize");

		ImageQuantize.def("convert_p8_image_data_to_bgra8888", [](const py::bytes& paletteData, const py::bytes& imageData) {
			const auto d = convertP8ImageDataToBGRA8888({static_cast<const std::byte*>(paletteData.data()), paletteData.size()}, {static_cast<const std::byte*>(imageData.data()), imageData.size()});
			return py::bytes{d.data(), d.size()};
		}, "palette_data"_a, "image_data"_a);
	}

	auto cPPL = py::class_<PPL>(vtfpp, "PPL");

	py::class_<PPL::Image>(cPPL, "Image")
		.def_ro("width",  &PPL::Image::width)
		.def_ro("height", &PPL::Image::height)
		.def_prop_ro("data", [](const PPL::Image& self) {
			return py::bytes{self.data.data(), self.data.size()};
		});

	cPPL
		.def(py::init<uint32_t, ImageFormat, uint32_t>(), "model_checksum"_a, "format"_a = ImageFormat::RGB888, "version"_a = 0)
		.def("__init__", [](PPL* self, const py::bytes& pplData) {
			return new(self) PPL{{static_cast<const std::byte*>(pplData.data()), pplData.size()}};
		}, "ppl_data"_a)
		.def(py::init<const std::string&>(), "path"_a)
		.def("__bool__", &PPL::operator bool, py::is_operator())
		.def_prop_rw("version", &PPL::getVersion, &PPL::setVersion)
		.def_prop_rw("model_checksum", &PPL::getModelChecksum, &PPL::setModelChecksum)
		.def_prop_ro("format", &PPL::getFormat)
		.def("set_format", &PPL::setFormat, "new_format"_a, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("has_image_for_lod", &PPL::hasImageForLOD, "lod"_a)
		.def_prop_ro("image_lods", &PPL::getImageLODs)
		.def("get_image_raw", [](const PPL& self, uint32_t lod = 0) -> std::optional<PPL::Image> {
			const auto* image = self.getImageRaw(lod);
			if (!image) {
				return std::nullopt;
			}
			return *image;
		}, "lod"_a)
		.def("get_image_as", &PPL::getImageAs, "new_format"_a, "lod"_a)
		.def("get_image_as_rgb888", &PPL::getImageAsRGB888, "lod"_a)
		.def("set_image", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t lod = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, lod, quality);
		}, "imageData"_a, "format"_a, "width"_a, "height"_a, "lod"_a = 0, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_resized", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, resizedWidth, resizedHeight, lod, filter, quality);
		}, "imageData"_a, "format"_a, "width"_a, "height"_a, "resized_width"_a, "resized_height"_a, "lod"_a = 0, "filter"_a = ImageConversion::ResizeFilter::DEFAULT, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_from_file", py::overload_cast<const std::string&, uint32_t, float>(&PPL::setImage), "image_path"_a, "lod"_a = 0, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_resized_from_file", py::overload_cast<const std::string&, uint32_t, uint32_t, uint32_t, ImageConversion::ResizeFilter, float>(&PPL::setImage), "image_path"_a, "resized_width"_a, "resized_height"_a, "lod"_a = 0, "filter"_a = ImageConversion::ResizeFilter::DEFAULT, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("save_image", [](const PPL& self, uint32_t lod = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveImageToFile(lod, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, "lod"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("save_image_to_file", py::overload_cast<const std::string&, uint32_t, ImageConversion::FileFormat>(&PPL::saveImageToFile, py::const_), "image_path"_a, "lod"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("bake", [](PPL& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&PPL::bake), "ppl_path"_a);

	auto cSHT = py::class_<SHT>(vtfpp, "SHT");
	auto cSHTSequence = py::class_<SHT::Sequence>(cSHT, "Sequence");
	auto cSHTSequenceFrame = py::class_<SHT::Sequence::Frame>(cSHTSequence, "Frame");

	py::class_<SHT::Sequence::Frame::Bounds>(cSHTSequenceFrame, "Bounds")
		.def_rw("x1", &SHT::Sequence::Frame::Bounds::x1)
		.def_rw("y1", &SHT::Sequence::Frame::Bounds::y1)
		.def_rw("x2", &SHT::Sequence::Frame::Bounds::x2)
		.def_rw("y2", &SHT::Sequence::Frame::Bounds::y2);

	cSHTSequenceFrame
		.def_rw("duration", &SHT::Sequence::Frame::duration)
		.def_rw("bounds",   &SHT::Sequence::Frame::bounds)
		.def("set_all_bounds", &SHT::Sequence::Frame::setAllBounds, "newBounds"_a);

	cSHTSequence
		.def_rw("id",             &SHT::Sequence::id)
		.def_rw("loop",           &SHT::Sequence::loop)
		.def_rw("frames",         &SHT::Sequence::frames)
		.def_rw("duration_total", &SHT::Sequence::durationTotal);

	cSHT
		.def(py::init<>())
		.def("__init__", [](SHT* self, const py::bytes& shtData) {
			return new(self) SHT{{static_cast<const std::byte*>(shtData.data()), shtData.size()}};
		}, "sht_data"_a)
		.def(py::init<const std::string&>(), "sht_path"_a)
		.def("__bool__", &SHT::operator bool, py::is_operator())
		.def_prop_rw("version", &SHT::getVersion, &SHT::setVersion)
		.def_prop_rw("sequences", [](const SHT& self) -> std::vector<SHT::Sequence> { return self.getSequences(); }, [](SHT& self, const std::vector<SHT::Sequence>& sequences) { self.getSequences() = sequences; })
		.def("get_sequence_from_id", [](const SHT& self, uint32_t id) -> const SHT::Sequence* { return self.getSequenceFromID(id); }, "id"_a, py::rv_policy::reference_internal)
		.def("get_frame_bounds_count", &SHT::getFrameBoundsCount)
		.def("bake", [](const SHT& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&SHT::bake, py::const_), "sht_path"_a);

	vtfpp.attr("TTH_SIGNATURE") = TTH_SIGNATURE;

	auto cVTF = py::class_<VTF>(vtfpp, "VTF");

	py::class_<TTX>(vtfpp, "TTX")
		.def(py::init<VTF&&>(), "vtf"_a)
		.def("__init__", [](TTX* self, const py::bytes& tthData, const py::bytes& ttzData) {
			return new(self) TTX{{static_cast<const std::byte*>(tthData.data()), tthData.size()}, {static_cast<const std::byte*>(ttzData.data()), ttzData.size()}};
		}, "tth_data"_a, "ttz_data"_a)
		.def(py::init<const std::string&, const std::string&>(), "tth_path"_a, "ttz_path"_a)
		.def("__bool__", &TTX::operator bool, py::is_operator())
		.def_prop_rw("version_major", &TTX::getMajorVersion, &TTX::setMajorVersion)
		.def_prop_rw("version_minor", &TTX::getMinorVersion, &TTX::setMinorVersion)
		.def_prop_rw("aspect_ratio_type", &TTX::getAspectRatioType, &TTX::setAspectRatioType)
		.def_prop_rw("mip_flags", py::overload_cast<>(&TTX::getMipFlags, py::const_), [](TTX& self, const std::vector<uint64_t>& mipFlags) {
			self.getMipFlags() = mipFlags;
		}, py::rv_policy::reference_internal)
		.def_prop_rw("vtf", py::overload_cast<>(&TTX::getVTF, py::const_), [](TTX& self, const VTF& vtf) {
			self.getVTF() = vtf;
		}, py::rv_policy::reference_internal)
		.def_prop_rw("compression_level", &TTX::getCompressionLevel, &TTX::setCompressionLevel)
		.def("bake", [](const TTX& self) -> std::pair<py::bytes, py::bytes> {
			const auto [d1, d2] = self.bake();
			return {py::bytes{d1.data(), d1.size()}, py::bytes{d2.data(), d2.size()}};
		})
		.def("bake_to_file", py::overload_cast<const std::string&, const std::string&>(&TTX::bake, py::const_), "tth_path"_a, "ttz_path"_a);

	vtfpp.attr("VTF_SIGNATURE") = VTF_SIGNATURE;
	vtfpp.attr("XTF_SIGNATURE") = XTF_SIGNATURE;
	vtfpp.attr("VTFX_SIGNATURE") = VTFX_SIGNATURE;
	vtfpp.attr("VTF3_SIGNATURE") = VTF3_SIGNATURE;

	py::enum_<CompressionMethod>(vtfpp, "CompressionMethod", py::is_arithmetic())
		.value("DEFLATE",      CompressionMethod::DEFLATE)
		.value("ZSTD",         CompressionMethod::ZSTD)
		.value("CONSOLE_LZMA", CompressionMethod::CONSOLE_LZMA);

	auto cResource = py::class_<Resource>(vtfpp, "Resource");

	py::enum_<Resource::Type>(cResource, "Type")
		.value("UNKNOWN",             Resource::TYPE_UNKNOWN)
		.value("THUMBNAIL_DATA",      Resource::TYPE_THUMBNAIL_DATA)
		.value("PALETTE_DATA",        Resource::TYPE_PALETTE_DATA)
		.value("FALLBACK_DATA",       Resource::TYPE_FALLBACK_DATA)
		.value("PARTICLE_SHEET_DATA", Resource::TYPE_PARTICLE_SHEET_DATA)
		.value("HOTSPOT_DATA",        Resource::TYPE_HOTSPOT_DATA)
		.value("IMAGE_DATA",          Resource::TYPE_IMAGE_DATA)
		.value("EXTENDED_FLAGS",      Resource::TYPE_EXTENDED_FLAGS)
		.value("CRC",                 Resource::TYPE_CRC)
		.value("AUX_COMPRESSION",     Resource::TYPE_AUX_COMPRESSION)
		.value("LOD_CONTROL_INFO",    Resource::TYPE_LOD_CONTROL_INFO)
		.value("KEYVALUES_DATA",      Resource::TYPE_KEYVALUES_DATA);

	py::enum_<Resource::Flags>(cResource, "Flags", py::is_flag())
		.value("NONE",       Resource::FLAG_NONE)
		.value("LOCAL_DATA", Resource::FLAG_LOCAL_DATA);

	cResource
		.def_static("get_order", [] { return Resource::getOrder(); })
		.def_ro("type",  &Resource::type)
		.def_ro("flags", &Resource::flags)
		.def("get_data_as_palette",                &Resource::getDataAsPalette, "frame"_a = 0)
		.def("get_data_as_particle_sheet",         &Resource::getDataAsParticleSheet)
		.def("get_data_as_crc",                    &Resource::getDataAsCRC)
		.def("get_data_as_extended_flags",         &Resource::getDataAsExtendedFlags)
		.def("get_data_as_lod_control_info",       &Resource::getDataAsLODControlInfo)
		.def("get_data_as_keyvalues_data",         &Resource::getDataAsKeyValuesData)
		.def("get_data_as_hotspot_data",           &Resource::getDataAsHotspotData)
		.def("get_data_as_aux_compression_level",  &Resource::getDataAsAuxCompressionLevel)
		.def("get_data_as_aux_compression_method", &Resource::getDataAsAuxCompressionMethod)
		.def("get_data_as_aux_compression_length", &Resource::getDataAsAuxCompressionLength, "mip"_a, "mip_count"_a, "frame"_a, "frame_count"_a, "face"_a, "face_count"_a);

	py::enum_<VTF::Flags>(cVTF, "Flags", py::is_flag())
		.value("V0_POINT_SAMPLE",                                 VTF::FLAG_V0_POINT_SAMPLE)
		.value("V0_TRILINEAR",                                    VTF::FLAG_V0_TRILINEAR)
		.value("V0_CLAMP_S",                                      VTF::FLAG_V0_CLAMP_S)
		.value("V0_CLAMP_T",                                      VTF::FLAG_V0_CLAMP_T)
		.value("V0_ANISOTROPIC",                                  VTF::FLAG_V0_ANISOTROPIC)
		.value("V0_VTEX_HINT_DXT5",                               VTF::FLAG_V0_VTEX_HINT_DXT5)
		.value("V0_VTEX_NO_COMPRESS",                             VTF::FLAG_V0_VTEX_NO_COMPRESS)
		.value("V0_NORMAL",                                       VTF::FLAG_V0_NORMAL)
		.value("V0_NO_MIP",                                       VTF::FLAG_V0_NO_MIP)
		.value("V0_NO_LOD",                                       VTF::FLAG_V0_NO_LOD)
		.value("V0_MIN_MIP",                                      VTF::FLAG_V0_MIN_MIP)
		.value("V0_PROCEDURAL",                                   VTF::FLAG_V0_PROCEDURAL)
		.value("V0_ONE_BIT_ALPHA",                                VTF::FLAG_V0_ONE_BIT_ALPHA)
		.value("V0_MULTI_BIT_ALPHA",                              VTF::FLAG_V0_MULTI_BIT_ALPHA)
		.value("V0_ENVMAP",                                       VTF::FLAG_V0_ENVMAP)
		.value("MASK_V0",                                         VTF::FLAG_MASK_V0)
		.value("MASK_V0_VTEX",                                    VTF::FLAG_MASK_V0_VTEX)
		.value("V1_RENDERTARGET",                                 VTF::FLAG_V1_RENDERTARGET)
		.value("V1_DEPTH_RENDERTARGET",                           VTF::FLAG_V1_DEPTH_RENDERTARGET)
		.value("V1_NO_DEBUG_OVERRIDE",                            VTF::FLAG_V1_NO_DEBUG_OVERRIDE)
		.value("V1_SINGLE_COPY",                                  VTF::FLAG_V1_SINGLE_COPY)
		.value("V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA",             VTF::FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA)
		.value("V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL", VTF::FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL)
		.value("V1_VTEX_CONVERT_NORMAL_TO_DUDV",                  VTF::FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV)
		.value("MASK_V1",                                         VTF::FLAG_MASK_V1)
		.value("MASK_V1_VTEX",                                    VTF::FLAG_MASK_V1_VTEX)
		.value("V2_VTEX_ALPHA_TEST_MIP_GENERATION",               VTF::FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION)
		.value("V2_NO_DEPTH_BUFFER",                              VTF::FLAG_V2_NO_DEPTH_BUFFER)
		.value("V2_VTEX_NICE_FILTERED",                           VTF::FLAG_V2_VTEX_NICE_FILTERED)
		.value("V2_CLAMP_U",                                      VTF::FLAG_V2_CLAMP_U)
		.value("MASK_V2",                                         VTF::FLAG_MASK_V2)
		.value("MASK_V2_VTEX",                                    VTF::FLAG_MASK_V2_VTEX)
		.value("XBOX_VTEX_PRESWIZZLED",                           VTF::FLAG_XBOX_VTEX_PRESWIZZLED)
		.value("XBOX_CACHEABLE",                                  VTF::FLAG_XBOX_CACHEABLE)
		.value("XBOX_UNFILTERABLE_OK",                            VTF::FLAG_XBOX_UNFILTERABLE_OK)
		.value("MASK_XBOX",                                       VTF::FLAG_MASK_XBOX)
		.value("MASK_XBOX_VTEX",                                  VTF::FLAG_MASK_XBOX_VTEX)
		.value("V3_ALL_MIPS",                                     VTF::FLAG_V3_ALL_MIPS)
		.value("V3_VERTEX_TEXTURE",                               VTF::FLAG_V3_VERTEX_TEXTURE)
		.value("V3_SSBUMP",                                       VTF::FLAG_V3_SSBUMP)
		.value("V3_BORDER",                                       VTF::FLAG_V3_BORDER)
		.value("MASK_V3",                                         VTF::FLAG_MASK_V3)
		.value("V4_SRGB",                                         VTF::FLAG_V4_SRGB)
		.value("MASK_V4",                                         VTF::FLAG_MASK_V4)
		.value("V4_TF2_STAGING_MEMORY",                           VTF::FLAG_V4_TF2_STAGING_MEMORY)
		.value("V4_TF2_IMMEDIATE_CLEANUP",                        VTF::FLAG_V4_TF2_IMMEDIATE_CLEANUP)
		.value("V4_TF2_IGNORE_PICMIP",                            VTF::FLAG_V4_TF2_IGNORE_PICMIP)
		.value("V4_TF2_STREAMABLE_COARSE",                        VTF::FLAG_V4_TF2_STREAMABLE_COARSE)
		.value("V4_TF2_STREAMABLE_FINE",                          VTF::FLAG_V4_TF2_STREAMABLE_FINE)
		.value("MASK_V4_TF2",                                     VTF::FLAG_MASK_V4_TF2)
		.value("V5_PWL_CORRECTED",                                VTF::FLAG_V5_PWL_CORRECTED)
		.value("V5_SRGB",                                         VTF::FLAG_V5_SRGB)
		.value("V5_DEFAULT_POOL",                                 VTF::FLAG_V5_DEFAULT_POOL)
		.value("V5_LOAD_MOST_MIPS",                               VTF::FLAG_V5_LOAD_MOST_MIPS)
		.value("MASK_V5",                                         VTF::FLAG_MASK_V5)
		.value("V5_CSGO_COMBINED",                                VTF::FLAG_V5_CSGO_COMBINED)
		.value("V5_CSGO_ASYNC_DOWNLOAD",                          VTF::FLAG_V5_CSGO_ASYNC_DOWNLOAD)
		.value("V5_CSGO_SKIP_INITIAL_DOWNLOAD",                   VTF::FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD)
		.value("V5_CSGO_YCOCG",                                   VTF::FLAG_V5_CSGO_YCOCG)
		.value("V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES",              VTF::FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES)
		.value("MASK_V5_CSGO",                                    VTF::FLAG_MASK_V5_CSGO)
		.value("MASK_INTERNAL",                                   VTF::FLAG_MASK_INTERNAL);

	py::enum_<VTF::Platform>(cVTF, "Platform")
		.value("UNKNOWN",       VTF::PLATFORM_UNKNOWN)
		.value("PC",            VTF::PLATFORM_PC)
		.value("XBOX",          VTF::PLATFORM_XBOX)
		.value("X360",          VTF::PLATFORM_X360)
		.value("PS3_ORANGEBOX", VTF::PLATFORM_PS3_ORANGEBOX)
		.value("PS3_PORTAL2",   VTF::PLATFORM_PS3_PORTAL2);

	py::class_<VTF::CreationOptions>(cVTF, "CreationOptions")
		.def(py::init<>())
		.def_rw("version",                    &VTF::CreationOptions::version)
		.def_rw("output_format",              &VTF::CreationOptions::outputFormat)
		.def_rw("width_resize_method",        &VTF::CreationOptions::widthResizeMethod)
		.def_rw("height_resize_method",       &VTF::CreationOptions::heightResizeMethod)
		.def_rw("filter",                     &VTF::CreationOptions::filter)
		.def_rw("flags",                      &VTF::CreationOptions::flags)
		.def_rw("initial_frame_count",        &VTF::CreationOptions::initialFrameCount)
		.def_rw("start_frame",                &VTF::CreationOptions::startFrame)
		.def_rw("is_cubemap",                 &VTF::CreationOptions::isCubeMap)
		.def_rw("initial_depth",              &VTF::CreationOptions::initialDepth)
		.def_rw("compute_transparency_flags", &VTF::CreationOptions::computeTransparencyFlags)
		.def_rw("compute_mips",               &VTF::CreationOptions::computeMips)
		.def_rw("compute_thumbnail",          &VTF::CreationOptions::computeThumbnail)
		.def_rw("compute_reflectivity",       &VTF::CreationOptions::computeReflectivity)
		.def_rw("compression_level",          &VTF::CreationOptions::compressionLevel)
		.def_rw("compression_method",         &VTF::CreationOptions::compressionMethod)
		.def_rw("bumpmap_scale",              &VTF::CreationOptions::bumpMapScale)
		.def_rw("gamma_correction",           &VTF::CreationOptions::gammaCorrection)
		.def_rw("invert_green_channel",       &VTF::CreationOptions::invertGreenChannel)
		.def_rw("xbox_mip_scale",             &VTF::CreationOptions::xboxMipScale);

	cVTF
		.def_ro_static("FORMAT_UNCHANGED",     &VTF::FORMAT_UNCHANGED)
		.def_ro_static("FORMAT_DEFAULT",       &VTF::FORMAT_DEFAULT)
		.def(py::init<>())
		.def("__init__", [](VTF* self, const py::bytes& vtfData, bool parseHeaderOnly = false) {
			return new(self) VTF{std::span{static_cast<const std::byte*>(vtfData.data()), vtfData.size()}, parseHeaderOnly};
		}, "vtf_data"_a, "parse_header_only"_a = false)
		.def(py::init<const std::string&, bool>(), "vtf_path"_a, "parse_header_only"_a = false)
		.def("__bool__", &VTF::operator bool, py::is_operator())
		.def_static("create_and_bake", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const VTF::CreationOptions& options) {
			VTF::create({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, vtfPath, options);
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "vtf_path"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_static("create_blank_and_bake", py::overload_cast<ImageFormat, uint16_t, uint16_t, const std::string&, const VTF::CreationOptions&>(&VTF::create), "format"_a, "width"_a, "height"_a, "vtf_path"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_static("create", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, const VTF::CreationOptions& options) {
			return VTF::create({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, options);
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_static("create_blank", py::overload_cast<ImageFormat, uint16_t, uint16_t, const VTF::CreationOptions&>(&VTF::create), "format"_a, "width"_a, "height"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_static("create_from_file_and_bake", py::overload_cast<const std::string&, const std::string&, const VTF::CreationOptions&>(&VTF::create), "image_path"_a, "vtf_path"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_static("create_from_file", py::overload_cast<const std::string&, const VTF::CreationOptions&>(&VTF::create), "image_path"_a, "creation_options"_a = VTF::CreationOptions{})
		.def_prop_rw("platform", &VTF::getPlatform, &VTF::setPlatform)
		.def_prop_rw("version", &VTF::getVersion, &VTF::setVersion)
		.def_prop_rw("image_width_resize_method", &VTF::getImageWidthResizeMethod, &VTF::setImageWidthResizeMethod)
		.def_prop_rw("image_height_resize_method", &VTF::getImageHeightResizeMethod, &VTF::setImageHeightResizeMethod)
		.def_prop_ro("width", [](const VTF& self) { return self.getWidth(); })
		.def("width_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getWidth(mip); }, "mip"_a = 0)
		.def_prop_ro("height", [](const VTF& self) { return self.getHeight(); })
		.def("height_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getHeight(mip); }, "mip"_a = 0)
		.def("set_size", &VTF::setSize, "width"_a, "height"_a, "filter"_a)
		.def_prop_rw("flags", &VTF::getFlags, &VTF::setFlags)
		.def("add_flags", &VTF::addFlags, "flags"_a)
		.def("remove_flags", &VTF::removeFlags, "flags"_a)
		.def("is_srgb", &VTF::isSRGB)
		.def("set_srgb", &VTF::setSRGB, "srgb"_a)
		.def("compute_transparency_flags", &VTF::computeTransparencyFlags)
		.def_static("get_default_compressed_format", &VTF::getDefaultCompressedFormat, "input_format"_a, "version"_a, "is_cubemap"_a)
		.def_prop_ro("format", &VTF::getFormat)
		.def("set_format", &VTF::setFormat, "new_format"_a, "filter"_a = ImageConversion::ResizeFilter::DEFAULT, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def_prop_rw("mip_count", &VTF::getMipCount, &VTF::setMipCount)
		.def("set_recommended_mip_count", &VTF::setRecommendedMipCount)
		.def("compute_mips", &VTF::computeMips, "filter"_a = ImageConversion::ResizeFilter::DEFAULT)
		.def_prop_rw("frame_count", &VTF::getFrameCount, &VTF::setFrameCount)
		.def_prop_ro("face_count", &VTF::getFaceCount)
		.def("set_face_count", &VTF::setFaceCount, "is_cubemap"_a)
		.def_prop_rw("depth", &VTF::getDepth, &VTF::setDepth)
		.def("set_frame_face_and_depth", &VTF::setFrameFaceAndDepth, "new_frame_count"_a, "is_cubemap"_a, "new_depth"_a = 1)
		.def_prop_rw("start_frame", &VTF::getStartFrame, &VTF::setStartFrame)
		.def_prop_rw("reflectivity", &VTF::getReflectivity, &VTF::setReflectivity)
		.def("compute_reflectivity", &VTF::computeReflectivity)
		.def_prop_rw("bumpmap_scale", &VTF::getBumpMapScale, &VTF::setBumpMapScale)
		.def_prop_ro("thumbnail_format", &VTF::getThumbnailFormat)
		.def_prop_ro("thumbnail_width", &VTF::getThumbnailWidth)
		.def_prop_ro("thumbnail_height", &VTF::getThumbnailHeight)
		.def_prop_ro("fallback_width", &VTF::getFallbackWidth)
		.def_prop_ro("fallback_height", &VTF::getFallbackHeight)
		.def_prop_ro("fallback_mip_count", &VTF::getFallbackMipCount)
		// Skipping getResources, don't want to do the same hack as in SHT here, it's way more expensive
		.def("get_resource", &VTF::getResource, "type"_a, py::rv_policy::reference_internal)
		.def("get_palette_resource_frame", [](const VTF& self, uint16_t frame = 0) {
			const auto d = self.getPaletteResourceFrame(frame);
			return py::bytes{d.data(), d.size()};
		}, "type"_a)
		.def("get_particle_sheet_frame_data_raw", [](const VTF& self, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataRaw(spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, "sht_sequence_id"_a, "sht_frame"_a, "sht_bounds"_a = 0, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("get_particle_sheet_frame_data_as", [](const VTF& self, ImageFormat format, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataAs(format, spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, "format"_a, "sht_sequence_id"_a, "sht_frame"_a, "sht_bounds"_a = 0, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("get_particle_sheet_frame_data_as_rgba8888", [](const VTF& self, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataAsRGBA8888(spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, "sht_sequence_id"_a, "sht_frame"_a, "sht_bounds"_a = 0, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("set_particle_sheet_resource", &VTF::setParticleSheetResource, "value"_a)
		.def("remove_particle_sheet_resource", &VTF::removeParticleSheetResource)
		.def("set_crc_resource", &VTF::setCRCResource, "value"_a)
		.def("remove_crc_resource", &VTF::removeCRCResource)
		.def("set_lod_resource", &VTF::setLODResource, "u"_a, "v"_a, "u360"_a = 0, "v360"_a = 0)
		.def("remove_lod_resource", &VTF::removeLODResource)
		.def("set_extended_flags_resource", &VTF::setExtendedFlagsResource, "value"_a)
		.def("remove_extended_flags_resource", &VTF::removeExtendedFlagsResource)
		.def("set_keyvalues_data_resource", &VTF::setKeyValuesDataResource, "value"_a)
		.def("remove_keyvalues_data_resource", &VTF::removeKeyValuesDataResource)
		.def("set_hotspot_data_resource", &VTF::setHotspotDataResource, "value"_a)
		.def("remove_hotspot_data_resource", &VTF::removeHotspotDataResource)
		.def_prop_rw("compression_level", &VTF::getCompressionLevel, &VTF::setCompressionLevel)
		.def_prop_rw("compression_method", &VTF::getCompressionMethod, &VTF::setCompressionMethod)
		.def("has_image_data", &VTF::hasImageData)
		.def("get_image_data_raw", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
			const auto d = self.getImageDataRaw(mip, frame, face, slice);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("get_image_data_as", [](const VTF& self, ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
			const auto d = self.getImageDataAs(newFormat, mip, frame, face, slice);
			return py::bytes{d.data(), d.size()};
		}, "new_format"_a, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("get_image_data_as_rgba8888", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) {
			const auto d = self.getImageDataAsRGBA8888(mip, frame, face, slice);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0)
		.def("set_image", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			return self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, filter, mip, frame, face, slice, quality);
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "filter"_a, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_from_file", py::overload_cast<const std::string&, ImageConversion::ResizeFilter, uint8_t, uint16_t, uint8_t, uint16_t, float>(&VTF::setImage), "image_path"_a, "filter"_a = ImageConversion::ResizeFilter::DEFAULT, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("save_image", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveImageToFile(mip, frame, face, slice, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("save_image_to_file", py::overload_cast<const std::string&, uint8_t, uint16_t, uint8_t, uint16_t, ImageConversion::FileFormat>(&VTF::saveImageToFile, py::const_), "image_path"_a, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "slice"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("has_thumbnail_data", &VTF::hasThumbnailData)
		.def("get_thumbnail_data_raw", [](const VTF& self) {
			const auto d = self.getThumbnailDataRaw();
			return py::bytes{d.data(), d.size()};
		})
		.def("get_thumbnail_data_as", [](const VTF& self, ImageFormat newFormat) {
			const auto d = self.getThumbnailDataAs(newFormat);
			return py::bytes{d.data(), d.size()};
		}, "new_format"_a)
		.def("get_thumbnail_data_as_rgba8888", [](const VTF& self) {
			const auto d = self.getThumbnailDataAsRGBA8888();
			return py::bytes{d.data(), d.size()};
		})
		.def("set_thumbnail", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			return self.setThumbnail({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, quality);
		}, "image_data"_a, "format"_a, "width"_a, "height"_a, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_thumbnail_from_file", py::overload_cast<const std::string&, float>(&VTF::setThumbnail), "image_path"_a, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("compute_thumbnail", &VTF::computeThumbnail, "filter"_a = ImageConversion::ResizeFilter::DEFAULT, "quality"_a = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("remove_thumbnail", &VTF::removeThumbnail)
		.def("save_thumbnail", [](const VTF& self, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveThumbnailToFile(fileFormat);
			return py::bytes{d.data(), d.size()};
		}, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("save_thumbnail_to_file", py::overload_cast<const std::string&, ImageConversion::FileFormat>(&VTF::saveThumbnailToFile, py::const_), "image_path"_a, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("has_fallback_data", &VTF::hasFallbackData)
		.def("get_fallback_data_raw", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataRaw(mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0)
		.def("get_fallback_data_as", [](const VTF& self, ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataAs(newFormat, mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, "new_format"_a, "mip"_a = 0, "frame"_a = 0, "face"_a = 0)
		.def("get_fallback_data_as_rgba8888", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataAsRGBA8888(mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0)
		.def("compute_fallback", &VTF::computeFallback, "filter"_a = ImageConversion::ResizeFilter::DEFAULT)
		.def("remove_fallback", &VTF::removeFallback)
		.def("save_fallback", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveFallbackToFile(mip, frame, face, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def("save_fallback_to_file", py::overload_cast<const std::string&, uint8_t, uint16_t, uint8_t, ImageConversion::FileFormat>(&VTF::saveFallbackToFile, py::const_), "image_path"_a, "mip"_a = 0, "frame"_a = 0, "face"_a = 0, "file_format"_a = ImageConversion::FileFormat::DEFAULT)
		.def_prop_rw("xbox_mip_scale", &VTF::getXBOXMipScale, &VTF::setXBOXMipScale)
		.def("bake", [](const VTF& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&VTF::bake, py::const_), "vtf_path"_a);
}

} // namespace vtfpp
