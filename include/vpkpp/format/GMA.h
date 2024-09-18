#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto GMA_SIGNATURE = sourcepp::parser::binary::makeFourCC("GMAD");
constexpr std::string_view GMA_EXTENSION = ".gma";

class GMA : public PackFile {
protected:
	struct Header {
		int32_t signature;
		uint8_t version;
		uint64_t steamID;
		uint64_t timestamp;
		std::string requiredContent;
		std::string addonName;
		std::string addonDescription;
		std::string addonAuthor;
		int32_t addonVersion;
	};

public:
	/// Open a GMA file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() override;

	[[nodiscard]] bool hasPackFileChecksum() const override;

	[[nodiscard]] bool verifyPackFileChecksum() override;

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

protected:
	explicit GMA(const std::string& fullFilePath_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	Header header{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(GMA_EXTENSION, &GMA::open);
};

} // namespace vpkpp
