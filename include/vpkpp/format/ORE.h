// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view ORE_EXTENSION = ".ore";

class ORE : public PackFileReadOnly {
public:
	/// Create an ORE file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an ORE file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr std::string_view GUID = "E7E541C05CFE4934B1CDA931EF2E1D99";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return ORE::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFileReadOnly::PackFileReadOnly;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(ORE_EXTENSION, &ORE::open);
};

} // namespace vpkpp
