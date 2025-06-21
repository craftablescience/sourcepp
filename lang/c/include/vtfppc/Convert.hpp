#pragma once

#include <vtfpp/vtfpp.h>

#include "PPL.h"
#include "SHT.h"
#include "TTX.h"
#include "VTF.h"

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

namespace Convert {

vtfpp::ImageConversion::ResizeMethod resizeMethod(vtfpp_image_conversion_resize_method_e method);

vtfpp_image_conversion_resize_method_e resizeMethod(vtfpp::ImageConversion::ResizeMethod method);

vtfpp::ImageConversion::ResizeFilter resizeFilter(vtfpp_image_conversion_resize_filter_e filter);

vtfpp::ImageConversion::FileFormat fileFormat(vtfpp_image_conversion_file_format_e fileFormat);

vtfpp::ImageFormat imageFormat(vtfpp_image_format_e format);

vtfpp_image_format_e imageFormat(vtfpp::ImageFormat format);

vtfpp::HOT* hot(vtfpp_hot_handle_t handle);

vtfpp::PPL* ppl(vtfpp_ppl_handle_t handle);

vtfpp::SHT* sht(vtfpp_sht_handle_t handle);

vtfpp::TTX* ttx(vtfpp_ttx_handle_t handle);

vtfpp::CompressionMethod compressionMethod(vtfpp_compression_method_e method);

vtfpp_compression_method_e compressionMethod(vtfpp::CompressionMethod method);

vtfpp::Resource::Type resourceType(vtfpp_resource_type_e type);

vtfpp_resource_type_e resourceType(vtfpp::Resource::Type type);

vtfpp::Resource::Flags resourceFlags(vtfpp_resource_flags_e flags);

vtfpp_resource_flags_e resourceFlags(vtfpp::Resource::Flags flags);

const vtfpp::Resource* resource(vtfpp_resource_handle_t resource);

vtfpp::VTF::Platform vtfPlatform(vtfpp_vtf_platform_e platform);

vtfpp_vtf_platform_e vtfPlatform(vtfpp::VTF::Platform platform);

vtfpp::VTF::CreationOptions vtfCreationOptions(vtfpp_vtf_creation_options_t options);

vtfpp::VTF* vtf(vtfpp_vtf_handle_t handle);

} // namespace Convert
