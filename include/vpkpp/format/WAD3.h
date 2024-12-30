#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr int8_t WAD3_FILENAME_MAX_SIZE = 16;
constexpr auto WAD3_SIGNATURE = sourcepp::parser::binary::makeFourCC("WAD3");
constexpr std::string_view WAD3_EXTENSION = ".wad";

/**
 * Valve GoldSrc WAD3 file
 */
class WAD3 : public PackFile {
public:
	/// Create a WAD3 file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open a WAD3 file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "1B4D626A278F47B9B2D4ADB244218B03";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return WAD3::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(WAD3_EXTENSION, &WAD3::open);
};

} // namespace vpkpp
