// ReSharper disable CppRedundantQualifier

#pragma once

#include <sourcepp/parser/Binary.h>

#include "../PackFile.h"

namespace vpkpp {

constexpr uint8_t PAK_FILENAME_MAX_SIZE = 56;
constexpr auto PAK_SIGNATURE = sourcepp::parser::binary::makeFourCC("PACK");

constexpr uint8_t PAK_SIN_FILENAME_MAX_SIZE = 120;
constexpr auto PAK_SIN_SIGNATURE = sourcepp::parser::binary::makeFourCC("SPAK");

constexpr uint8_t PAK_HROT_FILENAME_MAX_SIZE = 120;
constexpr auto PAK_HROT_SIGNATURE = sourcepp::parser::binary::makeFourCC("HROT");

constexpr std::string_view PAK_EXTENSION = ".pak";
constexpr std::string_view SIN_EXTENSION = ".sin";

class PAK : public PackFile {
public:
	enum class Type {
		PAK,
		SIN,
		HROT,
	};

	/// Create a PAK file
	static std::unique_ptr<PackFile> create(const std::string& path, Type type = Type::PAK);

	/// Open a PAK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] Type getType() const;

	void setType(Type type_);

protected:
	using PackFile::PackFile;

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	[[nodiscard]] uint32_t getSignature() const;

	[[nodiscard]] uint8_t getFilenameLength() const;

	Type type = Type::PAK;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(PAK_EXTENSION, &PAK::open);
	VPKPP_REGISTER_PACKFILE_OPEN(SIN_EXTENSION, &PAK::open);
};

} // namespace vpkpp
