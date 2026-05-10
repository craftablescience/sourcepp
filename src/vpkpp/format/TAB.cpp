// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppRedundantQualifier

#include <vpkpp/format/TAB.h>

#include <filesystem>
#include <format>
#include <numeric>

#include <BufferStream.h>
#include <FileStream.h>
#include <tomcrypt.h>

using namespace sourcepp;
using namespace vpkpp;

namespace {

constexpr std::string_view TAB_FILEPATH_LIST_STRIP_PATH_INDEX = "projects/justcause/data/";

[[nodiscard]] std::filesystem::path getArchivePath(const TAB& tab, uint32_t archiveIndex) {
	return std::filesystem::path{tab.getFilepath()}.parent_path() / std::format("{}{}{}", tab.getFilestem(), archiveIndex, ARC_EXTENSION);
}

} // namespace

std::unique_ptr<PackFile> TAB::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* tab = new TAB{path};
	auto packFile = std::unique_ptr<PackFile>(tab);

	FileStream reader{tab->fullFilePath};
	reader.seek_in(0);

	reader >> tab->version;
	if (tab->version != 3) {
		BufferStream::swap_endian(&tab->version);
		if (tab->version == 3) {
			reader.set_big_endian(true);
		} else {
			return nullptr;
		}
	}

	reader >> tab->sectorSize >> tab->numArchives;

	std::vector<std::tuple<std::filesystem::path, uint64_t, uint32_t>> archives;
	uint32_t alignment = 0;
	for (uint32_t i = 0; i < tab->numArchives; i++) {
		auto& [archivePath, archiveSize, archiveAlignment] = archives.emplace_back();
		archivePath = ::getArchivePath(*tab, i),
		archiveSize = static_cast<uint32_t>(std::filesystem::file_size(archivePath));
		alignment += (archiveSize + tab->sectorSize - 1) / tab->sectorSize;
		archiveAlignment = alignment;
	}

	// Here we load in the filepath list if it exists
	std::unordered_map<uint32_t, std::string> crackedHashes;
	if (const std::filesystem::path mapPath{std::filesystem::path{tab->fullFilePath}.parent_path() / std::format("{}list.txt", tab->getFilestem())}; std::filesystem::exists(mapPath)) {
		std::ifstream mapFile{mapPath};
		std::string filepath;
		while (std::getline(mapFile, filepath)) {
			if (filepath.empty() || filepath.starts_with(':')) {
				continue;
			}
			string::trim(filepath);
			string::normalizeSlashes(filepath);
			string::toLower(filepath);
			if (const auto index = filepath.rfind(TAB_FILEPATH_LIST_STRIP_PATH_INDEX); index != std::string::npos) {
				filepath = filepath.substr(index + TAB_FILEPATH_LIST_STRIP_PATH_INDEX.size());
			}
			crackedHashes[TAB::hashFilePath(filepath)] = filepath;
		}
	}

	const auto fileCount = (std::filesystem::file_size(tab->fullFilePath) - sizeof(uint32_t) * 3) / (sizeof(uint32_t) * 3);
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		std::string entryPath;

		// note: NOT a CRC32! check TAB::hashFilePath
		entry.crc32 = reader.read<uint32_t>();
		if (crackedHashes.contains(entry.crc32)) {
			entryPath = tab->cleanEntryPath(crackedHashes[entry.crc32]);
		} else {
			entryPath = tab->cleanEntryPath(TAB_HASHED_FILEPATH_PREFIX.data() + string::encodeHex({reinterpret_cast<const std::byte*>(&entry.crc32), sizeof(entry.crc32)}));
		}

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();
		entry.archiveIndex = 0;

		for (int j = 0; j < archives.size(); j++) {
			if (entry.offset < std::get<2>(archives[j])) {
				entry.archiveIndex = j;
				break;
			}
		}
		if (entry.archiveIndex == 0) {
			entry.offset = (entry.offset * tab->sectorSize) % ARC_CHUNK_SIZE;
		} else {
			entry.offset = (entry.offset - std::get<2>(archives[entry.archiveIndex - 1])) * tab->sectorSize % ARC_CHUNK_SIZE;
		}

		tab->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> TAB::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{::getArchivePath(*this, entry->archiveIndex)};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

Attribute TAB::getSupportedEntryAttributes() const {
	using enum Attribute;
	return ARCHIVE_INDEX | LENGTH;
}

TAB::operator std::string() const {
	return PackFileReadOnly::operator std::string() + std::format(" | Version v{}", this->version);
}

uint32_t TAB::hashFilePath(const std::string& filepath) {
	auto cleanPath = filepath;
	string::normalizeSlashes(cleanPath);
	string::toLower(cleanPath);
	cleanPath = string::trim(std::filesystem::path{cleanPath}.filename().string());

	std::vector<std::byte> buffer;
	buffer.resize(TAB_FILENAME_MAX_SIZE);
	std::memset(buffer.data(), 0, TAB_FILENAME_MAX_SIZE);
	std::memcpy(buffer.data(), cleanPath.c_str(), cleanPath.size());

	buffer.push_back(std::byte{0x80});
	if (buffer.size() % 64 < 56) {
		buffer.resize(buffer.size() + (56 - buffer.size() % 64));
	} else if (buffer.size() % 64 > 56) {
		buffer.resize(buffer.size() + (64 - buffer.size() % 64) + 56);
	}
	for (int i = 7; i >= 0; i--) {
		buffer.push_back(static_cast<std::byte>(static_cast<uint64_t>(TAB_FILENAME_MAX_SIZE) * 8 >> i * 8 & 0xff));
	}

	for (std::span bufferU32{reinterpret_cast<uint32_t*>(buffer.data()), buffer.size() / sizeof(uint32_t)}; auto& uint : bufferU32) {
		BufferStream::swap_endian(&uint);
	}
	hash_state sha1;
	sha1_init(&sha1);
	sha1_process(&sha1, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());
	BufferStream::swap_endian(&sha1.sha1.state[0]);
	return sha1.sha1.state[0];
}
