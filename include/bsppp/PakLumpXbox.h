#pragma once

#include <vpkpp/format/XZP.h>

#include "PakLump.h"

namespace bsppp {

/// Utility class for vpkpp integration - you need to include this header in a C++ file
/// somewhere in your executable for automatic registration to work! Thanks C++
class PakLumpXbox : public vpkpp::XZP {
public:
	~PakLumpXbox() override;

	/// Open a BSP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "E0B02EF72F044612B80648AEBB92451A";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return PakLump::GUID;
	}

	[[nodiscard]] explicit operator std::string() const override;

protected:
	explicit PakLumpXbox(const std::string& fullFilePath_);

	const std::string tempPakLumpPath;
	uint32_t version{};
	uint32_t mapRevision{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(BSP_EXTENSION, &PakLumpXbox::open);
};

} // namespace bsppp
