#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr int8_t PAK_FILENAME_MAX_SIZE = 56;
constexpr auto PAK_SIGNATURE = sourcepp::parser::binary::makeFourCC("PACK");
constexpr std::string_view PAK_EXTENSION = ".pak";

class PAK : public PackFile {
public:
	/// Create a PAK file
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open a PAK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	static constexpr inline std::string_view GUID = "C282210FE64D46D1AE364D7E8E925542";

	[[nodiscard]] constexpr std::string_view getGUID() const override {
		return PAK::GUID;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PAK_EXTENSION, &PAK::open);
};

} // namespace vpkpp
