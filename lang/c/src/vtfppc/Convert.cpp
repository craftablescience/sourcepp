#include <vtfppc/Convert.hpp>

#include <vtfpp/PPL.h>
#include <vtfpp/SHT.h>
#include <vtfpp/TTX.h>
#include <vtfpp/VTF.h>

using namespace vtfpp;

ImageConversion::ResizeMethod Convert::resizeMethod(vtfpp_image_conversion_resize_method_e method) {
	return static_cast<ImageConversion::ResizeMethod>(method);
}

vtfpp_image_conversion_resize_method_e Convert::resizeMethod(vtfpp::ImageConversion::ResizeMethod method) {
	return static_cast<vtfpp_image_conversion_resize_method_e>(method);
}

ImageConversion::ResizeFilter Convert::resizeFilter(vtfpp_image_conversion_resize_filter_e filter) {
	return static_cast<ImageConversion::ResizeFilter>(filter);
}

ImageConversion::FileFormat Convert::fileFormat(vtfpp_image_conversion_file_format_e fileFormat) {
	return static_cast<ImageConversion::FileFormat>(fileFormat);
}

ImageFormat Convert::imageFormat(vtfpp_image_format_e format) {
	return static_cast<ImageFormat>(format);
}

vtfpp_image_format_e Convert::imageFormat(vtfpp::ImageFormat format) {
	return static_cast<vtfpp_image_format_e>(format);
}

PPL* Convert::ppl(vtfpp_ppl_handle_t handle) {
	return static_cast<PPL*>(handle);
}

SHT* Convert::sht(vtfpp_sht_handle_t handle) {
	return static_cast<SHT*>(handle);
}

TTX* Convert::ttx(vtfpp_ttx_handle_t handle) {
	return static_cast<TTX*>(handle);
}

CompressionMethod Convert::compressionMethod(vtfpp_compression_method_e method) {
	return static_cast<CompressionMethod>(method);
}

vtfpp_compression_method_e Convert::compressionMethod(CompressionMethod method) {
	return static_cast<vtfpp_compression_method_e>(method);
}

Resource::Type Convert::resourceType(vtfpp_resource_type_e type) {
	return static_cast<Resource::Type>(type);
}

vtfpp_resource_type_e Convert::resourceType(Resource::Type type) {
	return static_cast<vtfpp_resource_type_e>(type);
}

Resource::Flags Convert::resourceFlags(vtfpp_resource_flags_e flags) {
	return static_cast<Resource::Flags>(flags);
}

vtfpp_resource_flags_e Convert::resourceFlags(Resource::Flags flags) {
	return static_cast<vtfpp_resource_flags_e>(flags);
}

const Resource* Convert::resource(vtfpp_resource_handle_t resource) {
	return static_cast<const Resource*>(resource);
}

VTF::Flags Convert::vtfFlags(vtfpp_vtf_flags_e flags) {
	return static_cast<VTF::Flags>(flags);
}

vtfpp_vtf_flags_e Convert::vtfFlags(VTF::Flags flags) {
	return static_cast<vtfpp_vtf_flags_e>(flags);
}

VTF::Platform Convert::vtfPlatform(vtfpp_vtf_platform_e platform) {
	return static_cast<VTF::Platform>(platform);
}

vtfpp_vtf_platform_e Convert::vtfPlatform(VTF::Platform platform) {
	return static_cast<vtfpp_vtf_platform_e>(platform);
}

VTF::CreationOptions Convert::vtfCreationOptions(vtfpp_vtf_creation_options_t options) {
	return {
		.majorVersion = options.majorVersion,
		.minorVersion = options.minorVersion,
		.outputFormat = Convert::imageFormat(options.outputFormat),
		.widthResizeMethod = Convert::resizeMethod(options.widthResizeMethod),
		.heightResizeMethod = Convert::resizeMethod(options.heightResizeMethod),
		.filter = Convert::resizeFilter(options.filter),
		.flags = Convert::vtfFlags(options.flags),
		.initialFrameCount = options.initialFrameCount,
		.startFrame = options.startFrame,
		.isCubeMap = static_cast<bool>(options.isCubeMap),
		.hasSphereMap = static_cast<bool>(options.hasSphereMap),
		.initialSliceCount = options.initialSliceCount,
		.computeTransparencyFlags = static_cast<bool>(options.computeTransparencyFlags),
		.computeMips = static_cast<bool>(options.computeMips),
		.computeThumbnail = static_cast<bool>(options.computeThumbnail),
		.computeReflectivity = static_cast<bool>(options.computeReflectivity),
		.platform = Convert::vtfPlatform(options.platform),
		.compressionLevel = options.compressionLevel,
		.compressionMethod = Convert::compressionMethod(options.compressionMethod),
		.bumpMapScale = options.bumpMapScale,
		.gammaCorrection = options.gammaCorrection,
		.invertGreenChannel = static_cast<bool>(options.invertGreenChannel),
	};
}

VTF* Convert::vtf(vtfpp_vtf_handle_t handle) {
	return static_cast<VTF*>(handle);
}
