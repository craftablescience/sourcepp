// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view HOG_SIGNATURE = "DHF";
constexpr std::string_view HOG_EXTENSION = ".hog";

class HOG : public PackFileReadOnly {
public:
	/// Open a HOG file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(HOG_EXTENSION, &HOG::open);
};

} // namespace vpkpp
