#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr uint32_t VPP_SIGNATURE_LIL = 0x51890ACE;
constexpr uint32_t VPP_SIGNATURE_BIG = 0xCE0A8951;
constexpr uint32_t VPP_ALIGNMENT = 2048;
constexpr std::string_view VPP_EXTENSION = ".vpp";

class VPP : public PackFileReadOnly {
public:
	/// Open a VPP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "C47C5C4D88AE4032967F8CDC93D6D2E1";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return VPP::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(VPP_EXTENSION, &VPP::open);
};

} // namespace vpkpp
