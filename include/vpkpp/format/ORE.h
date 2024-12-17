#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view ORE_EXTENSION = ".ore";

class ORE : public PackFileReadOnly {
public:
	/// Create an ORE file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open an ORE file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	//bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	explicit ORE(const std::string& fullFilePath_);

	//void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(ORE_EXTENSION, &ORE::open);
};

} // namespace vpkpp
