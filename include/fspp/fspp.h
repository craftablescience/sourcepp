#pragma once

#include <optional>

#include <steampp/steampp.h>

namespace fspp {

struct FileSystemOptions {
	std::string language;
	bool prioritizeVPKs = true;
	bool loadAddonList = false;
	bool useDLCFolders = false;
	bool useUpdate = true;
	bool useXLSPPatch = true;
};

class FileSystem {
public:
	using SearchPathMap = std::unordered_map<std::string, std::vector<std::string>>;

	/**
	 * Creates a FileSystem based on a Steam installation
	 * @param appID The AppID of the base game
	 * @param gameName The name of the directory where gameinfo.txt is located (e.g. "portal2")
	 * @param options FileSystem creation options
	 * @return The created FileSystem if the specified Steam game is installed
	 */
	[[nodiscard]] static std::optional<FileSystem> load(steampp::AppID appID, std::string_view gameName, const FileSystemOptions& options = {});

	/**
	 * Creates a FileSystem based on a local installation
	 * @param gamePath The full path to the directory where gameinfo.txt is located (e.g. "path/to/portal2")
	 * @param options FileSystem creation options
	 * @return The created FileSystem if gameinfo.txt is found
	 */
	[[nodiscard]] static std::optional<FileSystem> load(std::string_view gamePath, const FileSystemOptions& options = {});

	[[nodiscard]] std::vector<std::string> getSearchPaths() const;

	[[nodiscard]] const std::vector<std::string>& getPathsForSearchPath(std::string_view searchPath) const;

	[[nodiscard]] const SearchPathMap& getSearchPathData() const;

	[[nodiscard]] std::optional<std::vector<std::byte>> read(std::string_view filePath, std::string_view searchPath = "GAME") const;

protected:
	explicit FileSystem(std::string_view gamePath, const FileSystemOptions& options = {});

private:
	SearchPathMap searchPaths;
};

} // namespace fspp
