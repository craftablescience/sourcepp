#include <steamppc/steampp.h>

#include <steampp/steampp.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <steamppc/Convert.hpp>

using namespace steampp;

SOURCEPP_STATIC(steampp, steam, steampp_steam_handle_t, new) {
	auto* steam = new Steam{};
	if (!*steam) {
		delete steam;
		steam = nullptr;
	}
	return steam;
}

SOURCEPP_STATIC(steampp, steam, void, free, steampp_steam_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::steam(*handle);
	*handle = nullptr;
}

SOURCEPP_METHOD(steampp, steam, const char*, get_install_dir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, "");

	return Convert::steam(handle)->getInstallDir().data();
}

SOURCEPP_METHOD(steampp, steam, sourcepp_string_array_t, get_library_dirs) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	return Convert::toStringArray(Convert::steam(handle)->getLibraryDirs());
}

SOURCEPP_METHOD(steampp, steam, sourcepp_string_t, get_sourcemod_dir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getSourceModDir());
}

SOURCEPP_METHOD(steampp, steam, sourcepp_buffer_uint32_t, get_installed_apps) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::steam(handle)->getInstalledApps());
}

SOURCEPP_METHOD(steampp, steam, size_t, get_installed_apps_count) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::steam(handle)->getInstalledApps().size();
}

SOURCEPP_METHOE(steampp, steam, int, is_app_installed, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppInstalled(appID);
}

SOURCEPP_METHOE(steampp, steam, const char*, get_app_name, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, "");

	return Convert::steam(handle)->getAppName(appID).data();
}

SOURCEPP_METHOE(steampp, steam, sourcepp_string_t, get_app_install_dir, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppInstallDir(appID));
}

SOURCEPP_METHOE(steampp, steam, sourcepp_string_t, get_app_icon_path, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppIconPath(appID));
}

SOURCEPP_METHOE(steampp, steam, sourcepp_string_t, get_app_logo_path, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppLogoPath(appID));
}

SOURCEPP_METHOE(steampp, steam, sourcepp_string_t, get_app_box_art_path, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppBoxArtPath(appID));
}

SOURCEPP_METHOE(steampp, steam, sourcepp_string_t, get_app_store_art_path, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::steam(handle)->getAppStoreArtPath(appID));
}

SOURCEPP_METHOE(steampp, steam, int, is_app_using_goldsrc_engine, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingGoldSrcEngine(appID);
}

SOURCEPP_METHOE(steampp, steam, int, is_app_using_source_engine, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingSourceEngine(appID);
}

SOURCEPP_METHOE(steampp, steam, int, is_app_using_source_2_engine, steampp_appid_t appID) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::steam(handle)->isAppUsingSource2Engine(appID);
}
