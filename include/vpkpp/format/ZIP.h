#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view BMZ_EXTENSION = ".bmz";
constexpr std::string_view ZIP_EXTENSION = ".zip";

class ZIP : public PackFile {
public:
	~ZIP() override;

	/// Open a ZIP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override;

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] std::vector<Attribute> getSupportedEntryAttributes() const override;

#ifdef VPKPP_ZIP_COMPRESSION
	[[nodiscard]] uint16_t getCompressionMethod() const;

	void setCompressionMethod(uint16_t compressionMethod);
#endif

protected:
	ZIP(const std::string& fullFilePath_, PackFileOptions options_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) override;

	bool bakeTempZip(const std::string& writeZipPath, const EntryCallback& callback);

	bool openZIP(std::string_view path);

	void closeZIP();

	const std::string tempZIPPath;

	void* streamHandle = nullptr;
	bool streamOpen = false;

	void* zipHandle = nullptr;
	bool zipOpen = false;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BMZ_EXTENSION, &ZIP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(ZIP_EXTENSION, &ZIP::open);
};

} // namespace vpkpp
