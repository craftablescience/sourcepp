#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

SOURCEPP_TYPEDF(steampp, appid, uint32_t);

SOURCEPP_HANDLE(steampp, steam);
SOURCEPP_STATIC(steampp, steam,   steampp_steam_handle_t, new); // REQUIRES MANUAL FREE: steampp_steam_free
SOURCEPP_STATIC(steampp, steam,                     void, free, steampp_steam_handle_t* handle);
SOURCEPP_METHOD(steampp, steam,              const char*, get_install_dir);
SOURCEPP_METHOD(steampp, steam,  sourcepp_string_array_t, get_library_dirs); // REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_METHOD(steampp, steam,        sourcepp_string_t, get_sourcemod_dir); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOD(steampp, steam, sourcepp_buffer_uint32_t, get_installed_apps); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_METHOD(steampp, steam,                   size_t, get_installed_apps_count);
SOURCEPP_METHOE(steampp, steam,                      int, is_app_installed, steampp_appid_t appID);
SOURCEPP_METHOE(steampp, steam,              const char*, get_app_name, steampp_appid_t appID);
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_install_dir, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_icon_path, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_logo_path, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_hero_path, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_box_art_path, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,        sourcepp_string_t, get_app_store_art_path, steampp_appid_t appID); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOE(steampp, steam,                      int, is_app_using_goldsrc_engine, steampp_appid_t appID);
SOURCEPP_METHOE(steampp, steam,                      int, is_app_using_source_engine, steampp_appid_t appID);
SOURCEPP_METHOE(steampp, steam,                      int, is_app_using_source_2_engine, steampp_appid_t appID);
