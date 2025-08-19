#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gcf_open(const char* path, vpkpp_entry_callback_t callback, vpkpp_pack_file_open_property_request_t requestProperty);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_gcf_guid(vpkpp_pack_file_handle_t handle);

SOURCEPP_API uint32_t vpkpp_gcf_get_version(vpkpp_pack_file_handle_t handle);

SOURCEPP_API uint32_t vpkpp_gcf_get_appid(vpkpp_pack_file_handle_t handle);

SOURCEPP_API uint32_t vpkpp_gcf_get_app_version(vpkpp_pack_file_handle_t handle);
