#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <sourcepp/math/Integer.h>
#include <tsl/htrie_map.h>

#include "Attribute.h"
#include "Entry.h"
#include "Options.h"
#include "PackFileType.h"

namespace vpkpp {

// Executable extensions - mostly just for Godot exports
constexpr std::string_view EXECUTABLE_EXTENSION0 = ".exe";    // - Windows
constexpr std::string_view EXECUTABLE_EXTENSION1 = ".bin";    // - Linux + Godot 3 and below (and Generic)
constexpr std::string_view EXECUTABLE_EXTENSION2 = ".x86_64"; //         | Godot 4 (64-bit)

class PackFile {
public:
	/// Accepts the entry's path and metadata
	using EntryCallback = std::function<void(const std::string& path, const Entry& entry)>;

	/// Accepts the entry's path and metadata
	using EntryPredicate = std::function<bool(const std::string& path, const Entry& entry)>;

	using EntryTrie = tsl::htrie_map<char, Entry>;

	PackFile(const PackFile& other) = delete;
	PackFile& operator=(const PackFile& other) = delete;

	PackFile(PackFile&& other) noexcept = default;
	PackFile& operator=(PackFile&& other) noexcept = default;

	virtual ~PackFile() = default;

	/// Open a generic pack file. The parser is selected based on the file extension
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const EntryCallback& callback = nullptr);

	/// Get the file type of the pack file
	[[nodiscard]] PackFileType getType() const;

	/// Get the current options of the pack file
	[[nodiscard]] PackFileOptions getOptions() const;

	/// Returns true if the format has a checksum for each entry
	[[nodiscard]] virtual constexpr bool hasEntryChecksums() const {
		return false;
	}

	/// Verify the checksums of each file, if a file fails the check its path will be added to the vector
	/// If there is no checksum ability in the format, it will return an empty vector
	[[nodiscard]] virtual std::vector<std::string> verifyEntryChecksums() const;

	/// Returns true if the entire file has a checksum
	[[nodiscard]] virtual bool hasPackFileChecksum() const;

	/// Verify the checksum of the entire file, returns true on success
	/// Will return true if there is no checksum ability in the format
	[[nodiscard]] virtual bool verifyPackFileChecksum() const;

	/// Returns true if the file is signed
	[[nodiscard]] virtual bool hasPackFileSignature() const;

	/// Verify the file signature, returns true on success
	/// Will return true if there is no signature ability in the format
	[[nodiscard]] virtual bool verifyPackFileSignature() const;

	/// Does the format support case-sensitive file names?
	[[nodiscard]] virtual constexpr bool isCaseSensitive() const noexcept {
		return false;
	}

	/// Check if an entry exists given the file path
	[[nodiscard]] bool hasEntry(const std::string& path, bool includeUnbaked = true) const;

	/// Try to find an entry given the file path
	[[nodiscard]] std::optional<Entry> findEntry(const std::string& path_, bool includeUnbaked = true) const;

	/// Try to read the entry's data to a bytebuffer
	[[nodiscard]] virtual std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const = 0;

	/// Try to read the entry's data to a string
	[[nodiscard]] std::optional<std::string> readEntryText(const std::string& path) const;

	[[nodiscard]] virtual constexpr bool isReadOnly() const noexcept {
		return false;
	}

	/// Add a new entry from a file path - the first parameter is the path in the PackFile, the second is the path on disk
	void addEntry(const std::string& entryPath, const std::string& filepath, EntryOptions options_);

	/// Add a new entry from a buffer
	void addEntry(const std::string& path, std::vector<std::byte>&& buffer, EntryOptions options_);

	/// Add a new entry from a buffer
	void addEntry(const std::string& path, const std::byte* buffer, uint64_t bufferLen, EntryOptions options_);

	/// Rename an existing entry
	virtual bool renameEntry(const std::string& oldPath_, const std::string& newPath_); // NOLINT(*-use-nodiscard)

	/// Rename an existing directory
	virtual bool renameDirectory(const std::string& oldDir_, const std::string& newDir_); // NOLINT(*-use-nodiscard)

	/// Remove an entry
	virtual bool removeEntry(const std::string& path_);

	/// Remove a directory
	virtual std::size_t removeDirectory(const std::string& dirName_);

	/// If output folder is unspecified, it will overwrite the original
	virtual bool bake(const std::string& outputDir_ /*= ""*/, const EntryCallback& callback /*= nullptr*/) = 0;

	/// Extract the given entry to disk at the given file path
	bool extractEntry(const std::string& entryPath, const std::string& filepath) const; // NOLINT(*-use-nodiscard)

	/// Extract the given directory to disk under the given output directory
	bool extractDirectory(const std::string& dir_, const std::string& outputDir) const; // NOLINT(*-use-nodiscard)

	/// Extract the contents of the pack file to disk at the given directory
	bool extractAll(const std::string& outputDir, bool createUnderPackFileDir = true) const; // NOLINT(*-use-nodiscard)

	/// Extract the contents of the pack file to disk at the given directory - only entries which match the predicate are extracted
	bool extractAll(const std::string& outputDir, const EntryPredicate& predicate, bool stripSharedDirs = true) const; // NOLINT(*-use-nodiscard)

	/// Get entries saved to disk
	[[nodiscard]] const EntryTrie& getBakedEntries() const;

	/// Get entries that have been added but not yet baked
	[[nodiscard]] const EntryTrie& getUnbakedEntries() const;

	/// Get the number of entries in the pack file
	[[nodiscard]] std::size_t getEntryCount(bool includeUnbaked = true) const;

	/// Run a callback for each entry in the pack file
	void runForAllEntries(const EntryCallback& operation, bool includeUnbaked = true) const;

	/// /home/user/pak01_dir.vpk
	[[nodiscard]] std::string_view getFilepath() const;

	/// /home/user/pak01_dir.vpk -> /home/user/pak01
	[[nodiscard]] std::string getTruncatedFilepath() const;

	/// /home/user/pak01_dir.vpk -> pak01_dir.vpk
	[[nodiscard]] std::string getFilename() const;

	/// /home/user/pak01_dir.vpk -> pak01.vpk
	[[nodiscard]] std::string getTruncatedFilename() const;

	/// /home/user/pak01_dir.vpk -> pak01_dir
	[[nodiscard]] std::string getFilestem() const;

	/// /home/user/pak01_dir.vpk -> pak01
	[[nodiscard]] virtual std::string getTruncatedFilestem() const;

	/// Returns a list of supported entry attributes
	/// Mostly for GUI programs that show entries and their metadata in a table ;)
	[[nodiscard]] virtual Attribute getSupportedEntryAttributes() const;

	[[nodiscard]] virtual explicit operator std::string() const;

	/// On Windows, some characters and file names are invalid - this escapes the given entry path
	[[nodiscard]] static std::string escapeEntryPathForWrite(const std::string& path);

	/// Returns a list of supported extensions, e.g. {".vpk", ".bsp"}
	[[nodiscard]] static std::vector<std::string> getSupportedFileTypes();

protected:
	PackFile(std::string fullFilePath_, PackFileOptions options_);

	void runForAllEntriesInternal(const std::function<void(const std::string&, Entry&)>& operation, bool includeUnbaked = true);

	[[nodiscard]] std::vector<std::string> verifyEntryChecksumsUsingCRC32() const;

	virtual void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) = 0;

	[[nodiscard]] std::string getBakeOutputDir(const std::string& outputDir) const;

	void mergeUnbakedEntries();

	void setFullFilePath(const std::string& outputDir);

	[[nodiscard]] std::string cleanEntryPath(const std::string& path) const;

	[[nodiscard]] static Entry createNewEntry();

	[[nodiscard]] static std::optional<std::vector<std::byte>> readUnbakedEntry(const Entry& entry);

	std::string fullFilePath;

	PackFileType type = PackFileType::UNKNOWN;
	PackFileOptions options;

	EntryTrie entries;
	EntryTrie unbakedEntries;

	using FactoryFunction = std::function<std::unique_ptr<PackFile>(const std::string& path, PackFileOptions options, const EntryCallback& callback)>;

	static std::unordered_map<std::string, std::vector<FactoryFunction>>& getOpenExtensionRegistry();

	static const FactoryFunction& registerOpenExtensionForTypeFactory(std::string_view extension, const FactoryFunction& factory);
};

class PackFileReadOnly : public PackFile {
public:
	[[nodiscard]] constexpr bool isReadOnly() const noexcept final {
		return true;
	}

	[[nodiscard]] explicit operator std::string() const override;

protected:
	PackFileReadOnly([[maybe_unused]] [[maybe_unused]] std::string fullFilePath_, PackFileOptions options_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) final;

	bool bake(const std::string& outputDir_ /*= ""*/, const EntryCallback& callback /*= nullptr*/) final;
};

} // namespace vpkpp

#define VPKPP_HELPER_CONCAT_INNER(a, b) a ## b
#define VPKPP_HELPER_CONCAT(a, b) VPKPP_HELPER_CONCAT_INNER(a, b)
#define VPKPP_HELPER_UNIQUE_NAME(base) VPKPP_HELPER_CONCAT(base, __LINE__)

#define VPKPP_REGISTER_PACKFILE_OPEN(extension, function) \
	static inline const FactoryFunction& VPKPP_HELPER_UNIQUE_NAME(packFileOpenTypeFactoryFunction) = PackFile::registerOpenExtensionForTypeFactory(extension, function)

#define VPKPP_REGISTER_PACKFILE_OPEN_EXECUTABLE(function) \
	static inline const FactoryFunction& VPKPP_HELPER_UNIQUE_NAME(packFileOpenExecutable0TypeFactoryFunction) = PackFile::registerOpenExtensionForTypeFactory(vpkpp::EXECUTABLE_EXTENSION0, function); \
	static inline const FactoryFunction& VPKPP_HELPER_UNIQUE_NAME(packFileOpenExecutable1TypeFactoryFunction) = PackFile::registerOpenExtensionForTypeFactory(vpkpp::EXECUTABLE_EXTENSION1, function); \
	static inline const FactoryFunction& VPKPP_HELPER_UNIQUE_NAME(packFileOpenExecutable2TypeFactoryFunction) = PackFile::registerOpenExtensionForTypeFactory(vpkpp::EXECUTABLE_EXTENSION2, function)
