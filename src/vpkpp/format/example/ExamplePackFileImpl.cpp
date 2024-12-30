#include "ExamplePackFileImpl.h"

#include <filesystem>
#include <tuple>

#include <sourcepp/fs/FS.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> EXAMPLE::open(const std::string& path, const EntryCallback& callback) {
	// Check if the file exists
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	// Create the pack file
	auto* example = new EXAMPLE{path};
	auto packFile = std::unique_ptr<PackFile>(example);

	// Here is where you add entries to the entries member variable
	// It's a map between a directory and a vector of entries
	// Every time an entry is added, the callback should be called if the callback exists
	std::vector<std::string> samplePaths{
		{"a/b/c/skibidi_toilet.png"},
		{"d/c/boykisser.mdl"},
		{"megamind.txt"},
	};
	for (const auto& entryPath_ : samplePaths) {
		// The path needs to be normalized, and respect case sensitivity
		auto entryPath = example->cleanEntryPath(entryPath_);

		// Use the createNewEntry function to avoid Entry having to friend every single damn class
		Entry entry = createNewEntry();

		// The length should be the full uncompressed length of the file data in bytes
		entry.length = 42;

		// This is the CRC32 of the file - a helper function to compute it is in <sourcepp/crypto/CRC32.h>
		// This can also be omitted if unused, 0 is the default
		entry.crc32 = 0;

		// Add the entry to the entries trie
		example->entries.emplace(entryPath, entry);

		// Call the callback
		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> EXAMPLE::readEntry(const std::string& path_) const {
	// Include this code verbatim
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// Use the contents of the entry to access the file data and return it
	// Return std::nullopt if there was an error during any step of this process - not an empty buffer!
	return std::nullopt;
}

void EXAMPLE::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	// Initialize the entry - set the entry properties just like in EXAMPLE::open
	entry.length = 0;
	// ...
}

bool EXAMPLE::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder (include this verbatim)
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Loop over all entries and save them
	this->runForAllEntries([&callback](const std::string& path, const Entry& entry) {
		auto binData = this->readEntry(path);
		if (!binData) {
			return;
		}

		// Write data here
		// ...

		// Call the callback
		if (callback) {
			callback(path, entry);
		}
	});

	// Call this when all the entries have been written to disk
	this->mergeUnbakedEntries();

	// Include this verbatim at the end of the function
	PackFile::setFullFilePath(outputDir);
	// Return false before this if it encounters an error
	return true;
}
