#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t steampp_appid_t;

typedef void* steampp_steam_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: steampp_steam_free
SOURCEPP_API steampp_steam_handle_t steampp_steam_new();

SOURCEPP_API void steampp_steam_free(steampp_steam_handle_t* handle);

SOURCEPP_API const char* steampp_get_install_dir(steampp_steam_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_API sourcepp_string_array_t steampp_get_library_dirs(steampp_steam_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_sourcemod_dir(steampp_steam_handle_t handle);

SOURCEPP_API size_t steampp_get_installed_apps(steampp_steam_handle_t handle, steampp_appid_t* array, size_t arrayLen);

SOURCEPP_API size_t steampp_get_installed_apps_count(steampp_steam_handle_t handle);

SOURCEPP_API bool steampp_is_app_installed(steampp_steam_handle_t handle, steampp_appid_t appID);

SOURCEPP_API const char* steampp_get_app_name(steampp_steam_handle_t handle, steampp_appid_t appID);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_app_install_dir(steampp_steam_handle_t handle, steampp_appid_t appID);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_app_icon_path(steampp_steam_handle_t handle, steampp_appid_t appID);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_app_logo_path(steampp_steam_handle_t handle, steampp_appid_t appID);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_app_box_art_path(steampp_steam_handle_t handle, steampp_appid_t appID);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t steampp_get_app_store_art_path(steampp_steam_handle_t handle, steampp_appid_t appID);

SOURCEPP_API bool steampp_is_app_using_goldsrc_engine(steampp_steam_handle_t handle, steampp_appid_t appID);

SOURCEPP_API bool steampp_is_app_using_source_engine(steampp_steam_handle_t handle, steampp_appid_t appID);

SOURCEPP_API bool steampp_is_app_using_source_2_engine(steampp_steam_handle_t handle, steampp_appid_t appID);
