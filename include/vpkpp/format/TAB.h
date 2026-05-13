// ReSharper disable CppRedundantQualifier

#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view TAB_EXTENSION = ".tab";

constexpr auto TAB_FILENAME_MAX_SIZE = 128;
constexpr std::string_view TAB_HASHED_FILEPATH_PREFIX = "__hashed__/";

constexpr std::string_view ARC_EXTENSION = ".arc";

/// Chunk size in bytes (1gb)
constexpr uint32_t ARC_CHUNK_SIZE = 1024 * 1024 * 1024;

class TAB : public PackFile {
public:
	enum class Version {
		JC1_LE,
		JC1_BE,
	};

	/// Create a TAB file
	static std::unique_ptr<PackFile> create(const std::string& path, Version version = Version::JC1_LE, uint32_t sectorSize = 2048);

	/// Open a TAB file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

	[[nodiscard]] Version getVersion() const;

	void setVersion(Version version_);

	[[nodiscard]] uint32_t getSectorSize() const;

	void setSectorSize(uint32_t sectorSize_);

	[[nodiscard]] static uint32_t hashFilePath(const std::string& filepath);

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	Version version = Version::JC1_LE;
	uint32_t sectorSize = 0;
	uint32_t numArchives = 0;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(TAB_EXTENSION, &TAB::open);
};

} // namespace vpkpp
