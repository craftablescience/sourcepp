#include <vtfppc/ImageConversion.h>

#include <vtfpp/ImageConversion.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>

using namespace vtfpp;

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint16_t width, uint16_t height) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(oldFormat != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newFormat != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::convertImageDataToFormat({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(oldFormat), static_cast<ImageFormat>(newFormat), width, height));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_convert_several_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(oldFormat != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newFormat != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(mipCount, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(frameCount, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(faceCount == 0 || faceCount == 6 || faceCount == 7, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(sliceCount, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::convertSeveralImageDataToFormat({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(oldFormat), static_cast<ImageFormat>(newFormat), mipCount, frameCount, faceCount, width, height, sliceCount));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	const auto faces = ImageConversion::convertHDRIToCubeMap({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height);
	std::vector<std::byte> out;
	for (int i = 0; i < faces.size(); i++) {
		out.insert(out.end(), faces[i].begin(), faces[i].end());
	}
	return Convert::toBuffer(out);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap_ex(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t resolution, int bilinear) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	const auto faces = ImageConversion::convertHDRIToCubeMap({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height, resolution, bilinear);
	std::vector<std::byte> out;
	for (int i = 0; i < faces.size(); i++) {
		out.insert(out.end(), faces[i].begin(), faces[i].end());
	}
	return Convert::toBuffer(out);
}

SOURCEPP_API vtfpp_image_conversion_file_format_e vtfpp_image_conversion_get_default_file_format_for_image_format(vtfpp_image_format_e format) {
	return static_cast<vtfpp_image_conversion_file_format_e>(ImageConversion::getDefaultFileFormatForImageFormat(static_cast<ImageFormat>(format)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_file(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::convertImageDataToFile({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height, static_cast<ImageConversion::FileFormat>(fileFormat)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_file_to_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e* format, int* width, int* height, int* frameCount) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(frameCount, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::convertFileToImageData({reinterpret_cast<const std::byte*>(buffer), bufferLen}, reinterpret_cast<ImageFormat&>(*format), *width, *height, *frameCount));
}

SOURCEPP_API uint16_t vtfpp_image_conversion_get_resized_dim(uint16_t n, vtfpp_image_conversion_resize_method_e method) {
	return ImageConversion::getResizedDim(n, static_cast<ImageConversion::ResizeMethod>(method));
}

SOURCEPP_API void vtfpp_image_conversion_set_resized_dims(uint16_t* width, vtfpp_image_conversion_resize_method_e widthResize, uint16_t* height, vtfpp_image_conversion_resize_method_e heightResize) {
	SOURCEPP_EARLY_RETURN(width);
	SOURCEPP_EARLY_RETURN(height);

	ImageConversion::setResizedDims(*width, static_cast<ImageConversion::ResizeMethod>(widthResize), *height, static_cast<ImageConversion::ResizeMethod>(heightResize));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newWidth, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newHeight, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::resizeImageData({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, newWidth, height, newHeight, srgb, static_cast<ImageConversion::ResizeFilter>(filter), static_cast<ImageConversion::ResizeEdge>(edge)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data_strict(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t* widthOut, vtfpp_image_conversion_resize_method_e widthResize, uint16_t height, uint16_t newHeight, uint16_t* heightOut, vtfpp_image_conversion_resize_method_e heightResize, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newWidth, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(widthOut, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newHeight, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(heightOut, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::resizeImageDataStrict({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, newWidth, *widthOut, static_cast<ImageConversion::ResizeMethod>(widthResize), height, newHeight, *heightOut, static_cast<ImageConversion::ResizeMethod>(heightResize), srgb, static_cast<ImageConversion::ResizeFilter>(filter), static_cast<ImageConversion::ResizeEdge>(edge)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_crop_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newWidth, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(newHeight, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::cropImageData({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, newWidth, xOffset, height, newHeight, yOffset));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_gamma_correct_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float gamma) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::gammaCorrectImageData({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height, gamma));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_invert_green_channel_for_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(ImageConversion::invertGreenChannelForImageData({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height));
}
