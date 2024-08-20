#pragma once

#include <vpkpp/PackFile.h>

/*
 * --- Example Pack File Implementation ---
 *
 * This code is a template for adding a new file format to vpkpp. Copy these two files and follow the comments!
 *
 * Any methods marked as "[OPTIONAL]" can be deleted if the file format does not support them.
 *
 * Note that if you are writing a read-only parser, you will need to make the following deviations:
 * - Inherit from PackFileReadOnly instead of PackFile
 * - Don't implement the bake and addEntryInternal methods (marked with "[WRITE]")
 */

namespace vpkpp {

// Define the accepted extension(s) as constant(s)
constexpr std::string_view EXAMPLE_EXTENSION = ".example";

// All file formats need a static open method, and need to derive four methods at minimum from PackFile
class EXAMPLE : public PackFile {
public:
	// Always return a unique_ptr to PackFile so it has a uniform return type
	// If your type needs any new options, add them to PackFileOptions - it was the cleanest way to do it without messing with variants or std::any
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const EntryCallback& callback = nullptr);

	// [OPTIONAL] Implement this and return true if your file format is case-sensitive
	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return PackFile::isCaseSensitive();
	}

	// Returns the raw entry data
	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	// [WRITE] Save any changes made to the opened file(s)
	bool bake(const std::string& outputDir_ /*= ""*/, const EntryCallback& callback /*= nullptr*/) override;

	// [OPTIONAL] Returns any attributes your file format's entries have (refer to the other file formats for more info)
	[[nodiscard]] Attribute getSupportedEntryAttributes() const override {
		return PackFile::getSupportedEntryAttributes();
	}

	// [OPTIONAL] Add any custom file info here (refer to the other file formats for how to structure this)
	[[nodiscard]] explicit operator std::string() const override {
		return PackFile::operator std::string();
	}

protected:
	EXAMPLE(const std::string& fullFilePath_, PackFileOptions options_);

	// [WRITE] Adds a new entry from either a filename or a buffer
	// Again, if your type needs any new options specific to entries, add them to EntryOptions
	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) override;

private:
	// Finally, register the open method with the extension
	// Remember since C++ is STUPID you need to add this header to PackFile.cpp as well, or this will get optimized away
	VPKPP_REGISTER_PACKFILE_OPEN(EXAMPLE_EXTENSION, &EXAMPLE::open);
};

} // namespace vpkpp
