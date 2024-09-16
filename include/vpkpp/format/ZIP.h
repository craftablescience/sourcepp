#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view BMZ_EXTENSION  = ".bmz";
constexpr std::string_view BZ2_EXTENSION  = ".bz2";
constexpr std::string_view GZIP_EXTENSION = ".gz";
constexpr std::string_view PK3_EXTENSION  = ".pk3";
constexpr std::string_view PK4_EXTENSION  = ".pk4";
constexpr std::string_view XZ_EXTENSION   = ".xz";
constexpr std::string_view ZIP_EXTENSION  = ".zip";
constexpr std::string_view ZST_EXTENSION  = ".zst";

class ZIP : public PackFile {
public:
	~ZIP() override;

	/// Create a ZIP file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open a ZIP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override;

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	explicit ZIP(const std::string& fullFilePath_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	bool bakeTempZip(const std::string& writeZipPath, BakeOptions options, const EntryCallback& callback);

	bool openZIP(std::string_view path);

	void closeZIP();

	const std::string tempZIPPath;

	void* streamHandle = nullptr;
	bool streamOpen = false;

	void* zipHandle = nullptr;
	bool zipOpen = false;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BMZ_EXTENSION,  &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(BZ2_EXTENSION,  &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(GZIP_EXTENSION, &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(PK3_EXTENSION,  &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(PK4_EXTENSION,  &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(XZ_EXTENSION,   &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(ZIP_EXTENSION,  &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(ZST_EXTENSION,  &ZIP::open);
};

} // namespace vpkpp
