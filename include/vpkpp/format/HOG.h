#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view HOG_SIGNATURE = "DHF";
constexpr std::string_view HOG_EXTENSION = ".hog";

class HOG : public PackFileReadOnly {
public:
	/// Open a HOG file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "FDE9941424FF4EC1BC4C90A7DA52AF87";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return HOG::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(HOG_EXTENSION, &HOG::open);
};

} // namespace vpkpp
