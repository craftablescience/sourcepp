#include <vpkpp/PackFile.h>

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <utility>

#include <FileStream.h>

#include <sourcepp/crypto/CRC32.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

// Need to include this so the compiler will think the automatic registry
// variables in the formats are important enough to initialize :3 (I love C++!)
#include <vpkpp/vpkpp.h>

using namespace sourcepp;
using namespace vpkpp;

namespace {

std::string joinPath(const std::vector<std::string>& list) {
	if (list.empty()) {
		return "";
	}
	std::string result = list.front();
	for (int i = 1; i < list.size(); ++i) {
		result += '/' + list[i];
	}
	return result;
}

std::vector<std::string> splitPath(const std::string& string) {
	std::vector<std::string> result;
	std::stringstream stream{string};
	std::string segment;
	while (std::getline(stream, segment, '/')) {
		result.push_back(segment);
	}
	return result;
}

#ifdef _WIN32

void replace(std::string& line, const std::string& oldString, const std::string& newString) {
	const auto oldSize = oldString.length();
	if (oldSize > line.length()) {
		return;
	}

	const auto newSize = newString.length();
	std::size_t pos = 0;
	while (true) {
		pos = line.find(oldString, pos);
		if (pos == std::string::npos) {
			break;
		}
		if (oldSize == newSize) {
			line.replace(pos, oldSize, newString);
		} else {
			line.erase(pos, oldSize);
			line.insert(pos, newString);
		}
		pos += newSize;
	}
}

void fixFilePathForWindows(std::string& path) {
	// Remove invalid characters
	::replace(path, "<", "_");
	::replace(path, "<", "_");
	::replace(path, ">", "_");
	::replace(path, ":", "_");
	::replace(path, "\"", "_");
	::replace(path, "|", "_");
	::replace(path, "?", "_");
	::replace(path, "*", "_");

	std::filesystem::path filePath{path};
	auto filename = filePath.filename().string();
	auto extension = filePath.extension().string();
	auto stem = filePath.stem().string();
	string::toUpper(stem);

	// Replace bad filenames
	if (stem == "CON" || stem == "PRN" || stem == "AUX" || stem == "NUL") {
		filename = "___" + extension;
	} else if (stem.length() == 4 && stem[3] != '0' && ((stem.starts_with("COM") || stem.starts_with("LPT")))) {
		filename = "___";
		filename += stem[3];
		filename += extension;
	}

	// Files cannot end with a period - weird
	if (extension == ".") {
		filename.pop_back();
		filename += '_';
	}

	path = (filePath.parent_path() / filename).string();
}

#endif

} // namespace

PackFile::PackFile(std::string fullFilePath_)
		: fullFilePath(std::move(fullFilePath_)) {}

std::unique_ptr<PackFile> PackFile::open(const std::string& path, const EntryCallback& callback) {
	auto extension = std::filesystem::path{path}.extension().string();
	string::toLower(extension);
	const auto& registry = PackFile::getOpenExtensionRegistry();
	if (registry.contains(extension)) {
		for (const auto& func : registry.at(extension)) {
			if (auto packFile = func(path, callback)) {
				return packFile;
			}
		}
	}
	return nullptr;
}

std::vector<std::string> PackFile::getOpenableExtensions() {
	std::vector<std::string> out;
	for (const auto& [extension, factoryFunctions] : PackFile::getOpenExtensionRegistry()) {
		if (std::find(out.begin(), out.end(), extension) == out.end()) {
			out.push_back(extension);
		}
	}
	std::sort(out.begin(), out.end());
	return out;
}

std::vector<std::string> PackFile::verifyEntryChecksums() const {
	return {};
}

bool PackFile::hasPackFileChecksum() const {
	return false;
}

bool PackFile::verifyPackFileChecksum() const {
	return true;
}

bool PackFile::hasPackFileSignature() const {
	return false;
}

bool PackFile::verifyPackFileSignature() const {
	return true;
}

bool PackFile::hasEntry(const std::string& path, bool includeUnbaked) const {
	return static_cast<bool>(this->findEntry(path, includeUnbaked));
}

std::optional<Entry> PackFile::findEntry(const std::string& path_, bool includeUnbaked) const {
	auto path = this->cleanEntryPath(path_);
	if (auto it = this->entries.find(path); it != this->entries.end()) {
		return *it;
	}
	if (includeUnbaked) {
		if (auto it = this->unbakedEntries.find(path); it != this->unbakedEntries.end()) {
			return *it;
		}
	}
	return std::nullopt;
}

std::optional<std::string> PackFile::readEntryText(const std::string& path) const {
	auto bytes = this->readEntry(path);
	if (!bytes) {
		return std::nullopt;
	}
	std::string out;
	for (auto byte : *bytes) {
		if (byte == static_cast<std::byte>(0))
			break;
		out += static_cast<char>(byte);
	}
	return out;
}

void PackFile::addEntry(const std::string& entryPath, const std::string& filepath, EntryOptions options) {
	if (this->isReadOnly()) {
		return;
	}

	auto buffer = fs::readFileBuffer(filepath);

	Entry entry{};
	entry.unbaked = true;
	entry.unbakedUsingByteBuffer = false;
	entry.unbakedData = filepath;
	string::normalizeSlashes(std::get<std::string>(entry.unbakedData));

	auto path = this->cleanEntryPath(entryPath);
	this->addEntryInternal(entry, path, buffer, options);
	this->unbakedEntries.emplace(path, entry);
}

void PackFile::addEntry(const std::string& path, std::vector<std::byte>&& buffer, EntryOptions options) {
	if (this->isReadOnly()) {
		return;
	}

	Entry entry{};
	entry.unbaked = true;
	entry.unbakedUsingByteBuffer = true;

	auto path_ = this->cleanEntryPath(path);
	this->addEntryInternal(entry, path_, buffer, options);
	entry.unbakedData = std::move(buffer);
	this->unbakedEntries.emplace(path_, entry);
}

void PackFile::addEntry(const std::string& path, const std::byte* buffer, uint64_t bufferLen, EntryOptions options) {
	std::vector<std::byte> data;
	if (buffer && bufferLen > 0) {
		data.resize(bufferLen);
		std::memcpy(data.data(), buffer, bufferLen);
	}
	this->addEntry(path, std::move(data), options);
}

void PackFile::addDirectory(const std::string& entryBaseDir, const std::string& dir, EntryOptions options) {
	this->addDirectory(entryBaseDir, dir, [options](const std::string& path) {
		return options;
	});
}

void PackFile::addDirectory(const std::string& entryBaseDir_, const std::string& dir, const EntryCreation& creation) {
	if (!std::filesystem::exists(dir) || std::filesystem::status(dir).type() != std::filesystem::file_type::directory) {
		return;
	}

	auto entryBaseDir = this->cleanEntryPath(entryBaseDir_);
	if (!entryBaseDir.empty()) {
		entryBaseDir += '/';
	}
	const auto dirLen = std::filesystem::absolute(dir).string().length() + 1;
	for (const auto& file : std::filesystem::recursive_directory_iterator(dir, std::filesystem::directory_options::skip_permission_denied)) {
		if (!file.is_regular_file()) {
			continue;
		}
		std::string absPath;
		std::string entryPath;
		try {
			absPath = std::filesystem::absolute(file.path()).string();
			string::normalizeSlashes(absPath);
			entryPath = this->cleanEntryPath(entryBaseDir + absPath.substr(dirLen));
		} catch (const std::exception&) {
			continue; // Likely a Unicode error, unsupported filename
		}
		if (entryPath.empty()) {
			continue;
		}
		this->addEntry(entryPath, absPath, creation ? creation(entryPath) : EntryOptions{});
	}
}

bool PackFile::renameEntry(const std::string& oldPath_, const std::string& newPath_) {
	auto oldPath = this->cleanEntryPath(oldPath_);
	auto newPath = this->cleanEntryPath(newPath_);
	if (this->entries.count(oldPath)) {
		// Currently there is no pack file format that relies on file path to access data.
		// If there ever is one, we're in trouble! (Well, no, just override the method.)
		auto entry = this->entries.at(oldPath);
		this->entries.erase(oldPath);
		this->entries.emplace(newPath, entry);
		return true;
	} else if (this->unbakedEntries.count(oldPath)) {
		auto entry = this->unbakedEntries.at(oldPath);
		this->unbakedEntries.erase(oldPath);
		this->unbakedEntries.emplace(newPath, entry);
		return true;
	}
	return false;
}

bool PackFile::renameDirectory(const std::string& oldDir_, const std::string& newDir_) {
	auto oldDir = this->cleanEntryPath(oldDir_) + '/';
	auto newDir = this->cleanEntryPath(newDir_) + '/';

	std::vector<std::string> entryPaths;
	std::vector<std::string> unbakedEntryPaths;
	this->runForAllEntries([&oldDir, &entryPaths, &unbakedEntryPaths](const std::string& path, const Entry& entry) {
		if (path.starts_with(oldDir)) {
			if (entry.unbaked) {
				unbakedEntryPaths.push_back(path);
			} else {
				entryPaths.push_back(path);
			}
		}
	});

	for (const auto& entryPath : entryPaths) {
		auto entry = this->entries.at(entryPath);
		this->entries.erase(entryPath);
		this->entries.emplace(newDir + entryPath.substr(oldDir.length()), entry);
	}
	for (const auto& entryPath : unbakedEntryPaths) {
		auto entry = this->unbakedEntries.at(entryPath);
		this->unbakedEntries.erase(entryPath);
		this->unbakedEntries.emplace(newDir + entryPath.substr(oldDir.length()), entry);
	}
	return !entryPaths.empty() || !unbakedEntryPaths.empty();
}

bool PackFile::removeEntry(const std::string& path_) {
	if (this->isReadOnly()) {
		return false;
	}

	auto path = this->cleanEntryPath(path_);
	if (this->entries.find(path) != this->entries.end()) {
		this->entries.erase(path);
		return true;
	}
	if (this->unbakedEntries.find(path) != this->unbakedEntries.end()) {
		this->unbakedEntries.erase(path);
		return true;
	}
	return false;
}

std::size_t PackFile::removeDirectory(const std::string& dirName_) {
	if (this->isReadOnly()) {
		return false;
	}

	auto dirName = this->cleanEntryPath(dirName_);
	dirName += '/';
	if (dirName == "/") {
		const auto size = this->getEntryCount();
		this->entries.clear();
		this->unbakedEntries.clear();
		return size;
	}
	std::size_t count = this->entries.erase_prefix(dirName);
	count += this->unbakedEntries.erase_prefix(dirName);
	return count;
}

bool PackFile::extractEntry(const std::string& entryPath, const std::string& filepath) const {
	if (filepath.empty()) {
		return false;
	}

	auto data = this->readEntry(entryPath);
	if (!data) {
		return false;
	}

	FileStream stream{filepath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	if (!stream) {
		return false;
	}

	stream.write(*data);
	return true;
}

bool PackFile::extractDirectory(const std::string& dir_, const std::string& outputDir) const {
	auto dir = this->cleanEntryPath(dir_);
	dir += '/';
	if (dir == "/") {
		return this->extractAll(outputDir, false);
	}

	auto outputDirPath = std::filesystem::path{outputDir} / std::filesystem::path{dir}.filename();
	bool noneFailed = true;
	this->runForAllEntries([this, &dir, &outputDirPath, &noneFailed](const std::string& path, const Entry& entry) {
		if (!path.starts_with(dir)) {
			return;
		}

		std::string outputPath = path.substr(dir.length());
#ifdef _WIN32
		::fixFilePathForWindows(outputPath);
#endif
		if (!this->extractEntry(path, (outputDirPath / outputPath).string())) {
			noneFailed = false;
		}
	});
	return noneFailed;
}

bool PackFile::extractAll(const std::string& outputDir, bool createUnderPackFileDir) const {
	if (outputDir.empty()) {
		return false;
	}

	std::filesystem::path outputDirPath{outputDir};
	if (createUnderPackFileDir) {
		outputDirPath /= this->getTruncatedFilestem();
	}
	bool noneFailed = true;
	this->runForAllEntries([this, &outputDirPath, &noneFailed](const std::string& path, const Entry& entry) {
		std::string entryPath = path; // NOLINT(*-unnecessary-copy-initialization)
#ifdef _WIN32
		::fixFilePathForWindows(entryPath);
#endif
		if (!this->extractEntry(path, (outputDirPath / entryPath).string())) {
			noneFailed = false;
		}
	});
	return noneFailed;
}

bool PackFile::extractAll(const std::string& outputDir, const EntryPredicate& predicate, bool stripSharedDirs) const {
	if (outputDir.empty() || !predicate) {
		return false;
	}

	// Get list of paths
	std::vector<std::string> saveEntryPaths;
	this->runForAllEntries([&predicate, &saveEntryPaths](const std::string& path, const Entry& entry) {
		if (predicate(path, entry)) {
			saveEntryPaths.push_back(path);
		}
	});
	if (saveEntryPaths.empty()) {
		return false;
	}

	std::size_t rootDirLen = 0;
	if (stripSharedDirs) {
		// Strip shared directories until we have a root folder
		std::vector<std::string> rootDirList;

		std::vector<std::vector<std::string>> pathSplits;
		pathSplits.reserve(saveEntryPaths.size());
		for (const auto& path : saveEntryPaths) {
			pathSplits.push_back(::splitPath(path));
		}
		while (true) {
			bool allTheSame = true;
			const std::string& first = pathSplits[0][0];
			for (const auto& path : pathSplits) {
				if (path.size() == 1) {
					allTheSame = false;
					break;
				}
				if (path[0] != first) {
					allTheSame = false;
					break;
				}
			}
			if (!allTheSame) {
				break;
			}
			rootDirList.push_back(first);
			for (auto& path : pathSplits) {
				path.erase(path.begin());
			}
		}
		rootDirLen = ::joinPath(rootDirList).length() + 1;
	}

	// Extract
	std::filesystem::path outputDirPath{outputDir};
	bool noneFailed = true;
	for (const auto& path : saveEntryPaths) {
		auto savePath = path;
#ifdef _WIN32
		::fixFilePathForWindows(savePath);
#endif
		if (!this->extractEntry(path, (outputDirPath / savePath.substr(rootDirLen)).string())) {
			noneFailed = false;
		}
	}
	return noneFailed;
}

const PackFile::EntryTrie& PackFile::getBakedEntries() const {
	return this->entries;
}

const PackFile::EntryTrie& PackFile::getUnbakedEntries() const {
	return this->unbakedEntries;
}

std::size_t PackFile::getEntryCount(bool includeUnbaked) const {
	std::size_t count = 0;
	count += this->entries.size();
	if (includeUnbaked) {
		count += this->unbakedEntries.size();
	}
	return count;
}

void PackFile::runForAllEntries(const EntryCallback& operation, bool includeUnbaked) const {
	std::string key;
	for (auto entry = this->entries.cbegin(); entry != this->entries.cend(); ++entry) {
		entry.key(key);
		operation(key, entry.value());
	}
	if (includeUnbaked) {
		for (auto entry = this->unbakedEntries.cbegin(); entry != this->unbakedEntries.cend(); ++entry) {
			entry.key(key);
			operation(key, entry.value());
		}
	}
}

void PackFile::runForAllEntries(const std::string& parentDir, const EntryCallback& operation, bool recursive, bool includeUnbaked) const {
	auto dir = this->cleanEntryPath(parentDir) + '/';

	std::string key;
	for (auto [entry, end] = this->entries.equal_prefix_range(dir); entry != end; ++entry) {
		entry.key(key);
		if (!recursive) {
			auto keyView = std::string_view{key}.substr(dir.length());
			if (std::find(keyView.begin(), keyView.end(), '/') != keyView.end()) {
				continue;
			}
		}
		operation(key, entry.value());
	}
	if (includeUnbaked) {
		for (auto [entry, end] = this->unbakedEntries.equal_prefix_range(dir); entry != end; ++entry) {
			entry.key(key);
			if (!recursive) {
				auto keyView = std::string_view{key}.substr(dir.length());
				if (std::find(keyView.begin(), keyView.end(), '/') != keyView.end()) {
					continue;
				}
			}
			operation(key, entry.value());
		}
	}
}

void PackFile::runForAllEntriesInternal(const std::function<void(const std::string&, Entry&)>& operation, bool includeUnbaked) {
	std::string key;
	for (auto entry = this->entries.begin(); entry != this->entries.end(); ++entry) {
		entry.key(key);
		operation(key, entry.value());
	}
	if (includeUnbaked) {
		for (auto entry = this->unbakedEntries.begin(); entry != this->unbakedEntries.end(); ++entry) {
			entry.key(key);
			operation(key, entry.value());
		}
	}
}

void PackFile::runForAllEntriesInternal(const std::string& parentDir, const std::function<void(const std::string&, Entry&)>& operation, bool recursive, bool includeUnbaked) {
	auto dir = this->cleanEntryPath(parentDir) + '/';

	std::string key;
	for (auto [entry, end] = this->entries.equal_prefix_range(dir); entry != end; ++entry) {
		entry.key(key);
		if (!recursive) {
			auto keyView = std::string_view{key}.substr(dir.length());
			if (std::find(keyView.begin(), keyView.end(), '/') != keyView.end()) {
				continue;
			}
		}
		operation(key, entry.value());
	}
	if (includeUnbaked) {
		for (auto [entry, end] = this->unbakedEntries.equal_prefix_range(dir); entry != end; ++entry) {
			entry.key(key);
			if (!recursive) {
				auto keyView = std::string_view{key}.substr(dir.length());
				if (std::find(keyView.begin(), keyView.end(), '/') != keyView.end()) {
					continue;
				}
			}
			operation(key, entry.value());
		}
	}
}

std::string_view PackFile::getFilepath() const {
	return this->fullFilePath;
}

std::string PackFile::getTruncatedFilepath() const {
	return (std::filesystem::path{this->fullFilePath}.parent_path() / this->getTruncatedFilestem()).string();
}

std::string PackFile::getFilename() const {
	return std::filesystem::path{this->fullFilePath}.filename().string();
}

std::string PackFile::getTruncatedFilename() const {
	const std::filesystem::path path{this->fullFilePath};
	return this->getTruncatedFilestem() + path.extension().string();
}

std::string PackFile::getFilestem() const {
	return std::filesystem::path{this->fullFilePath}.stem().string();
}

std::string PackFile::getTruncatedFilestem() const {
	return this->getFilestem();
}

Attribute PackFile::getSupportedEntryAttributes() const {
	return Attribute::NONE;
}

PackFile::operator std::string() const {
	return this->getTruncatedFilename();
}

std::string PackFile::escapeEntryPathForWrite(const std::string& path) {
#ifdef _WIN32
	auto copy = path;
	::fixFilePathForWindows(copy);
	return copy;
#else
	return path;
#endif
}

std::vector<std::string> PackFile::verifyEntryChecksumsUsingCRC32() const {
	std::vector<std::string> out;
	this->runForAllEntries([this, &out](const std::string& path, const Entry& entry) {
		if (!entry.crc32) {
			return;
		}
		auto data = this->readEntry(path);
		if (!data || crypto::computeCRC32(*data) != entry.crc32) {
			out.push_back(path);
		}
	}, false); // Don't include unbaked since we probably calculate those manually on write
	return out;
}

std::string PackFile::getBakeOutputDir(const std::string& outputDir) const {
	std::string out = outputDir;
	if (!out.empty()) {
		string::normalizeSlashes(out, false);
	} else {
		out = this->fullFilePath;
		auto lastSlash = out.rfind('/');
		if (lastSlash != std::string::npos) {
			out = this->getFilepath().substr(0, lastSlash);
		} else {
			out = ".";
		}
	}
	return out;
}

void PackFile::mergeUnbakedEntries() {
	std::string key;
	for (auto entry = this->unbakedEntries.begin(); entry != this->unbakedEntries.end(); ++entry) {
		entry.key(key);

		entry->unbaked = false;

		// Clear any data that might be stored in it
		entry->unbakedUsingByteBuffer = false;
		entry->unbakedData = "";

		this->entries.insert(key, *entry);
	}
	this->unbakedEntries.clear();
}

void PackFile::setFullFilePath(const std::string& outputDir) {
	// Assumes PackFile::getBakeOutputDir is the input for outputDir
	this->fullFilePath = outputDir + '/' + this->getFilename();
}

std::string PackFile::cleanEntryPath(const std::string& path) const {
	auto path_ = path;
	string::normalizeSlashes(path_, true);
	if (!this->isCaseSensitive()) {
		string::toLower(path_);
	}
	return path_;
}

Entry PackFile::createNewEntry() {
	return {};
}

std::optional<std::vector<std::byte>> PackFile::readUnbakedEntry(const Entry& entry) {
	if (!entry.unbaked) {
		return std::nullopt;
	}

	// Get the stored data
	std::vector<std::byte> unbakedData;
	if (entry.unbakedUsingByteBuffer) {
		unbakedData = std::get<std::vector<std::byte>>(entry.unbakedData);
	} else {
		unbakedData = fs::readFileBuffer(std::get<std::string>(entry.unbakedData));
	}
	return unbakedData;
}

std::unordered_map<std::string, std::vector<PackFile::OpenFactoryFunction>>& PackFile::getOpenExtensionRegistry() {
	static std::unordered_map<std::string, std::vector<PackFile::OpenFactoryFunction>> extensionRegistry;
	return extensionRegistry;
}

const PackFile::OpenFactoryFunction& PackFile::registerOpenExtensionForTypeFactory(std::string_view extension, const OpenFactoryFunction& factory) {
	std::string extensionStr{extension};
	auto& registry = PackFile::getOpenExtensionRegistry();
	if (!registry.contains(extensionStr)) {
		registry[extensionStr] = {};
	}
	registry[extensionStr].push_back(factory);
	return factory;
}

PackFileReadOnly::PackFileReadOnly(const std::string& fullFilePath_)
		: PackFile(fullFilePath_) {}

PackFileReadOnly::operator std::string() const {
	return PackFile::operator std::string() + " (Read-Only)";
}

void PackFileReadOnly::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	// Stubbed
}

bool PackFileReadOnly::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	return false; // Stubbed
}
