#pragma once

#include <array>

#include <bsppp/bsppp.h>

#include "ZIP.h"

namespace vpkpp {

using bsppp::BSP_SIGNATURE;
constexpr std::string_view BSP_EXTENSION = ".bsp";

class BSP : public ZIP, private bsppp::BSP {
public:
	/// Open a BSP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return false;
	}

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] explicit operator std::string() const override;

protected:
	explicit BSP(const std::string& fullFilePath_);

	const std::string tempBSPPakLumpPath;

private:
	using bsppp::BSP::operator bool;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BSP_EXTENSION, &BSP::open);
};

} // namespace vpkpp
