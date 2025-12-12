#include <fspp/fspp.h>

#include <filesystem>

#include <bsppp/bsppp.h>
#include <kvpp/kvpp.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>
#include <vpkpp/vpkpp.h>

using namespace bsppp;
using namespace fspp;
using namespace kvpp;
using namespace sourcepp;
using namespace steampp;
using namespace vpkpp;

namespace {

[[nodiscard]] std::string getAppInstallDir(AppID appID) {
	static Steam steam;
	return steam.getAppInstallDir(appID).string();
}

} // namespace

std::optional<FileSystem> FileSystem::load(AppID appID, std::string_view gameID, const FileSystemOptions& options) {
	const auto gamePath = ::getAppInstallDir(appID);
	if (gamePath.empty()) {
		return std::nullopt;
	}
	return load((std::filesystem::path{gamePath} / gameID).string(), options);
}

std::optional<FileSystem> FileSystem::load(std::string_view gamePath, const FileSystemOptions& options) {
	if (!std::filesystem::exists(std::filesystem::path{gamePath} / "gameinfo.txt") || !std::filesystem::is_regular_file(std::filesystem::path{gamePath} / "gameinfo.txt")) {
		return std::nullopt;
	}
	return FileSystem{gamePath, options};
}

FileSystem::FileSystem(std::string_view gamePath, const FileSystemOptions& options)
		: rootPath(std::filesystem::path{gamePath}.parent_path().string()) {
	string::normalizeSlashes(this->rootPath);
	const auto gameID = std::filesystem::path{gamePath}.filename().string();

	// Load gameinfo.txt
	KV1 gameinfo{fs::readFileText((std::filesystem::path{gamePath} / "gameinfo.txt").string())};
	if (gameinfo.getChildCount() == 0) {
		return;
	}

	// Load searchpaths
	const auto& searchPathKVs = gameinfo[0]["FileSystem"]["SearchPaths"];
	if (searchPathKVs.isInvalid()) {
		return;
	}
	for (int i = 0; i < searchPathKVs.getChildCount(); i++) {
		auto searches = string::split(string::toLower(searchPathKVs[i].getKey()), '+');
		auto path = std::string{string::toLower(searchPathKVs[i].getValue())};

		// Replace |all_source_engine_paths| with "", |gameinfo_path| with "<game>/"
		static constexpr std::string_view ALL_SOURCE_ENGINE_PATHS = "|all_source_engine_paths|";
		static constexpr std::string_view GAMEINFO_PATH = "|gameinfo_path|";
		if (path.starts_with(ALL_SOURCE_ENGINE_PATHS)) {
			path = path.substr(ALL_SOURCE_ENGINE_PATHS.length());
		} else if (path.starts_with(GAMEINFO_PATH)) {
			path = gameID + '/' + path.substr(GAMEINFO_PATH.length());
		}
		if (path.ends_with(".") && !path.ends_with("..")) {
			path.pop_back();
		}
		string::normalizeSlashes(path);

		if (path.ends_with(".vpk")) {
			auto fullPath = this->rootPath + '/' + path;

			// Normalize the ending (add _dir if present)
			if (!std::filesystem::exists(fullPath)) {
				auto fullPathWithDir = (std::filesystem::path{fullPath}.parent_path() / std::filesystem::path{fullPath}.stem()).string() + "_dir.vpk";
				if (!std::filesystem::exists(fullPathWithDir)) {
					continue;
				}
				fullPath = fullPathWithDir;
			}

			// Add the VPK search path
			for (const auto& search : searches) {
				if (!this->searchPathVPKs.contains(search)) {
					this->searchPathVPKs[search] = std::vector<std::unique_ptr<PackFile>>{};
				}
				auto packFile = PackFile::open(fullPath);
				if (packFile) {
					this->searchPathVPKs[search].push_back(std::move(packFile));
				}
			}
		} else {
			for (const auto& search : searches) {
				if (!this->searchPathDirs.contains(search)) {
					this->searchPathDirs[search] = {};
				}
				if (path.ends_with("/*")) {
					// Add the glob dir searchpath
					if (const auto globParentPath = this->rootPath + '/' + path.substr(0, path.length() - 2); std::filesystem::exists(globParentPath) && std::filesystem::is_directory(globParentPath)) {
						for (const auto directoryIterator : std::filesystem::directory_iterator{globParentPath, std::filesystem::directory_options::skip_permission_denied}) {
							auto globChildPath = std::filesystem::relative(directoryIterator.path(), this->rootPath).string();
							string::normalizeSlashes(globChildPath);
							this->searchPathDirs[search].push_back(globChildPath);
						}
					}
				} else if (std::filesystem::exists(this->rootPath + '/' + path)) {
					// Add the dir searchpath
					this->searchPathDirs[search].push_back(path);

					if (search == "game") {
						// Add dir/bin to GAMEBIN searchpath
						if (!this->searchPathDirs.contains("gamebin")) {
							this->searchPathDirs["gamebin"] = {};
						}
						this->searchPathDirs["gamebin"].push_back(path + "/bin");

						if (i == 0) {
							// Add dir to MOD searchpath
							if (!this->searchPathDirs.contains("mod")) {
								this->searchPathDirs["mod"] = {};
							}
							this->searchPathDirs["mod"].push_back(path);
						}
					}
				}

				// todo: Add the pakXX_dir VPK searchpath(s) if they exist
			}
		}
	}

	// todo: Add DLCs / update dir / xlsppatch dir if they exist

	// Add EXECUTABLE_PATH if it doesn't exist, point it at "bin/<platform>"; "bin"; ""
	if (!this->searchPathDirs.contains("executable_path")) {
		if (!options.binPlatform.empty() && std::filesystem::exists(std::filesystem::path{this->rootPath} / "bin" / options.binPlatform)) {
			this->searchPathDirs["executable_path"] = {"bin/" + options.binPlatform};
		} else {
			this->searchPathDirs["executable_path"] = {};
		}
		this->searchPathDirs["executable_path"].push_back("bin");
		this->searchPathDirs["executable_path"].push_back("");
	}

	// Add PLATFORM if it doesn't exist, point it at "platform"
	if (!this->searchPathDirs.contains("platform")) {
		this->searchPathDirs["platform"] = {"platform"};
	}

	// Add PLATFORM path to GAME searchpath as well
	if (this->searchPathDirs.contains("game")) {
		bool foundPlatform = false;
		for (const auto& path : this->searchPathDirs["game"]) {
			if (path == "platform") {
				foundPlatform = true;
			}
		}
		if (!foundPlatform) {
			this->searchPathDirs["game"].push_back("platform");
		}
	}

	// Add DEFAULT_WRITE_PATH if it doesn't exist, point it at "<game>"
	if (!this->searchPathDirs.contains("default_write_path")) {
		this->searchPathDirs["default_write_path"] = {gameID};
	}

	// Add LOGDIR if it doesn't exist, point it at "<game>"
	if (!this->searchPathDirs.contains("logdir")) {
		this->searchPathDirs["logdir"] = {gameID};
	}

	// Add CONFIG if it doesn't exist, point it at "platform/config"
	if (!this->searchPathDirs.contains("config")) {
		this->searchPathDirs["config"] = {"platform/config"};
	}
}

const FileSystem::SearchPathMapDir& FileSystem::getSearchPathDirs() const {
	return this->searchPathDirs;
}

FileSystem::SearchPathMapDir& FileSystem::getSearchPathDirs() {
	return this->searchPathDirs;
}

const FileSystem::SearchPathMapVPK& FileSystem::getSearchPathVPKs() const {
	return this->searchPathVPKs;
}

FileSystem::SearchPathMapVPK& FileSystem::getSearchPathVPKs() {
	return this->searchPathVPKs;
}

std::optional<std::vector<std::byte>> FileSystem::read(std::string_view filePath, std::string_view searchPath, bool prioritizeVPKs) const {
	std::string filePathStr = string::toLower(filePath);
	string::normalizeSlashes(filePathStr, true);
	std::string searchPathStr = string::toLower(searchPath);

	const auto checkVPKs = [this, &filePathStr, &searchPathStr]() -> std::optional<std::vector<std::byte>> {
		if (!this->searchPathVPKs.contains(searchPathStr)) {
			return std::nullopt;
		}
		for (const auto& packFile : this->searchPathVPKs.at(searchPathStr)) {
			if (packFile->hasEntry(filePathStr)) {
				return packFile->readEntry(filePathStr);
			}
		}
		return std::nullopt;
	};

	if (prioritizeVPKs) {
		if (auto data = checkVPKs()) {
			return data;
		}
	}

	if (this->searchPathDirs.contains(searchPathStr)) {
		for (const auto& basePath : this->searchPathDirs.at(searchPathStr)) {
			// todo: case insensitivity on Linux
			if (const auto testPath = this->rootPath + '/' + basePath + '/' + filePathStr; std::filesystem::exists(testPath)) {
				return fs::readFileBuffer(testPath);
			}
		}
	}

	if (!prioritizeVPKs) {
		return checkVPKs();
	}
	return std::nullopt;
}

std::optional<std::vector<std::byte>> FileSystem::readForMap(const PackFile* map, std::string_view filePath, std::string_view searchPath, bool prioritizeVPKs) const {
	if (const auto filePathStr = std::string{filePath}; map && map->hasEntry(filePathStr)) {
		return map->readEntry(filePathStr);
	}
	return this->read(filePath, searchPath, prioritizeVPKs);
}
