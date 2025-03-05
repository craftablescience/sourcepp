#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

struct ZSTD_DDict_s;

namespace vpkpp {

constexpr uint32_t SPK_SIGNATURE = sourcepp::parser::binary::makeFourCC("SPK\0");
constexpr std::string_view SPK_EXTENSION = ".spk";

class SPK : public PackFileReadOnly {
public:
	/// Create a new SPK file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an SPK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "117C7C2D338F434EA98A13D563FA8280";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return SPK::GUID;
	}

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override;

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	std::unique_ptr<ZSTD_DDict_s, void(*)(void*)> decompressionDict{nullptr, nullptr};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(SPK_EXTENSION, &SPK::open);
};

} // namespace vpkpp
