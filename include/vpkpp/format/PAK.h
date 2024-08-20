#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr int8_t PAK_FILENAME_MAX_SIZE = 56;
constexpr auto PAK_SIGNATURE = sourcepp::parser::binary::makeFourCC("PACK");
constexpr std::string_view PAK_EXTENSION = ".pak";

class PAK : public PackFile {
public:
	/// Open a PAK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	PAK(const std::string& fullFilePath_, PackFileOptions options_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options_) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PAK_EXTENSION, &PAK::open);
};

} // namespace vpkpp
