#include <steamppc/steampp.h>

#include <steampp/steampp.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <steamppc/Convert.hpp>

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

	delete Convert::steam(*handle);
	*handle = nullptr;
}

SOURCEPP_API const char* steampp_get_install_dir(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, "");

	return Convert::steam(handle)->getInstallDir().data();
}

SOURCEPP_API sourcepp_string_array_t steampp_get_library_dirs(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	return Convert::toStringArray(Convert::steam(handle)->getLibraryDirs());
}

SOURCEPP_API sourcepp_string_t steampp_get_sourcemod_dir(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getSourceModDir());
}

SOURCEPP_API size_t steampp_get_installed_apps(steampp_steam_handle_t handle, AppID* array, size_t arrayLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(array, 0);
	SOURCEPP_EARLY_RETURN_VAL(arrayLen, 0);

	return Convert::writeVectorToMem<AppID>(Convert::steam(handle)->getInstalledApps(), array, arrayLen);
}

SOURCEPP_API size_t steampp_get_installed_apps_count(steampp_steam_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::steam(handle)->getInstalledApps().size();
}

SOURCEPP_API bool steampp_is_app_installed(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppInstalled(appID);
}

SOURCEPP_API const char* steampp_get_app_name(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, "");

	return Convert::steam(handle)->getAppName(appID).data();
}

SOURCEPP_API sourcepp_string_t steampp_get_app_install_dir(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppInstallDir(appID));
}

SOURCEPP_API sourcepp_string_t steampp_get_app_icon_path(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppIconPath(appID));
}

SOURCEPP_API sourcepp_string_t steampp_get_app_logo_path(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppLogoPath(appID));
}

SOURCEPP_API sourcepp_string_t steampp_get_app_box_art_path(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppBoxArtPath(appID));
}

SOURCEPP_API sourcepp_string_t steampp_get_app_store_art_path(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppStoreArtPath(appID));
}

SOURCEPP_API bool steampp_is_app_using_goldsrc_engine(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingGoldSrcEngine(appID);
}

SOURCEPP_API bool steampp_is_app_using_source_engine(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingSourceEngine(appID);
}

SOURCEPP_API bool steampp_is_app_using_source_2_engine(steampp_steam_handle_t handle, AppID appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingSource2Engine(appID);
}
