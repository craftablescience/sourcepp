// ReSharper disable CppRedundantQualifier

#pragma once

#include <array>

#include "../PackFile.h"

namespace vpkpp {

constexpr uint32_t FPX_SIGNATURE          = 0x3241ff33;
constexpr std::string_view FPX_DIR_SUFFIX = "_fdr";
constexpr std::string_view FPX_EXTENSION  = ".fpx";

constexpr uint32_t VPK_SIGNATURE          = 0x55aa1234;
constexpr std::string_view VPK_DIR_SUFFIX = "_dir";
constexpr std::string_view VPK_EXTENSION  = ".vpk";

constexpr uint16_t VPK_DIR_ARCHIVE_INDEX = 0x7fff;

/// Maximum preload data size in bytes
constexpr uint16_t VPK_MAX_PRELOAD_BYTES = 1024;

/// Chunk size in bytes (default is 200mb for S1, 1gb for S2)
constexpr uint32_t VPK_DEFAULT_CHUNK_SIZE = 200 * 1024 * 1024;

class VPK : public PackFile {
public:
	enum class Version : uint32_t {
		VALVE_V0   = 0,
		VALVE_V1   = 1,
		VALVE_V2   = 2,
		TI_FPX_V10 = 10,
		PROMOD_V54 = 54,
	};

	enum class SignatureType : int32_t {
		UNKNOWN             = -2,
		LEGACY              = -1,
		WHOLE_FILE          = 0,
		WHOLE_FILE_CHECKSUM = 1,
	};

protected:
	struct HashedChunk {
		/// The archive index of the file
		uint16_t archiveIndex;
		/// The hash type of this entry
		enum class HashType : uint16_t {
			MD5    = 0,
			BLAKE3 = 1,
		} hashType;
		/// The offset in the archive
		uint32_t offset;
		/// The length in bytes
		uint32_t length;
		/// The hash
		std::array<std::byte, 16> hash;
	};

	struct FreedChunk {
		uint64_t offset;
		uint64_t length;
		uint32_t archiveIndex;
	};

public:
	/// Create a new directory VPK file - should end in "_dir.vpk" or "_fdr.fpx"! This is not enforced but STRONGLY recommended
	static std::unique_ptr<PackFile> create(const std::string& path, Version version = Version::VALVE_V2);

	/// Open a VPK file
	[[nodiscard]] static std::unique_ptr<PackFile> open(const std::string& path, const EntryCallback& callback = nullptr);

	[[nodiscard]] constexpr bool hasEntryChecksums() const override {
		return true;
	}

	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override;

	[[nodiscard]] bool hasPackFileChecksum() const override;

	[[nodiscard]] bool verifyPackFileChecksum() const override;

	[[nodiscard]] bool hasPackFileSignature() const override;

	[[nodiscard]] bool verifyPackFileSignature() const override;

	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path_) const override;

	bool removeEntry(const std::string& filename_) override;

	std::size_t removeDirectory(const std::string& dirName_) override;

	bool bake(const std::string& outputDir_ /*= ""*/, BakeOptions options /*= {}*/, const EntryCallback& callback /*= nullptr*/) override;

	[[nodiscard]] std::string getTruncatedFilestem() const override;

	[[nodiscard]] Attribute getSupportedEntryAttributes() const override;

	[[nodiscard]] explicit operator std::string() const override;

	/// Expected signature size in bits for the given signature type
	[[nodiscard]] static constexpr uint16_t getSignatureSize(SignatureType signatureType) {
		switch (signatureType) {
			case SignatureType::UNKNOWN:
				break;
			case SignatureType::LEGACY:
			case SignatureType::WHOLE_FILE:
				return 1024;
			case SignatureType::WHOLE_FILE_CHECKSUM:
				return 4096;
		}
		return 0;
	}

	/// Generate keypair files, which can be used to sign a VPK
	/// Input is a truncated file path, e.g. "/x/y/z/key" or just "key" for the CWD
	/// It will append ".publickey.vdf" and ".privatekey.vdf" to the input and save those files
	static void generateKeyPairFiles(const std::filesystem::path& base, SignatureType signatureType, std::string_view passphrase = "");

	/// Sign the VPK with the given private key KeyValues file. (See below comment)
	bool sign(const std::filesystem::path& filename_, SignatureType signatureType_, std::string_view passphrase = "");

	/// Sign the VPK with the given keypair. Run this after VPK::bake if modifying the VPK file contents
	bool sign(const std::vector<std::byte>& privateKey, const std::vector<std::byte>& publicKey_, SignatureType signatureType_, std::string_view passphrase = "");

	/// Get the VPK version
	[[nodiscard]] Version getVersion() const;

	/// Set the VPK version
	void setVersion(Version version_);

	/// Get the VPK chunk size in bytes (size of generated archives when baking)
	[[nodiscard]] uint32_t getChunkSize() const;

	/// Set the VPK chunk size in bytes (size of generated archives when baking)
	void setChunkSize(uint32_t newChunkSize);

protected:
	using PackFile::PackFile;

	[[nodiscard]] static std::unique_ptr<PackFile> openInternal(const std::string& path, const EntryCallback& callback = nullptr);

	void addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) override;

	[[nodiscard]] bool hasExtendedHeader() const;

	[[nodiscard]] bool hasCompression() const;

	int32_t numArchives = -1;

	Version version = Version::VALVE_V2;
	Version versionModified = Version::VALVE_V2;
	uint32_t headerSize = 0;
	uint32_t treeSize = 0;
	uint32_t fileDataSize = 0;
	uint32_t hashedChunksSize = 0;
	uint32_t otherMD5Size = 0;
	uint32_t signatureSize = 0;
	std::vector<HashedChunk> hashedChunks;
	std::array<std::byte, 16> treeChecksum{};
	std::array<std::byte, 16> hashedChunksChecksum{};
	std::array<std::byte, 16> wholeFileChecksum{};
	SignatureType signatureType = SignatureType::UNKNOWN;
	std::vector<std::byte> publicKey;
	std::vector<std::byte> signature;

	uint32_t currentlyFilledChunkSize = 0;
	uint32_t chunkSize = VPK_DEFAULT_CHUNK_SIZE;
	std::vector<FreedChunk> freedChunks;

private:
	VPKPP_REGISTER_PACKFILE_OPEN(FPX_EXTENSION, &VPK::open);
	VPKPP_REGISTER_PACKFILE_OPEN(VPK_EXTENSION, &VPK::open);
};

} // namespace vpkpp
