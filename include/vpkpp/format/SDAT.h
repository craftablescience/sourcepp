// ReSharper disable CppRedundantQualifier

#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto SDAT_SIGNATURE = sourcepp::parser::binary::makeFourCC("PKG2");
constexpr std::string_view SDAT_EXTENSION = ".sdat";

class SDAT : public PackFile {
public:
	/// Create an SDAT file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an SDAT file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(SDAT_EXTENSION, &SDAT::open);
};

} // namespace vpkpp
