// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view OL_SIGNATURE = "Worldcraft Prefab Library\x0d\x0a\x1a";
constexpr std::string_view OL_EXTENSION = ".ol";

class OL : public PackFileReadOnly {
public:
	/// Open an OL file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool isCaseSensitive() const override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] const std::string& getNotes() const;

	[[nodiscard]] std::optional<std::string> getEntryNotes(const std::string& path) const;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	std::string notes;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(OL_EXTENSION, &OL::open);
};

} // namespace vpkpp
