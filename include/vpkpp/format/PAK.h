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
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const Callback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const Entry& entry) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, const Callback& callback /*= nullptr*/) override;

	[[nodiscard]] std::vector<Attribute> getSupportedEntryAttributes() const override;

protected:
	PAK(const std::string& fullFilePath_, PackFileOptions options_);

	Entry& addEntryInternal(Entry& entry, const std::string& filename_, std::vector<std::byte>& buffer, EntryOptions options_) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PAK_EXTENSION, &PAK::open);
};

} // namespace vpkpp
