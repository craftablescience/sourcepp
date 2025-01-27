/**
 * Based on SteamAppPathProvider. See README.md and THIRDPARTY_LEGAL_NOTICES.txt for more information.
 */

#include <steampp/steampp.h>

#include <algorithm>
#include <filesystem>
#include <ranges>
#include <unordered_set>

#ifdef _WIN32
#include <memory>
#include <Windows.h>
#else
#include <cstdlib>
#endif

#include <kvpp/kvpp.h>
#include <sourcepp/FS.h>

using namespace kvpp;
using namespace sourcepp;
using namespace steampp;

namespace {

bool isAppUsingGoldSrcEnginePredicate(std::string_view installDir) {
	std::filesystem::directory_iterator dirIterator{installDir, std::filesystem::directory_options::skip_permission_denied};
	std::error_code ec;
	return std::any_of(std::filesystem::begin(dirIterator), std::filesystem::end(dirIterator), [&ec](const auto& entry){
		return entry.is_directory(ec) && std::filesystem::exists(entry.path() / "liblist.gam", ec);
	});
}

bool isAppUsingSourceEnginePredicate(std::string_view installDir) {
	std::filesystem::directory_iterator dirIterator{installDir, std::filesystem::directory_options::skip_permission_denied};
	std::error_code ec;
	return std::any_of(std::filesystem::begin(dirIterator), std::filesystem::end(dirIterator), [&ec](const auto& entry){
		return entry.is_directory(ec) && std::filesystem::exists(entry.path() / "gameinfo.txt", ec);
	});
}

bool isAppUsingSource2EnginePredicate(std::string_view installDir) {
	std::filesystem::directory_iterator dirIterator{installDir, std::filesystem::directory_options::skip_permission_denied};
	std::error_code ec;
	return std::any_of(std::filesystem::begin(dirIterator), std::filesystem::end(dirIterator), [&ec](const auto& entry) {
		if (!entry.is_directory(ec)) {
			return false;
		}
		if (std::filesystem::exists(entry.path() / "gameinfo.gi", ec)) {
			return true;
		}
		std::filesystem::directory_iterator subDirIterator{entry.path(), std::filesystem::directory_options::skip_permission_denied};
		return std::any_of(std::filesystem::begin(subDirIterator), std::filesystem::end(subDirIterator), [&ec](const auto& entry_) {
			return entry_.is_directory(ec) && std::filesystem::exists(entry_.path() / "gameinfo.gi", ec);
		});
	});
}

// Note: this can't be a template because gcc threw a fit. No idea why
std::unordered_set<AppID> getAppsKnownToUseEngine(bool(*p)(std::string_view)) {
	if (p == &::isAppUsingGoldSrcEnginePredicate) {
		return {
#include "cache/EngineGoldSrc.inl"
		};
	}
	if (p == &::isAppUsingSourceEnginePredicate) {
		return {
#include "cache/EngineSource.inl"
		};
	}
	if (p == &::isAppUsingSource2EnginePredicate) {
		return {
#include "cache/EngineSource2.inl"
		};
	}
	return {};
}

template<bool(*P)(std::string_view)>
bool isAppUsingEngine(const Steam* steam, AppID appID) {
	static std::unordered_set<AppID> knownIs = ::getAppsKnownToUseEngine(P);
	if (knownIs.contains(appID)) {
		return true;
	}

	static std::unordered_set<AppID> knownIsNot;
	if (knownIsNot.contains(appID)) {
		return false;
	}

	if (!steam->isAppInstalled(appID)) {
		return false;
	}

	const auto installDir = steam->getAppInstallDir(appID);
	if (std::error_code ec; !std::filesystem::exists(installDir, ec)) [[unlikely]] {
		return false;
	}

	if (P(installDir)) {
		knownIs.emplace(appID);
		return true;
	}
	knownIsNot.emplace(appID);
	return false;
}

} // namespace

Steam::Steam() {
	std::filesystem::path steamLocation;
	std::error_code ec;

#ifdef _WIN32
	{
		// 16383 being the maximum length of a path
		static constexpr DWORD STEAM_LOCATION_MAX_SIZE = 16383;
		std::unique_ptr<char[]> steamLocationData{new char[STEAM_LOCATION_MAX_SIZE]};

		HKEY steam;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Valve\Steam)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &steam) != ERROR_SUCCESS) {
			return;
		}

		DWORD steamLocationSize = STEAM_LOCATION_MAX_SIZE;
		if (RegQueryValueExA(steam, "InstallPath", nullptr, nullptr, reinterpret_cast<LPBYTE>(steamLocationData.get()), &steamLocationSize) != ERROR_SUCCESS) {
			return;
		}

		RegCloseKey(steam);
		steamLocation = steamLocationSize > 0 ? std::string(steamLocationData.get(), steamLocationSize - 1) : "";
	}
#else
	{
		std::filesystem::path home{std::getenv("HOME")};
#ifdef __APPLE__
		steamLocation = home / "Library" / "Application Support" / "Steam";
#else
		// Snap install takes priority, the .steam symlink may exist simultaneously with Snap installs
		steamLocation = home / "snap" / "steam" / "common" / ".steam" / "steam";

		if (!std::filesystem::exists(steamLocation, ec)) {
			// Use the regular install path
			steamLocation = home / ".steam" / "steam";
		}
#endif
	}

	if (!std::filesystem::exists(steamLocation, ec)) {
		std::string location;
		std::filesystem::path d{"cwd/steamclient64.dll"};
		for (const auto& entry : std::filesystem::directory_iterator{"/proc/"}) {
			if (std::filesystem::exists(entry / d, ec)) {
				ec.clear();
				const auto s = std::filesystem::read_symlink(entry.path() / "cwd", ec);
				if (ec) {
					continue;
				}
				location = s.string();
				break;
			}
		}
		if (location.empty()) {
			return;
		}
		steamLocation = location;
	}
#endif

	if (!std::filesystem::exists(steamLocation.string(), ec)) {
		return;
	}
	this->steamInstallDir = steamLocation.string();

	auto libraryFoldersFilePath = steamLocation / "steamapps" / "libraryfolders.vdf";
	if (!std::filesystem::exists(libraryFoldersFilePath, ec)) {
		return;
	}

	KV1 libraryFolders{fs::readFileText(libraryFoldersFilePath.string())};

	const auto& libraryFoldersValue = libraryFolders["libraryfolders"];
	if (libraryFoldersValue.isInvalid()) {
		return;
	}

	for (uint64_t i = 0; i < libraryFoldersValue.getChildCount(); i++) {
		const auto& folder = libraryFoldersValue[i];

		auto folderName = folder.getKey();
		if (folderName == "TimeNextStatsReport" || folderName == "ContentStatsID") {
			continue;
		}

		const auto& folderPath = folder["path"];
		if (folderPath.isInvalid()) {
			continue;
		}

		std::filesystem::path libraryFolderPath{folderPath.getValue()};
		{
			std::string libraryFolderPathProcessedEscapes;
			bool hitBackslash = false;
			for (char c : libraryFolderPath.string()) {
				if (hitBackslash || c != '\\') {
					libraryFolderPathProcessedEscapes += c;
					hitBackslash = false;
				} else {
					hitBackslash = true;
				}
			}
			libraryFolderPath = libraryFolderPathProcessedEscapes;
		}
		libraryFolderPath /= "steamapps";

		if (!std::filesystem::exists(libraryFolderPath, ec)) {
			continue;
		}
		this->libraryDirs.push_back(libraryFolderPath.string());

		for (const auto& entry : std::filesystem::directory_iterator{libraryFolderPath, std::filesystem::directory_options::skip_permission_denied}) {
			auto entryName = entry.path().filename().string();
			if (!entryName.starts_with("appmanifest_") || !entryName.ends_with(".acf")) {
				continue;
			}

			KV1 appManifest(fs::readFileText(entry.path().string()));

			const auto& appState = appManifest["AppState"];
			if (appState.isInvalid()) {
				continue;
			}

			const auto& appName = appState["name"];
			if (appName.isInvalid()) {
				continue;
			}
			const auto& appInstallDir = appState["installdir"];
			if (appInstallDir.isInvalid()) {
				continue;
			}
			const auto& appID = appState["appid"];
			if (appID.isInvalid()) {
				continue;
			}

			this->gameDetails[std::stoi(std::string{appID.getValue()})] = GameInfo{
				.name = std::string{appName.getValue()},
				.installDir = std::string{appInstallDir.getValue()},
				.libraryInstallDirsIndex = this->libraryDirs.size() - 1,
			};
		}
	}
}

std::string_view Steam::getInstallDir() const {
	return this->steamInstallDir;
}

const std::vector<std::string>& Steam::getLibraryDirs() const {
	return this->libraryDirs;
}

std::string Steam::getSourceModDir() const {
	return (std::filesystem::path{this->steamInstallDir} / "steamapps" / "sourcemods").string();
}

std::vector<AppID> Steam::getInstalledApps() const {
	auto keys = std::views::keys(this->gameDetails);
	return {keys.begin(), keys.end()};
}

bool Steam::isAppInstalled(AppID appID) const {
	return this->gameDetails.contains(appID);
}

std::string_view Steam::getAppName(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	return this->gameDetails.at(appID).name;
}

std::string Steam::getAppInstallDir(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	return (std::filesystem::path{this->libraryDirs[this->gameDetails.at(appID).libraryInstallDirsIndex]} / "common" / this->gameDetails.at(appID).installDir).string();
}

std::string Steam::getAppIconPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	auto path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / (std::to_string(appID) + "_icon.jpg")).string();
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		// Currently the icon is the only file with a SHA-1 hash for a name. If this changes then we're fucked (need to make a binary KV1 parser)
		for (const auto& image : std::filesystem::directory_iterator{std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / std::to_string(appID), std::filesystem::directory_options::skip_permission_denied, ec}) {
			if (!image.is_regular_file()) {
				continue;
			}
			// SHA-1 = 160 bits -> 20 bytes -> 40 hex chars
			if (image.path().stem().string().size() == 40) {
				return image.path().string();
			}
		}
		return "";
	}
	return path;
}

std::string Steam::getAppLogoPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	auto path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / std::to_string(appID) / "logo.png").string();
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / (std::to_string(appID) + "_logo.png")).string();
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::string Steam::getAppBoxArtPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	auto path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / std::to_string(appID) / "library_600x900.jpg").string();
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / (std::to_string(appID) + "_library_600x900.jpg")).string();
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::string Steam::getAppStoreArtPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	auto path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / std::to_string(appID) / "header.jpg").string();
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = (std::filesystem::path{this->steamInstallDir} / "appcache" / "librarycache" / (std::to_string(appID) + "_header.jpg")).string();
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

bool Steam::isAppUsingGoldSrcEngine(AppID appID) const {
	return ::isAppUsingEngine<::isAppUsingGoldSrcEnginePredicate>(this, appID);
}

bool Steam::isAppUsingSourceEngine(AppID appID) const {
	return ::isAppUsingEngine<::isAppUsingSourceEnginePredicate>(this, appID);
}

bool Steam::isAppUsingSource2Engine(AppID appID) const {
	return ::isAppUsingEngine<::isAppUsingSource2EnginePredicate>(this, appID);
}

Steam::operator bool() const {
	return !this->gameDetails.empty();
}
