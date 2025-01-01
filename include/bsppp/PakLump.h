#pragma once

#include <array>

#include <vpkpp/format/ZIP.h>

namespace bsppp {

constexpr std::string_view BSP_EXTENSION = ".bsp";

/// Utility class for vpkpp integration - you need to include this header in a C++ file
/// somewhere in your executable for automatic registration to work! Thanks C++
class PakLump : public vpkpp::ZIP {
public:
	~PakLump() override;

	/// Open a BSP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "DEB4270A89F443EF8A8F7FE26F6F308E";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return PakLump::GUID;
	}

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return false;
	}

	bool bake(const std::string& outputDir_ /*= ""*/, vpkpp::BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] explicit operator std::string() const override;

protected:
	explicit PakLump(const std::string& fullFilePath_);

	const std::string tempPakLumpPath;
	uint32_t version{};
	uint32_t mapRevision{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BSP_EXTENSION, &PakLump::open);
};

} // namespace bsppp
