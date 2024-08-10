#pragma once

#include <array>
#include <tuple>

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view VPK_VTMB_EXTENSION = ".vpk";

class VPK_VTMB : public PackFile {
public:
	/// Open Vampire: The Masquerade - Bloodlines VPK files
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, PackFileOptions options = {}, const Callback& callback = nullptr);

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const Entry& entry) const override;

	bool bake(const std::string& outputDir_ /*= ""*/, const Callback& callback /*= nullptr*/) override;

	[[nodiscard]] std::string getTruncatedFilestem() const override;

	[[nodiscard]] std::vector<Attribute> getSupportedEntryAttributes() const override;

protected:
	VPK_VTMB(const std::string& fullFilePath_, PackFileOptions options_);

	void openNumbered(uint16_t archiveIndex, const std::string& path, const Callback& callback);

	Entry& addEntryInternal(Entry& entry, const std::string& filename_, std::vector<std::byte>& buffer, EntryOptions options_) override;

	std::vector<uint16_t> knownArchives;
	uint16_t currentArchive = 0;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(VPK_VTMB_EXTENSION, &VPK_VTMB::open);
};

} // namespace vpkpp
