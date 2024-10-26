#include <vpkpp/format/VPK_VTMB.h>

#include <filesystem>

#include <FileStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

VPK_VTMB::VPK_VTMB(const std::string& fullFilePath_)
		: PackFile(fullFilePath_) {
	this->type = PackFileType::VPK_VTMB;
}

std::unique_ptr<PackFile> VPK_VTMB::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream
			.write<uint32_t>(0)
			.write<uint32_t>(0)
			.write<uint8_t>(0);
	}
	return VPK_VTMB::open(path);
}

std::unique_ptr<PackFile> VPK_VTMB::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	// Extra check to make sure this is a VTMB VPK path
	const auto stem = std::filesystem::path{path}.stem().string();
	if (stem.length() != 7 || !stem.starts_with("pack") || !parser::text::isNumber(stem.substr(4))) {
		return nullptr;
	}

	auto* vpkVTMB = new VPK_VTMB{path};
	auto packFile = std::unique_ptr<PackFile>(vpkVTMB);

	for (int i = 0; i <= 9; i++) {
		if (!std::filesystem::exists(vpkVTMB->getTruncatedFilepath() + string::padNumber(i * 100, 3) + VPK_VTMB_EXTENSION.data())) {
			break;
		}
		for (int j = 0; j <= 99; j++) {
			auto numberedPath = vpkVTMB->getTruncatedFilepath() + string::padNumber(i * 100 + j, 3) + VPK_VTMB_EXTENSION.data();
			if (!std::filesystem::exists(numberedPath)) {
				break;
			}
			vpkVTMB->openNumbered(i * 100 + j, numberedPath, callback);
		}
	}

	vpkVTMB->currentArchive++;
	return packFile;
}

void VPK_VTMB::openNumbered(uint32_t archiveIndex, const std::string& path, const EntryCallback& callback) {
	FileStream reader{path};
	reader.seek_in(sizeof(uint32_t) * 2 + sizeof(uint8_t), std::ios::end);

	auto fileCount = reader.read<uint32_t>();
	auto dirOffset = reader.read<uint32_t>();

	// Make 100% sure
	auto version = reader.read<uint8_t>();
	if (version != 0) {
		return;
	}

	// Ok now let's load this thing
	this->knownArchives.push_back(archiveIndex);
	if (archiveIndex > this->currentArchive) {
		this->currentArchive = archiveIndex;
	}

	reader.seek_in(dirOffset);
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();
		entry.archiveIndex = archiveIndex;

		auto entryPath = this->cleanEntryPath(reader.read_string(reader.read<uint32_t>()));

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		this->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}
}

std::optional<std::vector<std::byte>> VPK_VTMB::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->getTruncatedFilepath() + string::padNumber(entry->archiveIndex, 3) + VPK_VTMB_EXTENSION.data()};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->length);
}

void VPK_VTMB::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.archiveIndex = this->currentArchive;
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool VPK_VTMB::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	if (this->knownArchives.empty()) {
		return false;
	}

	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPathStem = outputDir + '/' + this->getTruncatedFilestem();

	// Copy files to temp dir and change current path
	auto tempDir = std::filesystem::temp_directory_path() / string::generateUUIDv4();
	std::error_code ec;
	if (!std::filesystem::create_directory(tempDir, ec)) {
		return false;
	}
	ec.clear();
	for (auto vpkIndex : this->knownArchives) {
		std::filesystem::copy(outputPathStem + string::padNumber(vpkIndex, 3) + VPK_VTMB_EXTENSION.data(), tempDir, ec);
		if (ec) {
			return false;
		}
		ec.clear();
	}
	this->fullFilePath = (tempDir / (this->getTruncatedFilestem())).string() + string::padNumber(this->knownArchives[0], 3) + VPK_VTMB_EXTENSION.data();

	// Reconstruct data for ease of access
	std::unordered_map<uint16_t, std::vector<std::pair<std::string, Entry*>>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		if (!entriesToBake.contains(entry.archiveIndex)) {
			entriesToBake[entry.archiveIndex] = {};
		}
		entriesToBake[entry.archiveIndex].emplace_back(path, &entry);
	});

	for (auto& [archiveIndex, entriesToBakeInArchive] : entriesToBake) {
		FileStream stream{outputPathStem + string::padNumber(archiveIndex, 3) + VPK_VTMB_EXTENSION.data(), FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// File data
		for (auto& [path, entry] : entriesToBakeInArchive) {
			if (auto binData = this->readEntry(path)) {
				entry->offset = stream.tell_out();
				stream.write(*binData);
			} else {
				entry->offset = 0;
				entry->length = 0;
			}
		}

		// Directory
		auto dirOffset = stream.tell_out();
		for (const auto& [path, entry] : entriesToBakeInArchive) {
			stream.write<uint32_t>(path.length());
			stream.write(path, false);
			stream.write<uint32_t>(entry->offset);
			stream.write<uint32_t>(entry->length);

			if (callback) {
				callback(path, *entry);
			}
		}

		// Footer
		stream.write<uint32_t>(entriesToBakeInArchive.size());
		stream.write<uint32_t>(dirOffset);
		stream.write<uint8_t>(0);
	}

	if (entriesToBake.contains(this->currentArchive)) {
		this->currentArchive++;
	}

	// Clean up
	this->mergeUnbakedEntries();
	std::filesystem::remove_all(tempDir, ec);
	PackFile::setFullFilePath(outputDir);
	return true;
}

std::string VPK_VTMB::getTruncatedFilestem() const {
	// Ok so technically these things can have names besides packXXX, but the game won't recognize it...
	// and I check the filename starts with "pack" in VPK_VTMB::open, so I'm allowed to do this :P
	return "pack";
}

Attribute VPK_VTMB::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | ARCHIVE_INDEX;
}
