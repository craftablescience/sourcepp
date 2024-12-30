#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto GRP_SIGNATURE = sourcepp::parser::binary::makeFourCC("FGP\0");
constexpr std::string_view GRP_EXTENSION = ".grp";

class GRP : public PackFileReadOnly {
public:
	/// Open a GRP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	explicit GRP(const std::string& fullFilePath_);

	uint32_t version = 0;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(GRP_EXTENSION, &GRP::open);
};

} // namespace vpkpp
