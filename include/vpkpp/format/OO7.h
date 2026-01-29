// ReSharper disable CppRedundantQualifier

#pragma once

#include <array>

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view OO7_EXTENSION = ".007";

class OO7 : public PackFileReadOnly {
public:
	/// Open a 007 file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] bool hasPackFileChecksum() const override;

	[[nodiscard]] bool verifyPackFileChecksum() const override;

	[[nodiscard]] constexpr bool isCaseSensitive() const override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	uint32_t majorVersion{};
	uint32_t minorVersion{};
	bool hasChecksum = false;
	std::array<std::byte, 16> checksum{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(OO7_EXTENSION, &OO7::open);
};

} // namespace vpkpp
