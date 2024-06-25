/**
 * Based on SteamAppPathProvider. See README.md and THIRDPARTY_LEGAL_NOTICES.txt for more information.
 */

#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace steampp {

using AppID = uint32_t;

class Steam {
public:
	Steam();

	[[nodiscard]] std::string_view getInstallDir() const;

	[[nodiscard]] const std::vector<std::string>& getLibraryDirs() const;

	[[nodiscard]] std::string getSourceModDir() const;

	[[nodiscard]] std::vector<AppID> getInstalledApps() const;

	[[nodiscard]] bool isAppInstalled(AppID appID) const;

	[[nodiscard]] std::string_view getAppName(AppID appID) const;

	[[nodiscard]] std::string getAppInstallDir(AppID appID) const;

	[[nodiscard]] std::string getAppIconPath(AppID appID) const;

	[[nodiscard]] std::string getAppLogoPath(AppID appID) const;

	[[nodiscard]] std::string getAppBoxArtPath(AppID appID) const;

	[[nodiscard]] std::string getAppStoreArtPath(AppID appID) const;

	[[nodiscard]] bool isAppUsingSourceEngine(AppID appID) const;

	[[nodiscard]] bool isAppUsingSource2Engine(AppID appID) const;

	[[nodiscard]] explicit operator bool() const;

private:
	struct GameInfo {
		std::string name;
		std::string installDir;
		std::size_t libraryInstallDirsIndex;
	};

	std::unordered_map<AppID, GameInfo> gameDetails;
	std::string steamInstallDir;
	std::vector<std::string> libraryDirs;
};

} // namespace steampp
