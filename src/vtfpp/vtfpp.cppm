module;

#include <vtfpp/ImageConversion.h>
#include <vtfpp/ImageFormats.h>
#include <vtfpp/vtfpp.h>

export module vtfpp;

export {

	// ImageConversion.h
	namespace vtfpp::ImageConversion {
		using vtfpp::ImageConversion::convertImageDataToFormat;
		using vtfpp::ImageConversion::convertImageDataToFile;
	} // namespace vtfpp::ImageConversion

	// ImageFormats.h
	namespace vtfpp {
		namespace ImageDimensions {
			using vtfpp::ImageDimensions::getMipDim;
		} // namespace vtfpp::ImageDimensions

		using vtfpp::ImageFormat;

		namespace ImageFormatDetails {
			using vtfpp::ImageFormatDetails::red;
			using vtfpp::ImageFormatDetails::green;
			using vtfpp::ImageFormatDetails::blue;
			using vtfpp::ImageFormatDetails::alpha;
			using vtfpp::ImageFormatDetails::bpp;
			using vtfpp::ImageFormatDetails::compressed;
			using vtfpp::ImageFormatDetails::large;
			using vtfpp::ImageFormatDetails::transparent;
			using vtfpp::ImageFormatDetails::opaque;
			using vtfpp::ImageFormatDetails::getDataLength;
			using vtfpp::ImageFormatDetails::getDataPosition;
		} // namespace ImageFormatDetails
	} // namespace vtfpp

	// vtfpp.h
	namespace vtfpp {
		using vtfpp::Resource;
		using vtfpp::VTF;
	} // namespace vtfpp

} // export
