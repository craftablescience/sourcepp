#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>

namespace py = nanobind;

#include <vtfpp/vtfpp.h>

namespace vtfpp {

inline void register_python(py::module_& m) {
	using namespace vtfpp;
	auto vtfpp = m.def_submodule("vtfpp");

	auto cHOT = py::class_<HOT>(vtfpp, "HOT");
	auto cHOTRect = py::class_<HOT::Rect>(cHOT, "Rect");

	py::enum_<HOT::Rect::Flags>(cHOTRect, "Flags", py::is_flag())
		.value("NONE", HOT::Rect::FLAG_NONE)
		.value("RANDOM_ROTATION", HOT::Rect::FLAG_RANDOM_ROTATION)
		.value("RANDOM_REFLECTION", HOT::Rect::FLAG_RANDOM_REFLECTION)
		.value("IS_ALTERNATE", HOT::Rect::FLAG_IS_ALTERNATE)
		.export_values();

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
		}, py::arg("hot_data"))
		.def(py::init<const std::string&>(), py::arg("hot_path"))
		.def("__bool__", &HOT::operator bool, py::is_operator())
		.def_prop_rw("version", &HOT::getVersion, &HOT::setVersion)
		.def_prop_rw("flags", &HOT::getFlags, &HOT::setFlags)
		.def_prop_rw("rects", [](const HOT& self) -> std::vector<HOT::Rect> { return self.getRects(); }, [](HOT& self, const std::vector<HOT::Rect>& rects) { self.getRects() = rects; })
		.def("bake", [](const HOT& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&HOT::bake, py::const_), py::arg("hot_path"));

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

		ImageFormatDetails
			.def("red",               &red,               py::arg("format"))
			.def("decompressedRed",   &decompressedRed,   py::arg("format"))
			.def("green",             &green,             py::arg("format"))
			.def("decompressedGreen", &decompressedGreen, py::arg("format"))
			.def("blue",              &blue,              py::arg("format"))
			.def("decompressedBlue",  &decompressedBlue,  py::arg("format"))
			.def("alpha",             &alpha,             py::arg("format"))
			.def("decompressedAlpha", &decompressedAlpha, py::arg("format"))
			.def("bpp",               &bpp,               py::arg("format"))
			.def("containerFormat",   &containerFormat,   py::arg("format"))
			.def("large",             &large,             py::arg("format"))
			.def("decimal",           &decimal,           py::arg("format"))
			.def("compressed",        &compressed,        py::arg("format"))
			.def("transparent",       &transparent,       py::arg("format"))
			.def("opaque",            &opaque,            py::arg("format"))
			.def("console",           &console,           py::arg("format"));

		ImageFormatDetails
			.def("get_data_length", py::overload_cast<ImageFormat, uint16_t, uint16_t, uint16_t>(&getDataLength), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("slice_count") = 1)
			.def("get_data_length_extended", py::overload_cast<ImageFormat, uint8_t, uint16_t, uint8_t, uint16_t, uint16_t, uint16_t>(&getDataLength), py::arg("format"), py::arg("mip_count"), py::arg("frame_count"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice_count") = 1)
			.def("get_data_length_xbox", &getDataLengthXBOX, py::arg("padded"), py::arg("format"), py::arg("mip_count"), py::arg("frame_count"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice_count") = 1);

		ImageFormatDetails.def("get_data_position", [](ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) -> std::pair<uint32_t, uint32_t> {
			uint32_t offset, length;
			if (getDataPosition(offset, length, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount)) {
				return {offset, length};
			}
			return {0, 0};
		}, py::arg("format"), py::arg("mip"), py::arg("mip_count"), py::arg("frame"), py::arg("frame_count"), py::arg("face"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice") = 0, py::arg("slice_count") = 1);

		ImageFormatDetails.def("get_data_position_xbox", [](bool padded, ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) -> std::pair<uint32_t, uint32_t> {
			uint32_t offset, length;
			if (getDataPositionXbox(offset, length, padded, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount)) {
					return {offset, length};
				}
				return {0, 0};
		}, py::arg("padded"), py::arg("format"), py::arg("mip"), py::arg("mip_count"), py::arg("frame"), py::arg("frame_count"), py::arg("face"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice") = 0, py::arg("slice_count") = 1);
	}

	{
		using namespace ImageDimensions;
		auto ImageDimensions = vtfpp.def_submodule("ImageDimensions");

		ImageDimensions
			.def("get_mip_dim", &getMipDim, py::arg("mip"), py::arg("dim"))
			.def("get_recommended_mip_count_for_dims", &getRecommendedMipCountForDims, py::arg("format"), py::arg("width"), py::arg("height"));
	}

	// Skip ImagePixel, difficult and pointless to bind

	{
		using namespace ImageConversion;
		auto ImageConversion = vtfpp.def_submodule("ImageConversion");

		ImageConversion.attr("DEFAULT_COMPRESSED_QUALITY") = DEFAULT_COMPRESSED_QUALITY;

		ImageConversion.def("convert_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height, float quality = DEFAULT_COMPRESSED_QUALITY) {
			const auto d = convertImageDataToFormat({static_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, width, height, quality);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("old_format"), py::arg("new_format"), py::arg("width"), py::arg("height"), py::arg("quality") = DEFAULT_COMPRESSED_QUALITY);

		ImageConversion.def("convert_several_image_data_to_format", [](const py::bytes& imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount, float quality = DEFAULT_COMPRESSED_QUALITY) {
			const auto d = convertSeveralImageDataToFormat({static_cast<const std::byte*>(imageData.data()), imageData.size()}, oldFormat, newFormat, mipCount, frameCount, faceCount, width, height, sliceCount, quality);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("old_format"), py::arg("new_format"), py::arg("mip_count"), py::arg("frame_count"), py::arg("face_count"), py::arg("width"), py::arg("height"), py::arg("slice_count"), py::arg("quality") = DEFAULT_COMPRESSED_QUALITY);

		ImageConversion.def("convert_hdri_to_cubemap", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution = 0, bool bilinear = true) -> std::tuple<py::bytes, py::bytes, py::bytes, py::bytes, py::bytes, py::bytes> {
			const auto ds = convertHDRIToCubeMap({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, resolution, bilinear);
			return {py::bytes{ds[0].data(), ds[0].size()}, py::bytes{ds[1].data(), ds[1].size()}, py::bytes{ds[2].data(), ds[2].size()}, py::bytes{ds[3].data(), ds[3].size()}, py::bytes{ds[4].data(), ds[4].size()}, py::bytes{ds[5].data(), ds[5].size()}};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("resolution") = 0, py::arg("bilinear") = true);

		py::enum_<FileFormat>(ImageConversion, "FileFormat")
			.value("DEFAULT", FileFormat::DEFAULT)
			.value("PNG",     FileFormat::PNG)
			.value("JPG",     FileFormat::JPG)
			.value("BMP",     FileFormat::BMP)
			.value("TGA",     FileFormat::TGA)
			.value("WEBP",    FileFormat::WEBP)
			.value("QOI",     FileFormat::QOI)
			.value("HDR",     FileFormat::HDR)
			.value("EXR",     FileFormat::EXR)
			.export_values();

		ImageConversion.def("get_default_file_format_for_image_format", &getDefaultFileFormatForImageFormat, py::arg("format"));

		ImageConversion.def("convert_image_data_to_file", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat = FileFormat::DEFAULT) {
			const auto d = convertImageDataToFile({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("file_format") = FileFormat::DEFAULT);

		ImageConversion.def("convert_file_to_image_data", [](const py::bytes& fileData) -> std::tuple<py::bytes, ImageFormat, int, int, int> {
			ImageFormat format;
			int width, height, frame;
			const auto d = convertFileToImageData({static_cast<const std::byte*>(fileData.data()), fileData.size()}, format, width, height, frame);
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
			.value("CATMULL_ROM",   ResizeFilter::CATMULL_ROM)
			.value("MITCHELL",      ResizeFilter::MITCHELL)
			.value("POINT_SAMPLE",  ResizeFilter::POINT_SAMPLE)
			.value("KAISER",        ResizeFilter::KAISER)
			.value("NICE",          ResizeFilter::NICE)
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
			const auto d = resizeImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, height, newHeight, srgb, filter, edge);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("new_width"), py::arg("height"), py::arg("new_height"), py::arg("srgb"), py::arg("filter"), py::arg("edge") = ResizeEdge::CLAMP);

		ImageConversion.def("resize_image_data_strict", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP) -> std::tuple<py::bytes, int, int> {
			uint16_t widthOut, heightOut;
			const auto d = resizeImageDataStrict({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, widthOut, widthResize, height, newHeight, heightOut, heightResize, srgb, filter, edge);
			return {py::bytes{d.data(), d.size()}, widthOut, heightOut};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("new_width"), py::arg("width_resize"), py::arg("height"), py::arg("new_height"), py::arg("height_resize"), py::arg("srgb"), py::arg("filter"), py::arg("edge") = ResizeEdge::CLAMP);

		ImageConversion.def("crop_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset) {
			const auto d = cropImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, newWidth, xOffset, height, newHeight, yOffset);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("new_width"), py::arg("x_offset"), py::arg("height"), py::arg("new_height"), py::arg("y_offset"));

		ImageConversion.def("gamma_correct_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, float gamma) {
			const auto d = gammaCorrectImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, gamma);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("gamma"));

		ImageConversion.def("invert_green_channel_for_image_data", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height) {
			const auto d = invertGreenChannelForImageData({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height);
			return py::bytes{d.data(), d.size()};
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"));

		// Skip extractChannelFromImageData, difficult to bind
	}

	{
		using namespace ImageQuantize;
		auto ImageQuantize = vtfpp.def_submodule("ImageQuantize");

		ImageQuantize.def("convert_p8_image_data_to_bgra8888", [](const py::bytes& paletteData, const py::bytes& imageData) {
			const auto d = convertP8ImageDataToBGRA8888({static_cast<const std::byte*>(paletteData.data()), paletteData.size()}, {static_cast<const std::byte*>(imageData.data()), imageData.size()});
			return py::bytes{d.data(), d.size()};
		}, py::arg("palette_data"), py::arg("image_data"));
	}

	auto cPPL = py::class_<PPL>(vtfpp, "PPL");

	py::class_<PPL::Image>(cPPL, "Image")
		.def_ro("width",  &PPL::Image::width)
		.def_ro("height", &PPL::Image::height)
		.def_prop_ro("data", [](const PPL::Image& self) {
			return py::bytes{self.data.data(), self.data.size()};
		});

	cPPL
		.def(py::init<uint32_t, ImageFormat, uint32_t>(), py::arg("model_checksum"), py::arg("format") = ImageFormat::RGB888, py::arg("version") = 0)
		.def("__init__", [](PPL* self, const py::bytes& pplData) {
			return new(self) PPL{{static_cast<const std::byte*>(pplData.data()), pplData.size()}};
		}, py::arg("ppl_data"))
		.def(py::init<const std::string&>(), py::arg("path"))
		.def("__bool__", &PPL::operator bool, py::is_operator())
		.def_prop_rw("version", &PPL::getVersion, &PPL::setVersion)
		.def_prop_rw("model_checksum", &PPL::getModelChecksum, &PPL::setModelChecksum)
		.def_prop_ro("format", &PPL::getFormat)
		.def("set_format", &PPL::setFormat, py::arg("new_format"), py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
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
		.def("set_image", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t lod = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, lod, quality);
		}, py::arg("imageData"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("lod") = 0, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_resized", [](PPL& self, const py::bytes& imageData, ImageFormat format, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, resizedWidth, resizedHeight, lod, filter, quality);
		}, py::arg("imageData"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("resized_width"), py::arg("resized_height"), py::arg("lod") = 0, py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_from_file", py::overload_cast<const std::string&, uint32_t, float>(&PPL::setImage), py::arg("image_path"), py::arg("lod") = 0, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_resized_from_file", py::overload_cast<const std::string&, uint32_t, uint32_t, uint32_t, ImageConversion::ResizeFilter, float>(&PPL::setImage), py::arg("image_path"), py::arg("resized_width"), py::arg("resized_height"), py::arg("lod") = 0, py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
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
		.def("set_all_bounds", &SHT::Sequence::Frame::setAllBounds, py::arg("newBounds"));

	cSHTSequence
		.def_rw("id",             &SHT::Sequence::id)
		.def_rw("loop",           &SHT::Sequence::loop)
		.def_rw("frames",         &SHT::Sequence::frames)
		.def_rw("duration_total", &SHT::Sequence::durationTotal);

	cSHT
		.def(py::init<>())
		.def("__init__", [](SHT* self, const py::bytes& shtData) {
			return new(self) SHT{{static_cast<const std::byte*>(shtData.data()), shtData.size()}};
		}, py::arg("sht_data"))
		.def(py::init<const std::string&>(), py::arg("sht_path"))
		.def("__bool__", &SHT::operator bool, py::is_operator())
		.def_prop_rw("version", &SHT::getVersion, &SHT::setVersion)
		.def_prop_rw("sequences", [](const SHT& self) -> std::vector<SHT::Sequence> { return self.getSequences(); }, [](SHT& self, const std::vector<SHT::Sequence>& sequences) { self.getSequences() = sequences; })
		.def("get_sequence_from_id", [](const SHT& self, uint32_t id) -> const SHT::Sequence* { return self.getSequenceFromID(id); }, py::arg("id"), py::rv_policy::reference_internal)
		.def("get_frame_bounds_count", &SHT::getFrameBoundsCount)
		.def("bake", [](const SHT& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&SHT::bake, py::const_), py::arg("sht_path"));

	vtfpp.attr("TTH_SIGNATURE") = TTH_SIGNATURE;

	auto cVTF = py::class_<VTF>(vtfpp, "VTF");

	py::class_<TTX>(vtfpp, "TTX")
		.def(py::init<VTF&&>(), py::arg("vtf"))
		.def("__init__", [](TTX* self, const py::bytes& tthData, const py::bytes& ttzData) {
			return new(self) TTX{{static_cast<const std::byte*>(tthData.data()), tthData.size()}, {static_cast<const std::byte*>(ttzData.data()), ttzData.size()}};
		}, py::arg("tth_data"), py::arg("ttz_data"))
		.def(py::init<const std::string&, const std::string&>(), py::arg("tth_path"), py::arg("ttz_path"))
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
		.def("bake_to_file", py::overload_cast<const std::string&, const std::string&>(&TTX::bake, py::const_), py::arg("tth_path"), py::arg("ttz_path"));

	vtfpp.attr("VTF_SIGNATURE") = VTF_SIGNATURE;
	vtfpp.attr("XTF_SIGNATURE") = XTF_SIGNATURE;
	vtfpp.attr("VTFX_SIGNATURE") = VTFX_SIGNATURE;
	vtfpp.attr("VTF3_SIGNATURE") = VTF3_SIGNATURE;

	py::enum_<CompressionMethod>(vtfpp, "CompressionMethod", py::is_arithmetic())
		.value("DEFLATE",      CompressionMethod::DEFLATE)
		.value("ZSTD",         CompressionMethod::ZSTD)
		.value("CONSOLE_LZMA", CompressionMethod::CONSOLE_LZMA)
		.export_values();

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
		.value("KEYVALUES_DATA",      Resource::TYPE_KEYVALUES_DATA)
		.export_values();

	py::enum_<Resource::Flags>(cResource, "Flags", py::is_flag())
		.value("NONE",       Resource::FLAG_NONE)
		.value("LOCAL_DATA", Resource::FLAG_LOCAL_DATA)
		.export_values();

	cResource
		.def_static("get_order", [] { return Resource::getOrder(); })
		.def_ro("type",  &Resource::type)
		.def_ro("flags", &Resource::flags)
		.def("get_data_as_particle_sheet",         &Resource::getDataAsParticleSheet)
		.def("get_data_as_crc",                    &Resource::getDataAsCRC)
		.def("get_data_as_extended_flags",         &Resource::getDataAsExtendedFlags)
		.def("get_data_as_lod_control_info",       &Resource::getDataAsLODControlInfo)
		.def("get_data_as_keyvalues_data",         &Resource::getDataAsKeyValuesData)
		.def("get_data_as_hotspot_data",           &Resource::getDataAsHotspotData)
		.def("get_data_as_aux_compression_level",  &Resource::getDataAsAuxCompressionLevel)
		.def("get_data_as_aux_compression_method", &Resource::getDataAsAuxCompressionMethod)
		.def("get_data_as_aux_compression_length", &Resource::getDataAsAuxCompressionLength, py::arg("mip"), py::arg("mip_count"), py::arg("frame"), py::arg("frame_count"), py::arg("face"), py::arg("face_count"));

	py::enum_<VTF::FlagsV0>(cVTF, "FlagsV0", py::is_flag())
		.value("POINT_SAMPLE",               VTF::FLAG_POINT_SAMPLE)
		.value("TRILINEAR",                  VTF::FLAG_TRILINEAR)
		.value("CLAMP_S",                    VTF::FLAG_CLAMP_S)
		.value("CLAMP_T",                    VTF::FLAG_CLAMP_T)
		.value("ANISOTROPIC",                VTF::FLAG_ANISOTROPIC)
		.value("HINT_DXT5",                  VTF::FLAG_HINT_DXT5)
		.value("NORMAL",                     VTF::FLAG_NORMAL)
		.value("NO_MIP",                     VTF::FLAG_NO_MIP)
		.value("NO_LOD",                     VTF::FLAG_NO_LOD)
		.value("MIN_MIP",                    VTF::FLAG_MIN_MIP)
		.value("PROCEDURAL",                 VTF::FLAG_PROCEDURAL)
		.value("ONE_BIT_ALPHA",              VTF::FLAG_ONE_BIT_ALPHA)
		.value("MULTI_BIT_ALPHA",            VTF::FLAG_MULTI_BIT_ALPHA)
		.value("ENVMAP",                     VTF::FLAG_ENVMAP)
		.value("RENDERTARGET",               VTF::FLAG_RENDERTARGET)
		.value("DEPTH_RENDERTARGET",         VTF::FLAG_DEPTH_RENDERTARGET)
		.value("NO_DEBUG_OVERRIDE",          VTF::FLAG_NO_DEBUG_OVERRIDE)
		.value("SINGLE_COPY",                VTF::FLAG_SINGLE_COPY)
		.export_values();

	py::enum_<VTF::FlagsV2>(cVTF, "FlagsV2", py::is_flag())
		.value("NO_DEPTH_BUFFER",            VTF::FLAG_V2_NO_DEPTH_BUFFER)
		.value("CLAMP_U",                    VTF::FLAG_V2_CLAMP_U)
		.export_values();

	py::enum_<VTF::FlagsXBOX>(cVTF, "FlagsXBOX", py::is_flag())
		.value("CACHEABLE",                  VTF::FLAG_XBOX_CACHEABLE)
		.value("UNFILTERABLE_OK",            VTF::FLAG_XBOX_UNFILTERABLE_OK)
		.export_values();

	py::enum_<VTF::FlagsV3>(cVTF, "FlagsV3", py::is_flag())
		.value("LOAD_ALL_MIPS",              VTF::FLAG_V3_LOAD_ALL_MIPS)
		.value("VERTEX_TEXTURE",             VTF::FLAG_V3_VERTEX_TEXTURE)
		.value("SSBUMP",                     VTF::FLAG_V3_SSBUMP)
		.value("BORDER",                     VTF::FLAG_V3_BORDER)
		.export_values();

	py::enum_<VTF::FlagsV4>(cVTF, "FlagsV4", py::is_flag())
		.value("SRGB",                       VTF::FLAG_V4_SRGB)
		.export_values();

	py::enum_<VTF::FlagsV4_TF2>(cVTF, "FlagsV4_TF2", py::is_flag())
		.value("STAGING_MEMORY",             VTF::FLAG_V4_TF2_STAGING_MEMORY)
		.value("IMMEDIATE_CLEANUP",          VTF::FLAG_V4_TF2_IMMEDIATE_CLEANUP)
		.value("IGNORE_PICMIP",              VTF::FLAG_V4_TF2_IGNORE_PICMIP)
		.value("STREAMABLE_COARSE",          VTF::FLAG_V4_TF2_STREAMABLE_COARSE)
		.value("STREAMABLE_FINE",            VTF::FLAG_V4_TF2_STREAMABLE_FINE)
		.export_values();

	py::enum_<VTF::FlagsV5>(cVTF, "FlagsV5", py::is_flag())
		.value("PWL_CORRECTED",              VTF::FLAG_V5_PWL_CORRECTED)
		.value("SRGB",                       VTF::FLAG_V5_SRGB)
		.value("DEFAULT_POOL",               VTF::FLAG_V5_DEFAULT_POOL)
		.value("LOAD_MOST_MIPS",             VTF::FLAG_V5_LOAD_MOST_MIPS)
		.export_values();

	py::enum_<VTF::FlagsV5_CSGO>(cVTF, "FlagsV5_CSGO", py::is_flag())
		.value("COMBINED",                   VTF::FLAG_V5_CSGO_COMBINED)
		.value("ASYNC_DOWNLOAD",             VTF::FLAG_V5_CSGO_ASYNC_DOWNLOAD)
		.value("SKIP_INITIAL_DOWNLOAD",      VTF::FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD)
		.value("YCOCG",                      VTF::FLAG_V5_CSGO_YCOCG)
		.value("ASYNC_SKIP_INITIAL_LOW_RES", VTF::FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES)
		.export_values();

	py::enum_<VTF::Platform>(cVTF, "Platform")
		.value("UNKNOWN",       VTF::PLATFORM_UNKNOWN)
		.value("PC",            VTF::PLATFORM_PC)
		.value("XBOX",          VTF::PLATFORM_XBOX)
		.value("X360",          VTF::PLATFORM_X360)
		.value("PS3_ORANGEBOX", VTF::PLATFORM_PS3_ORANGEBOX)
		.value("PS3_PORTAL2",   VTF::PLATFORM_PS3_PORTAL2)
		.export_values();

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
		.def_rw("initial_slice_count",        &VTF::CreationOptions::initialSliceCount)
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
		.def_ro_static("FLAGS_MASK_V0",        &VTF::FLAGS_MASK_V0)
		.def_ro_static("FLAGS_MASK_V2",        &VTF::FLAGS_MASK_V2)
		.def_ro_static("FLAGS_MASK_V3",        &VTF::FLAGS_MASK_V3)
		.def_ro_static("FLAGS_MASK_V4",        &VTF::FLAGS_MASK_V4)
		.def_ro_static("FLAGS_MASK_V4_TF2",    &VTF::FLAGS_MASK_V4_TF2)
		.def_ro_static("FLAGS_MASK_V5",        &VTF::FLAGS_MASK_V5)
		.def_ro_static("FLAGS_MASK_V5_CSGO",   &VTF::FLAGS_MASK_V5_CSGO)
		.def_ro_static("FLAGS_MASK_INTERNAL",  &VTF::FLAGS_MASK_INTERNAL)
		.def_ro_static("FORMAT_UNCHANGED",     &VTF::FORMAT_UNCHANGED)
		.def_ro_static("FORMAT_DEFAULT",       &VTF::FORMAT_DEFAULT)
		.def(py::init<>())
		.def("__init__", [](VTF* self, const py::bytes& vtfData, bool parseHeaderOnly = false) {
			return new(self) VTF{std::span{static_cast<const std::byte*>(vtfData.data()), vtfData.size()}, parseHeaderOnly};
		}, py::arg("vtf_data"), py::arg("parse_header_only") = false)
		.def(py::init<const std::string&, bool>(), py::arg("vtf_path"), py::arg("parse_header_only") = false)
		.def("__bool__", &VTF::operator bool, py::is_operator())
		.def_static("create_and_bake", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const VTF::CreationOptions& options) {
			VTF::create({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, vtfPath, options);
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_static("create_blank_and_bake", py::overload_cast<ImageFormat, uint16_t, uint16_t, const std::string&, const VTF::CreationOptions&>(&VTF::create), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_static("create", [](const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, const VTF::CreationOptions& options) {
			return VTF::create({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, options);
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_static("create_blank", py::overload_cast<ImageFormat, uint16_t, uint16_t, const VTF::CreationOptions&>(&VTF::create), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_static("create_from_file_and_bake", py::overload_cast<const std::string&, const std::string&, const VTF::CreationOptions&>(&VTF::create), py::arg("image_path"), py::arg("vtf_path"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_static("create_from_file", py::overload_cast<const std::string&, const VTF::CreationOptions&>(&VTF::create), py::arg("image_path"), py::arg("creation_options") = VTF::CreationOptions{})
		.def_prop_rw("platform", &VTF::getPlatform, &VTF::setPlatform)
		.def_prop_rw("version", &VTF::getVersion, &VTF::setVersion)
		.def_prop_rw("image_width_resize_method", &VTF::getImageWidthResizeMethod, &VTF::setImageWidthResizeMethod)
		.def_prop_rw("image_height_resize_method", &VTF::getImageHeightResizeMethod, &VTF::setImageHeightResizeMethod)
		.def_prop_ro("width", [](const VTF& self) { return self.getWidth(); })
		.def("width_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getWidth(mip); }, py::arg("mip") = 0)
		.def_prop_ro("height", [](const VTF& self) { return self.getHeight(); })
		.def("height_for_mip", [](const VTF& self, uint8_t mip = 0) { return self.getHeight(mip); }, py::arg("mip") = 0)
		.def("set_size", &VTF::setSize, py::arg("width"), py::arg("height"), py::arg("filter"))
		.def_prop_rw("flags", &VTF::getFlags, &VTF::setFlags)
		.def("add_flags", &VTF::addFlags, py::arg("flags"))
		.def("remove_flags", &VTF::removeFlags, py::arg("flags"))
		.def("is_srgb", &VTF::isSRGB)
		.def("set_srgb", &VTF::setSRGB, py::arg("srgb"))
		.def("compute_transparency_flags", &VTF::computeTransparencyFlags)
		.def_static("get_default_compressed_format", &VTF::getDefaultCompressedFormat, py::arg("input_format"), py::arg("version"), py::arg("is_cubemap"))
		.def_prop_ro("format", &VTF::getFormat)
		.def("set_format", &VTF::setFormat, py::arg("new_format"), py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def_prop_rw("mip_count", &VTF::getMipCount, &VTF::setMipCount)
		.def("set_recommended_mip_count", &VTF::setRecommendedMipCount)
		.def("compute_mips", &VTF::computeMips, py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT)
		.def_prop_rw("frame_count", &VTF::getFrameCount, &VTF::setFrameCount)
		.def_prop_ro("face_count", &VTF::getFaceCount)
		.def("set_face_count", &VTF::setFaceCount, py::arg("is_cubemap"))
		.def_prop_rw("slice_count", &VTF::getSliceCount, &VTF::setSliceCount)
		.def("set_frame_face_and_slice_count", &VTF::setFrameFaceAndSliceCount, py::arg("new_frame_count"), py::arg("is_cubemap"), py::arg("new_slice_count") = 1)
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
		.def("get_resource", &VTF::getResource, py::arg("type"), py::rv_policy::reference_internal)
		.def("get_particle_sheet_frame_data_raw", [](const VTF& self, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataRaw(spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, py::arg("sht_sequence_id"), py::arg("sht_frame"), py::arg("sht_bounds") = 0, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
		.def("get_particle_sheet_frame_data_as", [](const VTF& self, ImageFormat format, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataAs(format, spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, py::arg("format"), py::arg("sht_sequence_id"), py::arg("sht_frame"), py::arg("sht_bounds") = 0, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
		.def("get_particle_sheet_frame_data_as_rgba8888", [](const VTF& self, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) -> std::tuple<uint16_t, uint16_t, py::bytes> {
			uint16_t spriteWidth, spriteHeight;
			const auto d = self.getParticleSheetFrameDataAsRGBA8888(spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
			return {spriteWidth, spriteHeight, py::bytes{d.data(), d.size()}};
		}, py::arg("sht_sequence_id"), py::arg("sht_frame"), py::arg("sht_bounds") = 0, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0)
		.def("set_particle_sheet_resource", &VTF::setParticleSheetResource, py::arg("value"))
		.def("remove_particle_sheet_resource", &VTF::removeParticleSheetResource)
		.def("set_crc_resource", &VTF::setCRCResource, py::arg("value"))
		.def("remove_crc_resource", &VTF::removeCRCResource)
		.def("set_lod_resource", &VTF::setLODResource, py::arg("u"), py::arg("v"), py::arg("u360") = 0, py::arg("v360") = 0)
		.def("remove_lod_resource", &VTF::removeLODResource)
		.def("set_extended_flags_resource", &VTF::setExtendedFlagsResource, py::arg("value"))
		.def("remove_extended_flags_resource", &VTF::removeExtendedFlagsResource)
		.def("set_keyvalues_data_resource", &VTF::setKeyValuesDataResource, py::arg("value"))
		.def("remove_keyvalues_data_resource", &VTF::removeKeyValuesDataResource)
		.def("set_hotspot_data_resource", &VTF::setHotspotDataResource, py::arg("value"))
		.def("remove_hotspot_data_resource", &VTF::removeHotspotDataResource)
		.def_prop_rw("compression_level", &VTF::getCompressionLevel, &VTF::setCompressionLevel)
		.def_prop_rw("compression_method", &VTF::getCompressionMethod, &VTF::setCompressionMethod)
		.def("has_image_data", &VTF::hasImageData)
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
		.def("set_image", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			return self.setImage({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, filter, mip, frame, face, slice, quality);
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("filter"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_image_from_file", py::overload_cast<const std::string&, ImageConversion::ResizeFilter, uint8_t, uint16_t, uint8_t, uint16_t, float>(&VTF::setImage), py::arg("image_path"), py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("slice") = 0, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
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
		.def("set_thumbnail", [](VTF& self, const py::bytes& imageData, ImageFormat format, uint16_t width, uint16_t height, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY) {
			return self.setThumbnail({static_cast<const std::byte*>(imageData.data()), imageData.size()}, format, width, height, quality);
		}, py::arg("image_data"), py::arg("format"), py::arg("width"), py::arg("height"), py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("set_thumbnail_from_file", py::overload_cast<const std::string&, float>(&VTF::setThumbnail), py::arg("image_path"), py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("compute_thumbnail", &VTF::computeThumbnail, py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT, py::arg("quality") = ImageConversion::DEFAULT_COMPRESSED_QUALITY)
		.def("remove_thumbnail", &VTF::removeThumbnail)
		.def("save_thumbnail", [](const VTF& self, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveThumbnailToFile(fileFormat);
			return py::bytes{d.data(), d.size()};
		}, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
		.def("save_thumbnail_to_file", py::overload_cast<const std::string&, ImageConversion::FileFormat>(&VTF::saveThumbnailToFile, py::const_), py::arg("image_path"), py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
		.def("has_fallback_data", &VTF::hasFallbackData)
		.def("get_fallback_data_raw", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataRaw(mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0)
		.def("get_fallback_data_as", [](const VTF& self, ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataAs(newFormat, mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, py::arg("new_format"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0)
		.def("get_fallback_data_as_rgba8888", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) {
			const auto d = self.getFallbackDataAsRGBA8888(mip, frame, face);
			return py::bytes{d.data(), d.size()};
		}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0)
		.def("compute_fallback", &VTF::computeFallback, py::arg("filter") = ImageConversion::ResizeFilter::DEFAULT)
		.def("remove_fallback", &VTF::removeFallback)
		.def("save_fallback", [](const VTF& self, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) {
			const auto d = self.saveFallbackToFile(mip, frame, face, fileFormat);
			return py::bytes{d.data(), d.size()};
		}, py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
		.def("save_fallback_to_file", py::overload_cast<const std::string&, uint8_t, uint16_t, uint8_t, ImageConversion::FileFormat>(&VTF::saveFallbackToFile, py::const_), py::arg("image_path"), py::arg("mip") = 0, py::arg("frame") = 0, py::arg("face") = 0, py::arg("file_format") = ImageConversion::FileFormat::DEFAULT)
		.def_prop_rw("xbox_mip_scale", &VTF::getXBOXMipScale, &VTF::setXBOXMipScale)
		.def("bake", [](const VTF& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&VTF::bake, py::const_), py::arg("vtf_path"));
}

} // namespace vtfpp
