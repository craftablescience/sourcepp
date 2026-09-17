// ReSharper disable CppRedundantQualifier

/* TODO
 *
 * - Do a pass over VPK::bake
 * - Add method to verify hashed chunks and call it in VPK::verifyPackFileChecksum
 * - Test VPK::verifyPackFileSignature on a CS2 VPK
 * - Implement new signing method in VPK::sign
 * - Properly generate keypair files that use PKCS#8 in VPK::generateKeyPairFiles
 * - Do a pass over C/Python bindings, update C# bindings
 *
 */

#include <vpkpp/format/VPK.h>

#include <filesystem>
#include <format>

#include <FileStream.h>
#include <kvpp/kvpp.h>
#include <sourcepp/crypto/CRC32.h>
#include <sourcepp/crypto/MD5.h>
#include <sourcepp/crypto/RSA.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>
#include <tomcrypt.h>

#ifdef VPKPP_SUPPORT_VPK_V54
#include <zstd.h>
#endif

using namespace kvpp;
using namespace sourcepp;
using namespace vpkpp;

namespace {

/// Runtime-only flag that indicates a file is going to be written to an existing archive file
constexpr uint32_t VPK_FLAG_REUSING_CHUNK = 0x1;

[[nodiscard]] std::string removeVPKAndOrDirSuffix(const std::string& path, VPK::Version version) {
	std::string filename = path;
	if (filename.size() >= 4 && filename.substr(filename.size() - 4) == (version == VPK::Version::TI_FPX_V10 ? FPX_EXTENSION : VPK_EXTENSION)) {
		filename = filename.substr(0, filename.size() - 4);
	}

	// This indicates it's a dir VPK, but some people ignore this convention...
	// It should fail later if it's not a proper dir VPK
	if (filename.size() >= 4 && filename.substr(filename.size() - 4) == (version == VPK::Version::TI_FPX_V10 ? FPX_DIR_SUFFIX : VPK_DIR_SUFFIX)) {
		filename = filename.substr(0, filename.size() - 4);
	}

	return filename;
}

} // namespace

std::unique_ptr<PackFile> VPK::create(const std::string& path, Version version) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};

		switch (version) {
			case Version::VALVE_V0:
				stream
					.write('\0');
				break;
			case Version::VALVE_V1:
				stream
					.write(VPK_SIGNATURE)
					.write(Version::VALVE_V1)
					.write<uint32_t>(1) // tree size
					.write('\0');
				break;
			case Version::VALVE_V2:
			case Version::PROMOD_V54:
				stream
					.write(VPK_SIGNATURE)
					.write(Version::VALVE_V1)
					.write<uint32_t>(1) // tree size
					.write<uint32_t>(0) // file data block size
					.write<uint32_t>(0) // archive MD5 block size
					.write<uint32_t>(0) // "other" MD5 block size
					.write<uint32_t>(0) // signature block size
					.write('\0');
				break;
			case Version::TI_FPX_V10:
				stream
					.write(FPX_SIGNATURE)
					.write(Version::TI_FPX_V10)
					.write<uint32_t>(1) // tree size
					.write('\0');
				break;
		}
	}
	return VPK::open(path);
}

std::unique_ptr<PackFile> VPK::open(const std::string& path, const EntryCallback& callback) {
	// Try loading the directory VPK first if this is a numbered archive and the dir exists
	if (path.size() >= 8) {
		if (string::matches(path.substr(path.size() - 8, path.size()), "_%d%d%d" + std::string{FPX_EXTENSION}, true)) {
			if (const auto dirPath = path.substr(0, path.size() - 8) + FPX_DIR_SUFFIX.data() + FPX_EXTENSION.data(); std::filesystem::exists(dirPath)) {
				if (auto vpk = VPK::openInternal(dirPath, callback)) {
					return vpk;
				}
			}
		}
		if (string::matches(path.substr(path.size() - 8, path.size()), "_%d%d%d" + std::string{VPK_EXTENSION}, true)) {
			if (const auto dirPath = path.substr(0, path.size() - 8) + VPK_DIR_SUFFIX.data() + VPK_EXTENSION.data(); std::filesystem::exists(dirPath)) {
				if (auto vpk = VPK::openInternal(dirPath, callback)) {
					return vpk;
				}
			}
		}
	}
	return VPK::openInternal(path, callback);
}

std::unique_ptr<PackFile> VPK::openInternal(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* vpk = new VPK{path};
	auto packFile = std::unique_ptr<PackFile>{vpk};

	FileStream reader{vpk->fullFilePath};
	reader.seek_in(0);

	if (const auto signature = reader.read<uint32_t>(); signature == FPX_SIGNATURE) {
		reader.read(vpk->version);
		if (vpk->version != Version::TI_FPX_V10) {
			// File is not an FPX
			return nullptr;
		}
	} else if (signature == VPK_SIGNATURE) {
		reader.read(vpk->version);
		if (
			vpk->version != Version::VALVE_V1 &&
			vpk->version != Version::VALVE_V2 &&
			vpk->version != Version::PROMOD_V54
		) {
			// Titanfall 1/2 are not supported currently
			return nullptr;
		}
	} else {
		reader.seek_in(3, std::ios::end);
		if (reader.read<char>() == '\0' && reader.read<char>() == '\0' && reader.read<char>() == '\0') {
			// hack: if file is 9 bytes long it's probably an empty VTMB VPK and we should bail so that code can pick it up
			// either way a 9 byte long VPK should not have any files in it
			if (std::filesystem::file_size(vpk->fullFilePath) == 9) {
				return nullptr;
			}

			vpk->version = Version::VALVE_V0;
			reader.seek_in(0);
		} else {
			// File is not a VPK
			return nullptr;
		}
	}
	vpk->versionModified = vpk->version;

	vpk->headerSize = 0;
	if (vpk->version != Version::VALVE_V0) {
		reader >> vpk->treeSize;
		vpk->headerSize += sizeof(uint32_t) * 3;
		if (vpk->hasExtendedHeader()) {
			reader
				>> vpk->fileDataSize
				>> vpk->hashedChunksSize
				>> vpk->otherMD5Size
				>> vpk->signatureSize;
			vpk->headerSize += sizeof(uint32_t) * 4;
		}
	}

	while (true) {
		const auto extension = reader.read_string();
		if (extension.empty()) {
			break;
		}

		while (true) {
			const auto directory = reader.read_string();
			if (directory.empty()) {
				break;
			}

			std::string fullDir;
			if (directory == " ") {
				fullDir = "";
			} else {
				fullDir = directory;
			}

			while (true) {
				const auto entryName = reader.read_string();
				if (entryName.empty()) {
					break;
				}

				Entry entry = createNewEntry();

				std::string entryPath;
				if (extension == " ") {
					entryPath = fullDir.empty() ? "" : fullDir + '/';
					entryPath += entryName;
				} else {
					entryPath = fullDir.empty() ? "" : fullDir + '/';
					entryPath += entryName + '.';
					entryPath += extension;
				}
				entryPath = vpk->cleanEntryPath(entryPath);

				reader.read(entry.crc32);
				const auto preloadedDataSize = reader.read<uint16_t>();
				entry.archiveIndex = reader.read<uint16_t>();
				entry.offset = reader.read<uint32_t>();
				entry.length = reader.read<uint32_t>();

				if (vpk->version == Version::PROMOD_V54) {
					entry.compressedLength = reader.read<uint32_t>();
				}

				if (reader.read<uint16_t>() != 0xffff) {
					// Invalid terminator
					return nullptr;
				}

				if (preloadedDataSize > 0) {
					entry.extraData = reader.read_bytes(preloadedDataSize);
					entry.length += preloadedDataSize;
				}

				if (entry.archiveIndex != VPK_DIR_ARCHIVE_INDEX && std::cmp_greater(entry.archiveIndex, vpk->numArchives)) {
					vpk->numArchives = static_cast<int32_t>(entry.archiveIndex);
				}

				vpk->entries.emplace(entryPath, entry);

				if (callback) {
					callback(entryPath, entry);
				}
			}
		}
	}
	if (vpk->version == Version::VALVE_V0) {
		vpk->treeSize = reader.tell_in();
	}

	// If there are no archives, -1 will be incremented to 0
	vpk->numArchives++;

	// VPK v1 has nothing else for us
	if (!vpk->hasExtendedHeader()) {
		vpk->fileDataSize = std::filesystem::file_size(vpk->fullFilePath) - vpk->treeSize - vpk->headerSize;
		return packFile;
	}

	// Skip over file data, if any
	reader.seek_in(vpk->fileDataSize, std::ios::cur);

	if (vpk->hashedChunksSize % sizeof(HashedChunk) != 0) {
		return nullptr;
	}
	const auto entryNum = vpk->hashedChunksSize / sizeof(HashedChunk);
	for (uint32_t i = 0; i < entryNum; i++) {
		auto& [archiveIndex, hashType, offset, length, hash] = vpk->hashedChunks.emplace_back();
		reader >> archiveIndex >> hashType >> offset >> length >> hash;
	}

	if (vpk->otherMD5Size != sizeof(vpk->treeChecksum) + sizeof(vpk->hashedChunksChecksum) + sizeof(vpk->wholeFileChecksum)) {
		// We don't know how to handle the VPK otherwise
		return nullptr;
	}
	vpk->treeChecksum = reader.read_bytes<16>();
	vpk->hashedChunksChecksum = reader.read_bytes<16>();
	vpk->wholeFileChecksum = reader.read_bytes<16>();

	if (!vpk->signatureSize) {
		return packFile;
	}
	if (auto publicKeySize = reader.read<uint32_t>(); publicKeySize == VPK_SIGNATURE) {
		// New signature format in some recent Source 2 games
		if (vpk->signatureSize <= 20) {
			// Empty
			return packFile;
		}
		reader.read(vpk->signatureType);
		if (vpk->signatureType != SignatureType::WHOLE_FILE && vpk->signatureType != SignatureType::WHOLE_FILE_CHECKSUM) {
			// Unknown signature type
			vpk->signatureType = SignatureType::UNKNOWN;
			return packFile;
		}
		reader.read<uint32_t>(publicKeySize);
		const auto realSignatureSize = reader.read<uint32_t>();
		reader.skip_in<uint32_t>();
		vpk->publicKey = reader.read_bytes(publicKeySize);
		vpk->signature = reader.read_bytes(realSignatureSize);
	} else {
		vpk->signatureType = SignatureType::LEGACY;
		vpk->publicKey = reader.read_bytes(publicKeySize);
		vpk->signature = reader.read_bytes(reader.read<int32_t>());
	}

	return packFile;
}

std::vector<std::string> VPK::verifyEntryChecksums() const {
	return this->verifyEntryChecksumsUsingCRC32();
}

bool VPK::hasPackFileChecksum() const {
	return this->hasExtendedHeader();
}

bool VPK::verifyPackFileChecksum() const {
	// File checksums are only in VALVE_V2 and PROMOD_V54
	if (!this->hasExtendedHeader()) {
		return true;
	}

	FileStream stream{this->getFilepath().data()};
	return
		this->treeChecksum == crypto::computeMD5(stream.seek_in(this->headerSize).read_bytes(this->treeSize)) &&
		this->hashedChunksChecksum == crypto::computeMD5(stream.seek_in(this->headerSize + this->treeSize + this->fileDataSize).read_bytes(this->hashedChunksSize)) &&
		this->wholeFileChecksum == crypto::computeMD5(stream.seek_in(0).read_bytes(this->headerSize + this->treeSize + this->fileDataSize + this->hashedChunksSize + this->otherMD5Size - sizeof(this->wholeFileChecksum)));
}

bool VPK::hasPackFileSignature() const {
	if (!this->hasExtendedHeader()) {
		return false;
	}
	if (this->publicKey.empty() || this->signature.empty()) {
		return false;
	}
	return true;
}

bool VPK::verifyPackFileSignature() const {
	// Signatures are only supported in VPKs that can hold them
	if (!this->hasExtendedHeader() || this->publicKey.empty() || this->signature.empty()) {
		return true;
	}
	switch (this->signatureType) {
		case SignatureType::UNKNOWN:
			break;
		case SignatureType::LEGACY:
		case SignatureType::WHOLE_FILE: {
			auto dirFileBuffer = fs::readFileBuffer(this->getFilepath().data());
			const auto signatureSectionSize = this->publicKey.size() + this->signature.size() + sizeof(uint32_t) * 2;
			if (dirFileBuffer.size() <= signatureSectionSize) {
				return false;
			}
			dirFileBuffer.resize(dirFileBuffer.size() - signatureSectionSize);
			return crypto::verifyPublicKeySHA256(dirFileBuffer, this->publicKey, this->signature);
		}
		case SignatureType::WHOLE_FILE_CHECKSUM: {
			if (!this->verifyPackFileChecksum()) {
				return false;
			}
			return crypto::verifyPublicKeySHA256(this->wholeFileChecksum, this->publicKey, this->signature);
		}
	}
	return false;
}

// NOLINTNEXTLINE(*-no-recursion)
std::optional<std::vector<std::byte>> VPK::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	const auto entryLength = this->hasCompression() && entry->compressedLength ? entry->compressedLength : entry->length;
	if (entryLength == 0) {
		return std::vector<std::byte>{};
	}
	std::vector out(entryLength, static_cast<std::byte>(0));

	if (!entry->extraData.empty()) {
		std::ranges::copy(entry->extraData, out.begin());
	}
	if (entryLength != entry->extraData.size()) {
		if (entry->archiveIndex != VPK_DIR_ARCHIVE_INDEX) {
			// Stored in a numbered archive
			FileStream stream{this->getTruncatedFilepath() + '_' + string::padNumber(entry->archiveIndex, 3) + std::string{this->version == Version::TI_FPX_V10 ? FPX_EXTENSION : VPK_EXTENSION}};
			if (!stream) {
				return std::nullopt;
			}
			stream.seek_in_u(entry->offset);
			auto bytes = stream.read_bytes(entryLength - entry->extraData.size());
			std::ranges::copy(bytes, out.begin() + static_cast<ptrdiff_t>(entry->extraData.size()));
		} else {
			// Stored in this directory VPK
			FileStream stream{this->fullFilePath};
			if (!stream) {
				return std::nullopt;
			}
			stream.seek_in_u(this->headerSize + this->treeSize + entry->offset);
			auto bytes = stream.read_bytes(entry->length - entry->extraData.size());
			std::ranges::copy(bytes, out.begin() + static_cast<ptrdiff_t>(entry->extraData.size()));
		}
	}

#ifndef VPKPP_SUPPORT_VPK_V54
	return out;
#else
	if (!this->hasCompression() || !entry->compressedLength) {
		return out;
	}

	const auto decompressionDict = this->readEntry(this->getTruncatedFilestem() + ".dict");
	if (!decompressionDict) {
		return std::nullopt;
	}

	const std::unique_ptr<ZSTD_DDict, void(*)(void*)> dDict{
		ZSTD_createDDict(decompressionDict->data(), decompressionDict->size()),
		[](void* dDict_) { ZSTD_freeDDict(static_cast<ZSTD_DDict*>(dDict_)); },
	};
	if (!dDict) {
		return std::nullopt;
	}

	const std::unique_ptr<ZSTD_DCtx, void(*)(void*)> dCtx{
		ZSTD_createDCtx(),
		[](void* dCtx_) { ZSTD_freeDCtx(static_cast<ZSTD_DCtx*>(dCtx_)); },
	};
	if (!dCtx) {
		return std::nullopt;
	}

	std::vector<std::byte> decompressedData;
	decompressedData.resize(entry->length);

	if (ZSTD_isError(ZSTD_decompress_usingDDict(dCtx.get(), decompressedData.data(), decompressedData.size(), out.data(), out.size(), dDict.get()))) {
		return std::nullopt;
	}
	return decompressedData;
#endif
}

void VPK::addEntryInternal(Entry& entry, const std::string&, std::vector<std::byte>& buffer, EntryOptions options) {
	if (this->hasCompression()) {
		// I don't feel like getting this to work right now
		options.vpk_preloadBytes = 0;
	}

	entry.crc32 = crypto::computeCRC32(buffer);
	entry.length = buffer.size();

	// Offset will be reset when it's baked, assuming we're not replacing an existing chunk (when flags = 1)
	// Compressed entries will not replace existing chunks, since their size is unknown
	entry.flags = 0;
	entry.offset = 0;
	entry.archiveIndex = options.vpk_saveToDirectory ? VPK_DIR_ARCHIVE_INDEX : this->numArchives;
	if (!options.vpk_saveToDirectory && !this->freedChunks.empty() && !this->hasCompression()) {
		int64_t bestChunkIndex = -1;
		std::size_t currentChunkGap = SIZE_MAX;
		for (int64_t i = 0; i < this->freedChunks.size(); i++) {
			if (
				(bestChunkIndex < 0 && this->freedChunks[i].length >= entry.length) ||
				(bestChunkIndex >= 0 && this->freedChunks[i].length >= entry.length && this->freedChunks[i].length - entry.length < currentChunkGap)
			) {
				bestChunkIndex = i;
				currentChunkGap = this->freedChunks[i].length - entry.length;
			}
		}
		if (bestChunkIndex >= 0) {
			entry.flags |= VPK_FLAG_REUSING_CHUNK;
			entry.offset = this->freedChunks[bestChunkIndex].offset;
			entry.archiveIndex = this->freedChunks[bestChunkIndex].archiveIndex;
			this->freedChunks.erase(this->freedChunks.begin() + bestChunkIndex);
			if (currentChunkGap < SIZE_MAX && currentChunkGap > 0) {
				// Add the remaining free space as a free chunk
				this->freedChunks.push_back({
					.offset = entry.offset + entry.length,
					.length = currentChunkGap,
					.archiveIndex = entry.archiveIndex,
				});
			}
		}
	}

	if (options.vpk_preloadBytes > 0) {
		const auto clampedPreloadBytes = std::clamp<uint16_t>(options.vpk_preloadBytes, 0, buffer.size() > VPK_MAX_PRELOAD_BYTES ? VPK_MAX_PRELOAD_BYTES : static_cast<uint16_t>(buffer.size()));
		entry.extraData.resize(clampedPreloadBytes);
		std::memcpy(entry.extraData.data(), buffer.data(), clampedPreloadBytes);
		buffer.erase(buffer.begin(), buffer.begin() + clampedPreloadBytes);
	}

	// Now that archive index is calculated for this entry, check if it needs to be incremented
	if (!options.vpk_saveToDirectory && !(entry.flags & VPK_FLAG_REUSING_CHUNK)) {
		entry.offset = this->currentlyFilledChunkSize;
		this->currentlyFilledChunkSize += static_cast<int>(buffer.size());
		if (this->currentlyFilledChunkSize > this->chunkSize) {
			this->currentlyFilledChunkSize = 0;
			this->numArchives++;
		}
	}
}

bool VPK::removeEntry(const std::string& filename_) {
	const auto filename = this->cleanEntryPath(filename_);
	if (const auto entry = this->findEntry(filename); entry && (!entry->unbaked || entry->flags & VPK_FLAG_REUSING_CHUNK)) {
		this->freedChunks.push_back({
			.offset = entry->offset,
			.length = entry->length,
			.archiveIndex = entry->archiveIndex,
		});
	}
	return PackFile::removeEntry(filename);
}

std::size_t VPK::removeDirectory(const std::string& dirName_) {
	auto dirName = this->cleanEntryPath(dirName_);
	if (!dirName.empty()) {
		dirName += '/';
	}
	this->runForAllEntries([this, &dirName](const std::string& path, const Entry& entry) {
		if (path.starts_with(dirName) && (!entry.unbaked || entry.flags & VPK_FLAG_REUSING_CHUNK)) {
			this->freedChunks.push_back({
				.offset = entry.offset,
				.length = entry.length,
				.archiveIndex = entry.archiveIndex,
			});
		}
	});
	return PackFile::removeDirectory(dirName_);
}

bool VPK::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

#ifdef VPKPP_SUPPORT_VPK_V54
	// Store compression dictionary
	std::optional<std::vector<std::byte>> compressionDict;
	std::unique_ptr<ZSTD_CDict, void(*)(void*)> cDict{nullptr, nullptr};
	std::unique_ptr<ZSTD_CCtx, void(*)(void*)> cCtx{nullptr, nullptr};
	if (this->version == Version::PROMOD_V54) {
		compressionDict = this->readEntry(this->getTruncatedFilestem() + ".dict");
		if (!compressionDict) {
			return false;
		}

		cDict = {
			ZSTD_createCDict(compressionDict->data(), compressionDict->size(), options.zip_compressionStrength),
			[](void* cDict_) { ZSTD_freeCDict(static_cast<ZSTD_CDict*>(cDict_)); },
		};
		if (!cDict) {
			return false;
		}

		cCtx = {
			ZSTD_createCCtx(),
			[](void* cCtx_) { ZSTD_freeCCtx(static_cast<ZSTD_CCtx*>(cCtx_)); },
		};
		if (!cCtx) {
			return false;
		}
	}
#endif

	// Reconstruct data so we're not looping over it a ton of times
	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::pair<std::string, Entry*>>>> temp;
	this->runForAllEntriesInternal([&temp](const std::string& path, Entry& entry) {
		const auto fsPath = std::filesystem::path{path};
		auto extension = fsPath.extension().string();
		if (extension.starts_with('.')) {
			extension = extension.substr(1);
		}
		const auto parentDir = fsPath.parent_path().string();

		if (extension.empty()) {
			extension = " ";
		}
		if (!temp.contains(extension)) {
			temp[extension] = {};
		}
		if (!temp.at(extension).contains(parentDir)) {
			temp.at(extension)[parentDir] = {};
		}
		temp.at(extension).at(parentDir).emplace_back(path, &entry);
	});

	// Temporarily store baked file data that's stored in the directory VPK since it's getting overwritten
	std::vector<std::byte> dirVPKEntryData;
	std::size_t newDirEntryOffset = 0;
	this->runForAllEntriesInternal([this, &dirVPKEntryData, &newDirEntryOffset](const std::string& path, Entry& entry) {
		if (entry.archiveIndex != VPK_DIR_ARCHIVE_INDEX || entry.length == entry.extraData.size()) {
			return;
		}

		auto binData = this->readEntry(path);
		if (!binData) {
			return;
		}
		dirVPKEntryData.reserve(dirVPKEntryData.size() + entry.length - entry.extraData.size());
		dirVPKEntryData.insert(dirVPKEntryData.end(), binData->begin() + static_cast<std::vector<std::byte>::difference_type>(entry.extraData.size()), binData->end());

		entry.offset = newDirEntryOffset;
		newDirEntryOffset += entry.length - entry.extraData.size();
	}, false);

	// Helper
	const auto getArchiveFilename = [this](const std::string& filename_, uint32_t archiveIndex) {
		std::string out{filename_ + '_' + string::padNumber(archiveIndex, 3) + std::string{this->version == Version::TI_FPX_V10 ? FPX_EXTENSION : VPK_EXTENSION}};
		string::normalizeSlashes(out);
		return out;
	};

	// Copy external binary blobs to the new dir
	if (!outputDir_.empty()) {
		for (uint32_t archiveIndex = 0; archiveIndex < this->numArchives; archiveIndex++) {
			std::string from = getArchiveFilename(this->getTruncatedFilepath(), archiveIndex);
			if (!std::filesystem::exists(from)) {
				continue;
			}
			std::string dest = getArchiveFilename(outputDir + '/' + this->getTruncatedFilestem(), archiveIndex);
			if (from == dest) {
				continue;
			}
			std::filesystem::copy_file(from, dest, std::filesystem::copy_options::overwrite_existing);
		}
	}

	FileStream outDir{outputPath, FileStream::OPT_READ | FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	outDir.seek_in(0);
	outDir.seek_out(0);

	// Dummy header
	if (this->version != Version::VALVE_V0) {
		outDir
			.write(this->version == Version::TI_FPX_V10 ? FPX_SIGNATURE : VPK_SIGNATURE)
			.write(this->version)
			.write(this->treeSize);
		if (this->hasExtendedHeader()) {
			outDir
				.write(this->fileDataSize)
				.write(this->hashedChunksSize)
				.write(this->otherMD5Size)
				.write(this->signatureSize);
		}
	}

	// File tree data
	for (auto& [ext, dirs] : temp) {
		outDir.write(ext);

		for (auto& [dir, tempEntries] : dirs) {
			outDir.write(!dir.empty() ? dir : " ");

			for (auto& [path, entry] : tempEntries) {
				// Calculate entry offset if it's unbaked and upload the data
				if (entry->unbaked) {
					auto entryData = readUnbakedEntry(*entry);
					if (!entryData) {
						continue;
					}

					if (entry->length == entry->extraData.size() && !this->hasCompression()) {
						// Override the archive index, no need for an archive VPK
						entry->archiveIndex = VPK_DIR_ARCHIVE_INDEX;
						entry->offset = dirVPKEntryData.size();
					} else if (entry->archiveIndex != VPK_DIR_ARCHIVE_INDEX && entry->flags & VPK_FLAG_REUSING_CHUNK) {
						// The entry is replacing pre-existing data in a VPK archive - it's not compressed
						auto archiveFilename = getArchiveFilename(::removeVPKAndOrDirSuffix(outputPath, this->version), entry->archiveIndex);
						FileStream stream{archiveFilename, FileStream::OPT_READ | FileStream::OPT_WRITE | FileStream::OPT_CREATE_IF_NONEXISTENT};
						stream.seek_out_u(entry->offset);
						stream.write(*entryData);
					} else if (entry->archiveIndex != VPK_DIR_ARCHIVE_INDEX) {
						// The entry is being appended to a newly created VPK archive
						auto archiveFilename = getArchiveFilename(::removeVPKAndOrDirSuffix(outputPath, this->version), entry->archiveIndex);
						entry->offset = std::filesystem::exists(archiveFilename) ? std::filesystem::file_size(archiveFilename) : 0;
						FileStream stream{archiveFilename, FileStream::OPT_APPEND | FileStream::OPT_CREATE_IF_NONEXISTENT};
#ifndef VPKPP_SUPPORT_VPK_V54
						stream.write(*entryData);
#else
						if (this->version != Version::PROMOD_V54 || path == this->getTruncatedFilestem() + ".dict") {
							stream.write(*entryData);
						} else {
							std::vector<std::byte> compressedData;
							compressedData.resize(ZSTD_compressBound(entryData->size()));
							auto compressedSize = ZSTD_compress_usingCDict(cCtx.get(), compressedData.data(), compressedData.size(), entryData->data(), entryData->size(), cDict.get());
							if (ZSTD_isError(compressedSize) || compressedData.size() < compressedSize) {
								return false;
							}
							stream.write(std::span{compressedData.data(), compressedSize});
							entry->compressedLength = compressedSize;
						}
#endif
					} else {
						// The entry will be added to the directory VPK
						entry->offset = dirVPKEntryData.size();
#ifndef VPKPP_SUPPORT_VPK_V54
						dirVPKEntryData.insert(dirVPKEntryData.end(), entryData->data(), entryData->data() + entryData->size());
#else
						if (this->version != Version::PROMOD_V54 || path == this->getTruncatedFilestem() + ".dict") {
							dirVPKEntryData.insert(dirVPKEntryData.end(), entryData->data(), entryData->data() + entryData->size());
						} else {
							std::vector<std::byte> compressedData;
							compressedData.resize(ZSTD_compressBound(entryData->size()));
							auto compressedSize = ZSTD_compress_usingCDict(cCtx.get(), compressedData.data(), compressedData.size(), entryData->data(), entryData->size(), cDict.get());
							if (ZSTD_isError(compressedSize) || compressedData.size() < compressedSize) {
								return false;
							}
							dirVPKEntryData.insert(dirVPKEntryData.end(), compressedData.data(), compressedData.data() + compressedSize);
							entry->compressedLength = compressedSize;
						}
#endif
					}

					// Clear flags
					entry->flags = 0;
				}

				outDir.write(std::filesystem::path{path}.stem().string());
				outDir.write(entry->crc32);
				outDir.write<uint16_t>(entry->extraData.size());
				outDir.write<uint16_t>(entry->archiveIndex);
				outDir.write<uint32_t>(entry->offset);
				outDir.write<uint32_t>(entry->length - entry->extraData.size());

				if (this->version == Version::PROMOD_V54) {
					outDir.write<uint32_t>(entry->compressedLength - entry->extraData.size());
				}

				outDir.write<uint32_t>(0xffff);

				if (!entry->extraData.empty()) {
					outDir.write(entry->extraData);
				}

				if (callback) {
					callback(path, *entry);
				}
			}
			outDir.write('\0');
		}
		outDir.write('\0');
	}
	outDir.write('\0');

	// Put files copied from the dir archive back
	if (!dirVPKEntryData.empty()) {
		outDir.write(dirVPKEntryData);
	}

	// Merge unbaked into baked entries
	this->mergeUnbakedEntries();

	// Calculate Header1
	this->treeSize = outDir.tell_out() - dirVPKEntryData.size() - this->headerSize;

	// Non-v1 stuff
	if (this->hasExtendedHeader()) {
		// Calculate hashes for all entries
		this->hashedChunks.clear();
		if (options.vpk_generateHashedChunks != BakeOptions::VPKHashBlockType::NONE) {
			this->runForAllEntries([this](const std::string& path, const Entry& entry) {
				const auto binData = this->readEntry(path);
				if (!binData) {
					return;
				}
				const HashedChunk hashedChunk{
					.archiveIndex = static_cast<uint16_t>(entry.archiveIndex),
					.hashType = HashedChunk::HashType::MD5, // todo
					.offset = static_cast<uint32_t>(entry.offset),
					.length = static_cast<uint32_t>(entry.length - entry.extraData.size()),
					.hash = crypto::computeMD5(*binData),
				};
				this->hashedChunks.push_back(hashedChunk);
			}, false);
		}

		// Calculate Header2
		this->fileDataSize = dirVPKEntryData.size();
		this->hashedChunksSize = this->hashedChunks.size() * sizeof(HashedChunk);
		this->otherMD5Size = 48;
		this->signatureSize = 0;

		// Calculate Footer2
		hash_state wholeFileChecksumMD5;
		md5_init(&wholeFileChecksumMD5);
		{
			//md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(&this->header1), sizeof(this->header1));
			//md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(&this->header2), sizeof(this->header2));
		}
		{
			//outDir.seek_in(sizeof(Header1) + sizeof(Header2));
			if (this->treeSize > 0) {
				std::vector<std::byte> treeData = outDir.read_bytes(this->treeSize);
				md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(treeData.data()), treeData.size());
				this->treeChecksum = crypto::computeMD5(treeData);
			} else {
				this->treeChecksum = {};
			}
		}
		if (!dirVPKEntryData.empty()) {
			md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(dirVPKEntryData.data()), dirVPKEntryData.size());
		}
		{
			if (!this->hashedChunks.empty()) {
				md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(this->hashedChunks.data()), this->hashedChunks.size() * sizeof(HashedChunk));
				this->hashedChunksChecksum = crypto::computeMD5({reinterpret_cast<const std::byte*>(this->hashedChunks.data()), this->hashedChunks.size() * sizeof(HashedChunk)});
			} else {
				this->hashedChunksChecksum = {};
			}
		}
		if (!this->treeChecksum.empty()) {
			md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(this->treeChecksum.data()), this->treeChecksum.size());
		}
		if (!this->hashedChunksChecksum.empty()) {
			md5_process(&wholeFileChecksumMD5, reinterpret_cast<const unsigned char*>(this->hashedChunksChecksum.data()), this->hashedChunksChecksum.size());
		}
		md5_done(&wholeFileChecksumMD5, reinterpret_cast<unsigned char*>(this->wholeFileChecksum.data()));

		// We can't recalculate the signature without the private key
		this->publicKey.clear();
		this->signature.clear();
	}

	// Ancient crap VPK with no header
	if (this->version == Version::VALVE_V0) {
		PackFile::setFullFilePath(outputDir);
		return true;
	}

	// Write new headers
	outDir.seek_out(0);
	//outDir.write(this->header1);

	// MD5/BLAKE3 hashes, file signature
	if (!this->hasExtendedHeader()) {
		PackFile::setFullFilePath(outputDir);
		return true;
	}

	//outDir.write(this->header2);

	// Add MD5 hashes
	//outDir.seek_out_u(sizeof(Header1) + sizeof(Header2) + this->treeSize + dirVPKEntryData.size());
	outDir.write(this->hashedChunks);
	outDir.write(this->treeChecksum);
	outDir.write(this->hashedChunksChecksum);
	outDir.write(this->wholeFileChecksum);

	// The signature section is not present
	PackFile::setFullFilePath(outputDir);
	return true;
}

std::string VPK::getTruncatedFilestem() const {
	std::string filestem = this->getFilestem();
	// This indicates it's a dir VPK, but some people ignore this convention...
	if (filestem.size() >= 4 && filestem.substr(filestem.size() - 4) == (this->version == Version::TI_FPX_V10 ? FPX_DIR_SUFFIX : VPK_DIR_SUFFIX)) {
		filestem = filestem.substr(0, filestem.size() - 4);
	}
	return filestem;
}

Attribute VPK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | VPK_PRELOADED_DATA | ARCHIVE_INDEX | CRC32;
}

VPK::operator std::string() const {
	return PackFile::operator std::string() + std::format(" | Version v{}", static_cast<uint32_t>(this->version));
}

void VPK::generateKeyPairFiles(const std::filesystem::path& base, SignatureType signatureType, std::string_view passphrase) {
	// todo: passphrase
	const auto [privateKey, publicKey] = crypto::computeRSAKeyPair(getSignatureSize(signatureType));
	const auto publicKeyHex = string::encodeHex(publicKey);
	{
		auto privateKeyPath = base;
		privateKeyPath.replace_extension(".privatekey.vdf");
		FileStream stream{privateKeyPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.write(std::format(R"(// Private key file.
// This key can be used to sign files.  Third parties can verify your signature by using your public key.
//
// THIS KEY SHOULD BE KEPT SECRET
//
// You should share your public key freely, but anyone who has your private key will be able to impersonate you.
private_key
{{
	type "rsa"
	rsa_private_key "{}"


	// Note: the private key is stored in plaintext.  It is not encrypted or protected by a password.
	//       Anyone who obtains this key can use it to sign files.
	private_key_encrypted {}

	// The public key that corresponds to this private key.  The public keyfile you can share with others is
	// saved in another file, but the key data is duplicated here to help you confirm which public key matches
	// with this private key.
	public_key
	{{
		type "rsa"
		rsa_public_key "{}"
	}}
}}
)", string::encodeHex(privateKey), static_cast<int>(!passphrase.empty()), publicKeyHex), false);
	}
	{
		auto publicKeyPath = base;
		publicKeyPath.replace_extension(".publickey.vdf");
		FileStream stream{publicKeyPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.write(std::format(R"(// Public key file.  You can publish this key file and share it with the world.
// It can be used by third parties to verify any signatures made with the corresponding private key.
public_key
{{
	type "rsa"
	rsa_public_key "{}"
}}
)", publicKeyHex), false);
	}
}

bool VPK::sign(const std::filesystem::path& filename_, SignatureType signatureType_, std::string_view passphrase) {
	if (!this->hasExtendedHeader() || !std::filesystem::exists(filename_) || std::filesystem::is_directory(filename_)) {
		return false;
	}

	const KV1 fileKV{fs::readFileText(filename_)};

	const auto privateKeyHex = fileKV["private_key"]["rsa_private_key"].getValue();
	if (privateKeyHex.empty()) {
		return false;
	}
	const auto publicKeyHex = fileKV["private_key"]["public_key"]["rsa_public_key"].getValue();
	if (publicKeyHex.empty()) {
		return false;
	}

	return this->sign(string::decodeHex(privateKeyHex), string::decodeHex(publicKeyHex), signatureType_, passphrase);
}

bool VPK::sign(const std::vector<std::byte>& privateKey, const std::vector<std::byte>& publicKey_, SignatureType signatureType_, std::string_view passphrase) {
	const auto failureHandler = [this, oldSignatureType = this->signatureType, oldSignatureSize = this->signatureSize] {
		this->signatureType = oldSignatureType;
		this->signatureSize = oldSignatureSize;
		this->publicKey.clear();
		this->signature.clear();
		return false;
	};

	if (!this->hasExtendedHeader()) {
		return failureHandler();
	}
	this->signatureType = signatureType_;
	this->publicKey = publicKey_;

	switch (this->signatureType) {
		case SignatureType::UNKNOWN:
			break;
		case SignatureType::LEGACY: {
			auto dirFileBuffer = fs::readFileBuffer(std::string{this->getFilepath()});
			if (dirFileBuffer.size() <= this->signatureSize) {
				return failureHandler();
			}
			dirFileBuffer.resize(dirFileBuffer.size() - this->signatureSize);

			uint16_t expectedSignatureSize = getSignatureSize(signatureType_) / 8;
			this->signatureSize = sizeof(uint32_t) + publicKey_.size() + sizeof(uint32_t) + expectedSignatureSize;
			{
				BufferStream stream{dirFileBuffer};
				stream.seek_u(this->headerSize - sizeof(uint32_t)).write<uint32_t>(this->signatureSize);
			}

			this->signature = crypto::signDataWithPrivateKeySHA256(dirFileBuffer, privateKey);
			if (this->signature.empty()) {
				return failureHandler();
			}
			if (this->signature.size() != expectedSignatureSize) {
				expectedSignatureSize = this->signature.size();
				this->signatureSize = sizeof(uint32_t) + publicKey_.size() + sizeof(uint32_t) + expectedSignatureSize;

				{
					BufferStream stream{dirFileBuffer};
					stream.seek_u(this->headerSize - sizeof(uint32_t)).write<uint32_t>(this->signatureSize);
				}

				this->signature = crypto::signDataWithPrivateKeySHA256(dirFileBuffer, privateKey);
				if (this->signature.empty() || this->signature.size() != expectedSignatureSize) {
					return failureHandler();
				}
			}

			{
				FileStream stream{std::string{this->getFilepath()}, FileStream::OPT_READ | FileStream::OPT_WRITE};
				stream
					.seek_out_u(this->headerSize - sizeof(uint32_t))
					.write<uint32_t>(this->signatureSize)
					.seek_out(this->headerSize + this->treeSize + this->fileDataSize + this->hashedChunksSize + this->otherMD5Size)
					.write<uint32_t>(this->publicKey.size())
					.write(this->publicKey)
					.write<uint32_t>(this->signature.size())
					.write(this->signature);
			}
			if (
				const auto expectedFileSize = this->headerSize + this->treeSize + this->fileDataSize + this->hashedChunksSize + this->otherMD5Size + this->signatureSize;
				expectedFileSize < std::filesystem::file_size(this->getFilepath())
			) {
				std::filesystem::resize_file(this->getFilepath(), expectedFileSize);
			}
			return true;
		}
		case SignatureType::WHOLE_FILE:
		case SignatureType::WHOLE_FILE_CHECKSUM: {
			// todo
			break;
		}
	}
	return false;
}

VPK::Version VPK::getVersion() const {
	return this->versionModified;
}

void VPK::setVersion(Version version_) {
	this->versionModified = version_;
}

uint32_t VPK::getChunkSize() const {
	return this->chunkSize;
}

void VPK::setChunkSize(uint32_t newChunkSize) {
	this->chunkSize = newChunkSize;
}

bool VPK::hasExtendedHeader() const {
	return this->version == Version::VALVE_V2 || this->version == Version::PROMOD_V54;
}

bool VPK::hasCompression() const {
	return this->version == Version::PROMOD_V54;
}
