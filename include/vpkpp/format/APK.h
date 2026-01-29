// ReSharper disable CppRedundantQualifier

#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto APK_SIGNATURE = sourcepp::parser::binary::makeFourCC("W#\0\0");
constexpr std::string_view APK_EXTENSION = ".apk";

class APK : public PackFile {
public:
	/// Create an APK file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an APK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(APK_EXTENSION, &APK::open);
};

} // namespace vpkpp
