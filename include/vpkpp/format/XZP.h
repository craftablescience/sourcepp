#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto XZP_HEADER_SIGNATURE = sourcepp::parser::binary::makeFourCC("piZx");
constexpr auto XZP_FOOTER_SIGNATURE = sourcepp::parser::binary::makeFourCC("tFzX");
constexpr std::string_view XZP_EXTENSION = ".xzp";

class XZP : public PackFileReadOnly {
public:
	/// Open an XZP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "A682CF9BCA0A4980A920B5C00C8E0945";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return XZP::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(XZP_EXTENSION, &XZP::open);
};

} // namespace vpkpp
