/**
 * Based on SteamAppPathProvider. See README.md and THIRDPARTY_LEGAL_NOTICES.txt for more information.
 */

// ReSharper disable CppRedundantQualifier

#include <steampp/steampp.h>

#include <algorithm>
#include <filesystem>
#include <format>
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

bool isAppUsingGoldSrcEnginePredicate(const std::filesystem::path& installDir) {
	std::filesystem::directory_iterator dirIterator{installDir, std::filesystem::directory_options::skip_permission_denied};
	std::error_code ec;
	return std::any_of(std::filesystem::begin(dirIterator), std::filesystem::end(dirIterator), [&ec](const auto& entry){
		return entry.is_directory(ec) && std::filesystem::exists(entry.path() / "liblist.gam", ec);
	});
}

bool isAppUsingSourceEnginePredicate(const std::filesystem::path& installDir) {
	std::filesystem::directory_iterator dirIterator{installDir, std::filesystem::directory_options::skip_permission_denied};
	std::error_code ec;
	return std::any_of(std::filesystem::begin(dirIterator), std::filesystem::end(dirIterator), [&ec](const auto& entry){
		return entry.is_directory(ec) && std::filesystem::exists(entry.path() / "gameinfo.txt", ec);
	});
}

bool isAppUsingSource2EnginePredicate(const std::filesystem::path& installDir) {
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
std::unordered_set<AppID> getAppsKnownToUseEngine(bool(*p)(const std::filesystem::path&)) {
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

template<bool(*P)(const std::filesystem::path&)>
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

[[nodiscard]] std::filesystem::path getAppArtPath(const KV1Binary& assetCache, AppID appID, const std::filesystem::path& steamInstallDir, std::string_view id) {
	if (
		!assetCache[0].hasChild("cache_version") || !assetCache[0].hasChild("0") ||
		static_cast<KV1BinaryValueType>(assetCache[0]["cache_version"].getValue().index()) != KV1BinaryValueType::INT32 ||
		*assetCache[0]["cache_version"].getValue<int32_t>() != 2
	) {
		return {};
	}
	const auto idStr = std::format("{}", appID);
	// note: the "0" here means use the english version of the library assets. there's no easily accessible way to grab
	// the user's steam language or figure out what index each language is as far as i can tell, so I will use this hack instead.
	// if the asset exists, there will always be a base english asset present so no need to search for language overrides
	const auto& cache = assetCache[0]["0"][idStr];
	if (cache.isInvalid() || !cache.hasChild(id)) {
		return {};
	}
	auto path = steamInstallDir / "appcache" / "librarycache" / idStr / *cache[id].getValue<std::string>();
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		return {};
	}
	return path;
}

} // namespace

Steam::Steam() {
	std::filesystem::path steamLocation;
	std::error_code ec;

#ifdef _WIN32
	{
		// 16384 being the maximum length of a null-terminated path
		static constexpr DWORD STEAM_LOCATION_MAX_SIZE = 16384;
		std::unique_ptr<wchar_t[]> steamLocationData{new wchar_t[STEAM_LOCATION_MAX_SIZE] {}};

		HKEY steam;
		if (
			RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &steam) != ERROR_SUCCESS &&
			RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &steam) != ERROR_SUCCESS
		) {
			return;
		}

		DWORD steamLocationSize = STEAM_LOCATION_MAX_SIZE * sizeof(wchar_t);
		if (RegQueryValueExW(steam, L"InstallPath", nullptr, nullptr, reinterpret_cast<LPBYTE>(steamLocationData.get()), &steamLocationSize) != ERROR_SUCCESS) {
			RegCloseKey(steam);
			return;
		}
		RegCloseKey(steam);

		steamLocation = steamLocationSize > 0 ? std::filesystem::path{steamLocationData.get()} : std::filesystem::path{};
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
		std::filesystem::path location;
		std::filesystem::path d{"cwd/steamclient64.dll"};
		for (const auto& entry : std::filesystem::directory_iterator{"/proc/"}) {
			if (std::filesystem::exists(entry / d, ec)) {
				ec.clear();
				location = std::filesystem::read_symlink(entry.path() / "cwd", ec);
				if (ec) {
					continue;
				}
				break;
			}
		}
		if (location.empty()) {
			return;
		}
		steamLocation = location;
	}
#endif

	if (!std::filesystem::exists(steamLocation, ec)) {
		return;
	}
	this->steamInstallDir = steamLocation;

	auto libraryFoldersFilePath = steamLocation / "config" / "libraryfolders.vdf";
	if (!std::filesystem::exists(libraryFoldersFilePath, ec)) {
		libraryFoldersFilePath = steamLocation / "steamapps" / "libraryfolders.vdf";
		if (!std::filesystem::exists(libraryFoldersFilePath, ec)) {
			return;
		}
	}

	KV1 libraryFolders{fs::readFileText(libraryFoldersFilePath), true};

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
		libraryFolderPath /= "steamapps";

		if (!std::filesystem::exists(libraryFolderPath, ec)) {
			continue;
		}
		this->libraryDirs.push_back(libraryFolderPath);

		for (const auto& entry : std::filesystem::directory_iterator{libraryFolderPath, std::filesystem::directory_options::skip_permission_denied}) {
			auto entryName = entry.path().filename().string();
			if (!entryName.starts_with("appmanifest_") || !entryName.ends_with(".acf")) {
				continue;
			}

			KV1 appManifest(fs::readFileText(entry.path()));

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

	const auto assetCacheFilePath = steamLocation / "appcache" / "librarycache" / "assetcache.vdf";
	if (std::filesystem::exists(assetCacheFilePath, ec)) {
		this->assetCache = KV1Binary{fs::readFileBuffer(assetCacheFilePath)};
	}
}

const std::filesystem::path& Steam::getInstallDir() const {
	return this->steamInstallDir;
}

std::span<const std::filesystem::path> Steam::getLibraryDirs() const {
	return this->libraryDirs;
}

std::filesystem::path Steam::getSourceModDir() const {
	return this->steamInstallDir / "steamapps" / "sourcemods";
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

std::filesystem::path Steam::getAppInstallDir(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	return this->libraryDirs[this->gameDetails.at(appID).libraryInstallDirsIndex] / "common" / this->gameDetails.at(appID).installDir;
}

std::filesystem::path Steam::getAppIconPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	if (const auto cachedPath = ::getAppArtPath(this->assetCache, appID, this->steamInstallDir, "4f"); !cachedPath.empty()) {
		return cachedPath;
	}
	auto path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}", appID) / "icon.jpg";
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}_icon.jpg", appID);
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::filesystem::path Steam::getAppLogoPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	if (const auto cachedPath = ::getAppArtPath(this->assetCache, appID, this->steamInstallDir, "2f"); !cachedPath.empty()) {
		return cachedPath;
	}
	auto path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}", appID) / "logo.png";
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}_logo.png", appID);
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::filesystem::path Steam::getAppHeroPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	if (const auto cachedPath = ::getAppArtPath(this->assetCache, appID, this->steamInstallDir, "1f"); !cachedPath.empty()) {
		return cachedPath;
	}
	auto path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}", appID) / "library_hero.jpg";
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}_library_hero.jpg", appID);
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::filesystem::path Steam::getAppBoxArtPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	if (const auto cachedPath = ::getAppArtPath(this->assetCache, appID, this->steamInstallDir, "0f"); !cachedPath.empty()) {
		return cachedPath;
	}
	auto path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}", appID) / "library_600x900.jpg";
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}_library_600x900.jpg", appID);
		if (!std::filesystem::exists(path, ec)) {
			return "";
		}
	}
	return path;
}

std::filesystem::path Steam::getAppStoreArtPath(AppID appID) const {
	if (!this->gameDetails.contains(appID)) {
		return "";
	}
	if (const auto cachedPath = ::getAppArtPath(this->assetCache, appID, this->steamInstallDir, "3f"); !cachedPath.empty()) {
		return cachedPath;
	}
	auto path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}", appID) / "header.jpg";
	if (std::error_code ec; !std::filesystem::exists(path, ec)) {
		path = this->steamInstallDir / "appcache" / "librarycache" / std::format("{}_header.jpg", appID);
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
