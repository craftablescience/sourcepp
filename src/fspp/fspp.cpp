#include <fspp/fspp.h>

#include <filesystem>
#include <ranges>

#include <kvpp/kvpp.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>
#include <vpkpp/format/BSP.h>

using namespace fspp;
using namespace kvpp;
using namespace sourcepp;
using namespace steampp;
using namespace vpkpp;

namespace {

#if defined(_WIN32)
	constexpr std::string_view FS_PLATFORM = "win64";
#elif defined(__APPLE__)
	constexpr std::string_view FS_PLATFORM = "osx64";
#elif defined(__linux__)
	constexpr std::string_view FS_PLATFORM = "linux64";
#else
	#error "Unknown platform!"
#endif

[[nodiscard]] std::string getAppInstallDir(AppID appID) {
	static Steam steam;
	return steam.getAppInstallDir(appID);
}

} // namespace

std::optional<FileSystem> FileSystem::load(steampp::AppID appID, std::string_view gameName, const FileSystemOptions& options) {
	auto gamePath = ::getAppInstallDir(appID);
	if (gamePath.empty()) {
		return std::nullopt;
	}
	return load((std::filesystem::path{gamePath} / gameName).string(), options);
}

std::optional<FileSystem> FileSystem::load(std::string_view gamePath, const FileSystemOptions& options) {
	if (!std::filesystem::exists(std::filesystem::path{gamePath} / "gameinfo.txt") || !std::filesystem::is_regular_file(std::filesystem::path{gamePath} / "gameinfo.txt")) {
		return std::nullopt;
	}
	return FileSystem{gamePath, options};
}

FileSystem::FileSystem(std::string_view gamePath, const FileSystemOptions& options) {
	SearchPathMap dirSearchPaths;
	SearchPathMap vpkSearchPaths;

	// Load paths from gameinfo.txt
	KV1 gameinfo{fs::readFileText((std::filesystem::path{gamePath} / "gameinfo.txt").string())};
	if (gameinfo.getChildCount() == 0) {
		return;
	}
	const auto& searchPathKVs = gameinfo[0]["FileSystem"]["SearchPaths"];
	if (searchPathKVs.isInvalid()) {
		return;
	}
	for (int i = 0; i < searchPathKVs.getChildCount(); i++) {
		auto searches = string::split(searchPathKVs.getKey(), '+');
		auto path = std::string{searchPathKVs[i].getValue()};

		// Replace |all_source_engine_paths| with <root>/, |gameinfo_path| with <root>/<game>/
		static constexpr std::string_view ALL_SOURCE_ENGINE_PATHS = "|all_source_engine_paths|";
		static constexpr std::string_view GAMEINFO_PATH = "|gameinfo_path|";
		if (path.starts_with(ALL_SOURCE_ENGINE_PATHS)) {
			path = (std::filesystem::path{gamePath} / ".." / path.substr(ALL_SOURCE_ENGINE_PATHS.length())).string();
		} else if (path.starts_with(GAMEINFO_PATH)) {
			path = (std::filesystem::path{gamePath} / path.substr(GAMEINFO_PATH.length())).string();
		}

		if (path.ends_with(".vpk")) {
			// Normalize the ending (add _dir if present)
			if (!std::filesystem::exists(path)) {
				auto pathWithDir = (std::filesystem::path{path}.parent_path() / std::filesystem::path{path}.stem()).string() + "_dir.vpk";
				if (!std::filesystem::exists(pathWithDir)) {
					continue;
				}
				path = pathWithDir;
			}

			for (const auto& search : searches) {
				if (!vpkSearchPaths.contains(search)) {
					vpkSearchPaths[search] = {};
				}
				vpkSearchPaths[search].push_back(path);
			}
		} else {
			for (const auto& search : searches) {
				if (!dirSearchPaths.contains(search)) {
					dirSearchPaths[search] = {};
				}
				dirSearchPaths[search].push_back(path);
			}
		}
	}

	// Add DLCs / update dir / xlsppatch dir if they exist

	// Add EXECUTABLE_PATH if it doesn't exist, point it at <root>/bin/<platform>/;<root>/bin/;<root>/

	// Add PLATFORM if it doesn't exist, point it at <root>/platform/

	// Add DEFAULT_WRITE_PATH, LOGDIR if they doesn't exist, point them at <root>/<game>/

	// Add CONFIG if it doesn't exist, point it at <root>/platform/config/

	// Merge dir/vpk search paths together
	const auto* firstSearchPathsMap = options.prioritizeVPKs ? &vpkSearchPaths : &dirSearchPaths;
	const auto* secondSearchPathsMap = options.prioritizeVPKs ? &dirSearchPaths : &vpkSearchPaths;
	for (const auto& [search, paths] : *firstSearchPathsMap) {
		this->searchPaths[search] = paths;
	}
	for (const auto& [search, paths] : *secondSearchPathsMap) {
		if (this->searchPaths.contains(search)) {
			// insert
		} else {
			this->searchPaths[search] = paths;
		}
	}
}

std::vector<std::string> FileSystem::getSearchPaths() const {
	auto keys = std::views::keys(this->searchPaths);
	return {keys.begin(), keys.end()};
}

const std::vector<std::string>& FileSystem::getPathsForSearchPath(std::string_view searchPath) const {
	return this->searchPaths.at(std::string{searchPath});
}

const FileSystem::SearchPathMap& FileSystem::getSearchPathData() const {
	return this->searchPaths;
}

std::optional<std::vector<std::byte>> FileSystem::read(std::string_view filePath, std::string_view searchPath) const {
	if (!this->searchPaths.contains(std::string{searchPath})) {
		return std::nullopt;
	}
	return std::nullopt;
}

std::optional<std::vector<std::byte>> FileSystem::readForMap(BSP& map, std::string_view filePath, std::string_view searchPath) const {
	if (!this->searchPaths.contains(std::string{searchPath})) {
		return std::nullopt;
	}
	return std::nullopt;
}
