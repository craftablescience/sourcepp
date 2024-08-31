#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr auto PCK_SIGNATURE = sourcepp::parser::binary::makeFourCC("GDPC");
constexpr std::string_view PCK_PATH_PREFIX = "res://";
constexpr std::string_view PCK_EXTENSION = ".pck";

class PCK : public PackFile {
protected:
	enum FlagsV2 : uint32_t {
		FLAG_NONE                    = 0,
		FLAG_ENCRYPTED          = 1 << 0,
		FLAG_RELATIVE_FILE_DATA = 1 << 1,
	};

	struct Header {
		uint32_t packVersion;
		uint32_t godotVersionMajor;
		uint32_t godotVersionMinor;
		uint32_t godotVersionPatch;
		FlagsV2 flags; // packVersion >= 2
	};

public:
	/// Create a new PCK file
	static std::unique_ptr<PackFile> create(const std::string& path, uint32_t version = 2, uint32_t godotMajorVersion = 0, uint32_t godotMinorVersion = 0, uint32_t godotPatchVersion = 0);

	/// Open a PCK file (potentially embedded in an executable)
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool isCaseSensitive() const noexcept override {
		return true;
	}

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

	/// Returns 1 for v1, 2 for v2
	[[nodiscard]] uint32_t getVersion() const;

	/// Change the version of the PCK. Valid values are 1 and 2
	void setVersion(uint32_t version);

	[[nodiscard]] std::tuple<uint32_t, uint32_t, uint32_t> getGodotVersion() const;

	void setGodotVersion(uint32_t major, uint32_t minor = 0, uint32_t patch = 0);

protected:
	explicit PCK(const std::string& fullFilePath_);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	Header header{};

	std::size_t startOffset = 0;
	std::size_t dataOffset = 0;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PCK_EXTENSION, &PCK::open);
	VPKPP_REGISTER_PACKFILE_OPEN_EXECUTABLE(&PCK::open);
};

} // namespace vpkpp
