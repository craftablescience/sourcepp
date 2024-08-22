#pragma once

#include "../PackFile.h"

namespace vpkpp {

constexpr std::string_view GCF_EXTENSION = ".gcf";

class GCF : public PackFileReadOnly {
protected:
#pragma pack(push, 1)
	struct Header {
		uint32_t dummy1;
		uint32_t dummy2;
		uint32_t gcfversion;
		uint32_t appid;
		uint32_t appversion;
		uint32_t dummy3;
		uint32_t dummy4;
		uint32_t filesize;
		uint32_t blocksize;
		uint32_t blockcount;
		uint32_t dummy5;
	};

	// second header with info about blocks inside the file
	// appears only ONCE not before every block!
	struct BlockHeader {
		uint32_t count;
		uint32_t used;
		uint32_t dummy1;
		uint32_t dummy2;
		uint32_t dummy3;
		uint32_t dummy4;
		uint32_t dummy5;
		uint32_t checksum;
	};

	struct Block {
		uint32_t entry_type;
		uint32_t file_data_offset;
		uint32_t file_data_size;
		uint32_t first_data_block_index;
		uint32_t next_block_entry_index;
		uint32_t prev_block_entry_index;
		uint32_t dir_index;
	};

	struct DirectoryHeader {
		uint32_t dummy1;
		uint32_t cacheid;
		uint32_t gcfversion;
		uint32_t itemcount;
		uint32_t filecount;
		uint32_t dummy2;
		uint32_t dirsize;
		uint32_t namesize;
		uint32_t info1count;
		uint32_t copycount;
		uint32_t localcount;
		uint32_t dummy3;
		uint32_t dummy4;
		uint32_t checksum;
	};

	struct DirectoryEntry {
		uint32_t nameoffset;
		uint32_t itemsize;
		uint32_t fileid;
		uint32_t dirtype;
		uint32_t parentindex;
		uint32_t nextindex;
		uint32_t firstindex;
	};

	struct DirectoryEntry2 {
		DirectoryEntry entry_real{};
		std::string filename;
	};

	struct DirectoryMapHeader {
		uint32_t dummy1;
		uint32_t dummy2;
	};

	struct DirectoryMapEntry {
		uint32_t firstblockindex;
	};

	struct DataBlockHeader {
		uint32_t appversion;
		uint32_t blockcount;
		uint32_t blocksize;
		uint32_t firstblockoffset;
		uint32_t used;
		uint32_t checksum;
	};

	struct ChecksumMapHeader {
		uint32_t dummy1;
		uint32_t dummy2;
		uint32_t item_count;
		uint32_t checksum_count;
	};

	struct ChecksumMapEntry {
		uint32_t count;
		uint32_t firstindex;
	};
#pragma pack(pop)

public:
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override;

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

protected:
	explicit GCF(const std::string& fullFilePath_);

	Header header{};
	BlockHeader blockheader{};
	std::vector<Block> blockdata{};
	std::vector<uint32_t> fragmap{};
	DirectoryHeader dirheader{};
	//std::vector<DirectoryEntry2> direntries{};
	std::vector<DirectoryMapEntry> dirmap_entries{};
	DataBlockHeader datablockheader{};
	std::vector<ChecksumMapEntry> chksum_map{};
	std::vector<uint32_t> checksums{};

private:
	VPKPP_REGISTER_PACKFILE_OPEN(GCF_EXTENSION, &GCF::open);
};

} // namespace vpkpp
