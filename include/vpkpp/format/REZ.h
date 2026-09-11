// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view REZ_EXTENSION = ".rez";

class REZ : public PackFileReadOnly {
public:
	/// Open a REZ file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] std::string_view getFileType() const;

	[[nodiscard]] std::string_view getUserTitle() const;

	[[nodiscard]] uint32_t getVersion() const;

protected:
	using PackFileReadOnly::PackFileReadOnly;

	std::string fileType;
	std::string userTitle;
	uint32_t version;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(REZ_EXTENSION, &REZ::open);
};

} // namespace vpkpp
