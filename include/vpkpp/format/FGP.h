// ReSharper disable CppRedundantQualifier

#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto FGP_SIGNATURE = sourcepp::parser::binary::makeFourCC("FGP\0");
constexpr std::string_view FGP_EXTENSION = ".grp";

constexpr std::string_view FGP_HASHED_FILEPATH_PREFIX = "__hashed__/";
constexpr auto FGP_SOURCEPP_FILENAMES_SIGNATURE = sourcepp::parser::binary::makeEightCC("PPECRUOS");

class FGP : public PackFile {
public:
	/// Create an FGP file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an FGP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool renameEntry(const std::string& oldPath, const std::string& newPath) override;

	bool renameDirectory(const std::string& oldDir, const std::string& newDir) override;

	bool removeEntry(const std::string& path) override;

	std::size_t removeDirectory(const std::string& dirName) override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

	[[nodiscard]] std::string getLoadingScreenFilePath() const;

	void setLoadingScreenFilePath(const std::string& path);

	[[nodiscard]] static uint32_t hashFilePath(const std::string& filepath);

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	static uint32_t getHeaderSize(uint32_t version, uint32_t fileCount);

	uint32_t version = 0;
	std::string loadingScreenPath;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(FGP_EXTENSION, &FGP::open);
};

} // namespace vpkpp
