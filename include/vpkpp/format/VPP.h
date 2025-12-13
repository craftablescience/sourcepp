// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr uint32_t VPP_SIGNATURE_LIL = 0x51890ACE;
constexpr uint32_t VPP_SIGNATURE_BIG = 0xCE0A8951;
constexpr uint32_t VPP_ALIGNMENT = 2048;
constexpr std::string_view VPP_EXTENSION = ".vpp";
constexpr std::string_view VPP_EXTENSION_PC = ".vpp_pc";
constexpr std::string_view VPP_EXTENSION_XBOX2 = ".vpp_xbox2";

class VPP : public PackFileReadOnly {
public:
	enum Flags : uint32_t {
		FLAG_NONE       = 0,
		FLAG_COMPRESSED = 1 << 0,
		FLAG_CONDENSED  = 1 << 1
	};

	/// Open a VPP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr std::string_view GUID = "C47C5C4D88AE4032967F8CDC93D6D2E1";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return VPP::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	Flags flags = FLAG_NONE;
	uint32_t entryBaseOffset = 0;
	std::vector<std::byte> uncondensedData;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(VPP_EXTENSION, &VPP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(VPP_EXTENSION_PC, &VPP::open);
	VPKPP_REGISTER_PACKFILE_OPEN(VPP_EXTENSION_XBOX2, &VPP::open);
};
SOURCEPP_BITFLAGS_ENUM(VPP::Flags)

} // namespace vpkpp
