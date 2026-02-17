#include <steamppc/steampp.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace steampp;

SOURCEPP_API steampp_steam_handle_t steampp_steam_new() {
	auto* steam = new Steam{};
	if (!*steam) {
		delete steam;
		steam = nullptr;
	}
	return steam;
}

SOURCEPP_API void steampp_steam_free(steampp_steam_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::cast(*handle);
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_install_dir(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getInstallDir().string());
}

SOURCEPP_API sourcepp_string_array_t steampp_steam_get_library_dirs(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	std::vector<std::string> libraryDirs;
	for (const auto& dir : convert::cast(handle)->getLibraryDirs()) {
		libraryDirs.push_back(dir.string());
	}
	return convert::toStringArray(libraryDirs);
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_sourcemod_dir(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getSourceModDir().string());
}

SOURCEPP_API sourcepp_buffer_uint32_t steampp_steam_get_installed_apps(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::cast(handle)->getInstalledApps());
}

SOURCEPP_API size_t steampp_steam_get_installed_apps_count(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::cast(handle)->getInstalledApps().size();
}

SOURCEPP_API int steampp_steam_is_app_installed(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::cast(handle)->isAppInstalled(appID);
}

SOURCEPP_API const char* steampp_steam_get_app_name(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, "");

	return convert::cast(handle)->getAppName(appID).data();
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_install_dir(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppInstallDir(appID).string());
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_icon_path(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppIconPath(appID).string());
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_logo_path(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppLogoPath(appID).string());
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_hero_path(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppHeroPath(appID).string());
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_box_art_path(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppBoxArtPath(appID).string());
}

SOURCEPP_API sourcepp_string_t steampp_steam_get_app_store_art_path(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::cast(handle)->getAppStoreArtPath(appID).string());
}

SOURCEPP_API int steampp_steam_is_app_using_goldsrc_engine(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::cast(handle)->isAppUsingGoldSrcEngine(appID);
}

SOURCEPP_API int steampp_steam_is_app_using_source_engine(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::cast(handle)->isAppUsingSourceEngine(appID);
}

SOURCEPP_API int steampp_steam_is_app_using_source_2_engine(steampp_steam_handle_t handle, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::cast(handle)->isAppUsingSource2Engine(appID);
}

SOURCEPP_API int steampp_steam_is_valid(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return static_cast<bool>(*convert::cast(handle));
}
