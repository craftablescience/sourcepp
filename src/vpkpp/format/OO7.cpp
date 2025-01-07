#include <vpkpp/format/OO7.h>

#include <filesystem>

#include <FileStream.h>
#include <miniz.h>
#include <sourcepp/crypto/MD5.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> OO7::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* oo7 = new OO7{path};
	auto packFile = std::unique_ptr<PackFile>(oo7);

	FileStream reader{oo7->fullFilePath};
	reader.seek_in(0);

	reader >> oo7->majorVersion >> oo7->minorVersion;
	if (oo7->majorVersion != 1 || (oo7->minorVersion != 1 && oo7->minorVersion != 3)) {
		// We only support v1.1 and v1.3
		return nullptr;
	}

	std::vector<std::string> v3EntriesFixup;

	// Read the file tree recursively
	std::function<void(const std::string&, bool)> readDir;
	readDir = [&callback, &oo7, &reader, &v3EntriesFixup, &readDir](const std::string& parentPath, bool root) {
		auto currentPath = parentPath + '/' + reader.read_string(reader.read<uint32_t>(), false);
		if (root) {
			currentPath = ""; // The root folder gets ignored
		}
		auto subDirCount = reader.read<uint32_t>();
		if (oo7->minorVersion == 3) {
			reader.skip_in<uint32_t>(); // File count
		}
		while (1) {
			auto filenameSize = reader.read<uint32_t>();
			if (!filenameSize) {
				break;
			}

			Entry entry = createNewEntry();

			auto entryPath = oo7->cleanEntryPath(currentPath + '/' + reader.read_string(filenameSize, false));

			bool compressed = reader.read<uint8_t>();
			entry.length = reader.read<uint32_t>();
			if (compressed) {
				entry.compressedLength = reader.read<uint32_t>();
			} else {
				reader.skip_in<uint32_t>();
			}

			if (oo7->minorVersion == 1) {
				entry.offset = reader.tell_in();

				if (compressed) {
					reader.skip_in(entry.compressedLength);
				} else {
					reader.skip_in(entry.length);
				}
			} else {
				v3EntriesFixup.push_back(entryPath);
			}

			oo7->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
		for (uint32_t i = 0; i < subDirCount; i++) {
			readDir(currentPath, false);
		}
	};
	readDir("", true);

	for (const auto& entryPath : v3EntriesFixup) {
		auto& entry = oo7->entries.at(entryPath);
		entry.offset = reader.tell_in();
		if (!entry.compressedLength) {
			reader.skip_in(entry.length);
		} else {
			reader.skip_in(entry.compressedLength);
		}
	}

	// Absolutely no idea what the demo checksum is calculated from, and if it's
	// the file tree minus file data that is insane. It technically has one, but
	// I'm not going to advertise that to users of this library.
	if (oo7->minorVersion == 3 && reader.read<uint32_t>() == 16) {
		oo7->hasChecksum = true;
		reader >> oo7->checksum;
	}

	return packFile;
}

bool OO7::hasPackFileChecksum() const {
	return this->hasChecksum;
}

bool OO7::verifyPackFileChecksum() const {
	if (!this->hasChecksum) {
		return true;
	}

	// Looping here is faster than parsing the tree again
	// We're assuming the first entry on disk is directly after the tree
	if (this->entries.empty()) {
		return true;
	}
	uint32_t minOffset = UINT32_MAX;
	this->runForAllEntries([&minOffset](const std::string&, const Entry& entry) {
		minOffset = (entry.offset < minOffset) ? entry.offset : minOffset;
	});
	auto data = fs::readFileBuffer(this->fullFilePath, sizeof(uint32_t) * 2);
	data.resize(minOffset - (sizeof(uint32_t) * 2));
	return crypto::computeMD5(data) == this->checksum;
}

std::optional<std::vector<std::byte>> OO7::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	if (!entry->compressedLength) {
		return stream.read_bytes(entry->length);
	}

	// Decompress
	auto compressedData = stream.read_bytes(entry->compressedLength);
	mz_ulong uncompressedLength = entry->length;
	std::vector<std::byte> uncompressedData(uncompressedLength);
	if (mz_uncompress(reinterpret_cast<unsigned char*>(uncompressedData.data()), &uncompressedLength, reinterpret_cast<const unsigned char*>(compressedData.data()), entry->compressedLength) != MZ_OK) {
		return std::nullopt;
	}
	return uncompressedData;
}

Attribute OO7::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}

OO7::operator std::string() const {
	return PackFileReadOnly::operator std::string() +
		" | Version v" + std::to_string(this->majorVersion) + '.' + std::to_string(this->minorVersion);
}
