/**
 * Based on SteamAppPathProvider. See README.md and THIRDPARTY_LEGAL_NOTICES.txt for more information.
 */

#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <kvpp/KV1Binary.h>

namespace steampp {

using AppID = uint32_t;

class Steam {
public:
	Steam();

	[[nodiscard]] const std::filesystem::path& getInstallDir() const;

	[[nodiscard]] std::span<const std::filesystem::path> getLibraryDirs() const;

	[[nodiscard]] std::filesystem::path getSourceModDir() const;

	[[nodiscard]] std::vector<AppID> getInstalledApps() const;

	[[nodiscard]] bool isAppInstalled(AppID appID) const;

	[[nodiscard]] std::string_view getAppName(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppInstallDir(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppIconPath(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppLogoPath(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppHeroPath(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppBoxArtPath(AppID appID) const;

	[[nodiscard]] std::filesystem::path getAppStoreArtPath(AppID appID) const;

	[[nodiscard]] bool isAppUsingGoldSrcEngine(AppID appID) const;

	[[nodiscard]] bool isAppUsingSourceEngine(AppID appID) const;

	[[nodiscard]] bool isAppUsingSource2Engine(AppID appID) const;

	[[nodiscard]] explicit operator bool() const;

private:
	struct GameInfo {
		std::string name;
		std::filesystem::path installDir;
		std::size_t libraryInstallDirsIndex;
	};

	std::unordered_map<AppID, GameInfo> gameDetails;
	std::filesystem::path steamInstallDir;
	std::vector<std::filesystem::path> libraryDirs;
	kvpp::KV1Binary assetCache;
};

} // namespace steampp
