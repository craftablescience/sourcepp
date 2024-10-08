#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view VPK_VTMB_EXTENSION = ".vpk";

class VPK_VTMB : public PackFile {
public:
	/// Create Vampire: The Masquerade - Bloodlines VPK files
	static std::unique_ptr<PackFile> create(const std::string& path);

	/// Open Vampire: The Masquerade - Bloodlines VPK files
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] std::string getTruncatedFilestem() const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

protected:
	explicit VPK_VTMB(const std::string& fullFilePath_);

	void openNumbered(uint32_t archiveIndex, const std::string& path, const EntryCallback& callback);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	std::vector<uint32_t> knownArchives;
	uint32_t currentArchive = 0;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(VPK_VTMB_EXTENSION, &VPK_VTMB::open);
};

} // namespace vpkpp
