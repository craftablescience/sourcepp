#pragma once

#include <optional>

#include <steampp/steampp.h>
#include <vpkpp/vpkpp.h>

namespace fspp {

#if defined(_WIN32)
	constexpr std::string_view DEFAULT_PLATFORM = "win64";
#elif defined(__APPLE__)
	constexpr std::string_view DEFAULT_PLATFORM = "osx64";
#elif defined(__linux__)
	constexpr std::string_view DEFAULT_PLATFORM = "linux64";
#else
	#warning "Unknown platform! Leaving the default platform blank..."
	constexpr std::string_view DEFAULT_PLATFORM = "";
#endif

struct FileSystemOptions {
	std::string binPlatform{DEFAULT_PLATFORM};
	//std::string language{};       // todo: add a <path>_<language> dir or <path>_<language>_dir.vpk for each GAME path
	//bool loadPakXXVPKs = true;    // todo: load pakXX_dir.vpk for each dir path
	//bool loadSteamMounts = true;  // todo: cfg/mounts.kv, the mounts block in gameinfo (Strata)
	//bool loadAddonList = false;   // todo: addonlist.txt (L4D2), addonlist.kv3 (Strata)
	//bool useDLCFolders = true;    // todo: dlc1, dlc2, etc.
	//bool useUpdate = true;        // todo: mount update folder on GAME/MOD with highest priority
	//bool useXLSPPatch = true;     // todo: mount xlsppatch folder on GAME/MOD with highester priority
};

class FileSystem {
public:
	using SearchPathMapDir = std::unordered_map<std::string, std::vector<std::string>>;
	using SearchPathMapVPK = std::unordered_map<std::string, std::vector<std::unique_ptr<vpkpp::PackFile>>>;

	/**
	 * Creates a FileSystem based on a Steam installation
	 * @param appID The AppID of the base game
	 * @param gameID The name of the directory where gameinfo.txt is located (e.g. "portal2")
	 * @param options FileSystem creation options
	 * @return The created FileSystem if the specified Steam game is installed
	 */
	[[nodiscard]] static std::optional<FileSystem> load(steampp::AppID appID, std::string_view gameID, const FileSystemOptions& options = {});

	/**
	 * Creates a FileSystem based on a local installation
	 * @param gamePath The full path to the directory where gameinfo.txt is located (e.g. "path/to/portal2")
	 * @param options FileSystem creation options
	 * @return The created FileSystem if gameinfo.txt is found
	 */
	[[nodiscard]] static std::optional<FileSystem> load(std::string_view gamePath, const FileSystemOptions& options = {});

	[[nodiscard]] const SearchPathMapDir& getSearchPathDirs() const;

	[[nodiscard]] SearchPathMapDir& getSearchPathDirs();

	[[nodiscard]] const SearchPathMapVPK& getSearchPathVPKs() const;

	[[nodiscard]] SearchPathMapVPK& getSearchPathVPKs();

	[[nodiscard]] std::optional<std::vector<std::byte>> read(std::string_view filePath, std::string_view searchPath = "GAME", bool prioritizeVPKs = true) const;

	[[nodiscard]] std::optional<std::vector<std::byte>> readForMap(const vpkpp::PackFile* map, std::string_view filePath, std::string_view searchPath = "GAME", bool prioritizeVPKs = true) const;

protected:
	explicit FileSystem(std::string_view gamePath, const FileSystemOptions& options = {});

private:
	std::string rootPath;
	SearchPathMapDir searchPathDirs;
	SearchPathMapVPK searchPathVPKs;
};

} // namespace fspp
