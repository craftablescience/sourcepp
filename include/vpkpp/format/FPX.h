// ReSharper disable CppRedundantQualifier

#pragma once

#include "VPK.h"

namespace vpkpp {

constexpr uint32_t FPX_SIGNATURE = 0x3241ff33;
constexpr std::string_view FPX_DIR_SUFFIX = "_fdr";
constexpr std::string_view FPX_EXTENSION = ".fpx";

class FPX : public VPK {
public:
	/// Create a new directory FPX file - should end in "_dir.fpx"! This is not enforced but STRONGLY recommended
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an FPX file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr std::string_view GUID = "57D4C78A6198489C81D715D42DD21D2F";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return FPX::GUID;
	}

protected:
	using VPK::VPK;

	[[nodiscard]] static std::unique_ptr<PackFile> openInternal(const std::string& path, const EntryCallback& callback = nullptr);

private:
	using VPK::generateKeyPairFiles;
	using VPK::sign;
	using VPK::getVersion;
	using VPK::setVersion;

	VPKPP_REGISTER_PACKFILE_OPEN(FPX_EXTENSION, &FPX::open);
};

} // namespace vpkpp
