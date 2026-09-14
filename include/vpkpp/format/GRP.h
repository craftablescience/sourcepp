// ReSharper disable CppRedundantQualifier

#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr int8_t GRP_FILENAME_MAX_SIZE = 12;
constexpr std::string_view GRP_SIGNATURE = "KenSilverman";
constexpr std::string_view GRP_EXTENSION = ".grp";

class GRP : public PackFile {
public:
	/// Create a GRP file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open a GRP file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(GRP_EXTENSION, &GRP::open);
};

} // namespace vpkpp
