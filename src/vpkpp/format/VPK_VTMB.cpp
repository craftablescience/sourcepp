#include <vpkpp/format/VPK_VTMB.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/parser/Text.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

VPK_VTMB::VPK_VTMB(const std::string& fullFilePath_, PackFileOptions options_)
		: PackFile(fullFilePath_, options_) {
	this->type = PackFileType::VPK_VTMB;
}

std::unique_ptr<PackFile> VPK_VTMB::open(const std::string& path, PackFileOptions options, const Callback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	// Extra check to make sure this is a VTMB VPK path
	auto stem = std::filesystem::path{path}.stem().string();
	if (stem.length() != 7 || !stem.starts_with("pack") || !parser::text::isNumber(stem.substr(4))) {
		return nullptr;
	}

	auto* vpkVTMB = new VPK_VTMB{path, options};
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

void VPK_VTMB::openNumbered(uint16_t archiveIndex, const std::string& path, const Callback& callback) {
	FileStream reader{path};
	reader.seek_in(-static_cast<int64_t>(sizeof(uint32_t) * 2 + sizeof(uint8_t)), std::ios::end);

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

		entry.path = reader.read_string(reader.read<uint32_t>());
		string::normalizeSlashes(entry.path, true);
		if (!this->isCaseSensitive()) {
			string::toLower(entry.path);
		}

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		auto parentDir = std::filesystem::path{entry.path}.parent_path().string();
		if (!this->entries.contains(parentDir)) {
			this->entries[parentDir] = {};
		}
		this->entries[parentDir].push_back(entry);

		if (callback) {
			callback(parentDir, entry);
		}
	}
}

std::optional<std::vector<std::byte>> VPK_VTMB::readEntry(const Entry& entry) const {
	if (entry.unbaked) {
		return this->readUnbakedEntry(entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->getTruncatedFilepath() + string::padNumber(entry.archiveIndex, 3) + VPK_VTMB_EXTENSION.data()};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry.offset);
	return stream.read_bytes(entry.length);
}

Entry& VPK_VTMB::addEntryInternal(Entry& entry, const std::string& filename_, std::vector<std::byte>& buffer, EntryOptions options_) {
	auto filename = filename_;
	if (!this->isCaseSensitive()) {
		string::toLower(filename);
	}
	auto [dir, name] = splitFilenameAndParentDir(filename);

	entry.archiveIndex = this->currentArchive;
	entry.path = filename;
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;

	if (!this->unbakedEntries.contains(dir)) {
		this->unbakedEntries[dir] = {};
	}
	this->unbakedEntries.at(dir).push_back(entry);
	return this->unbakedEntries.at(dir).back();
}

bool VPK_VTMB::bake(const std::string& outputDir_, const Callback& callback) {
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
	std::unordered_map<uint16_t, std::vector<Entry*>> entriesToBake;
	for (auto& [entryDir, entryList] : this->entries) {
		for (auto& entry : entryList) {
			if (!entriesToBake.contains(entry.archiveIndex)) {
				entriesToBake[entry.archiveIndex] = {};
			}
			entriesToBake[entry.archiveIndex].push_back(&entry);
		}
	}
	for (auto& [entryDir, entryList] : this->unbakedEntries) {
		for (auto& entry : entryList) {
			if (!entriesToBake.contains(entry.archiveIndex)) {
				entriesToBake[entry.archiveIndex] = {};
			}
			entriesToBake[entry.archiveIndex].push_back(&entry);
		}
	}

	for (auto& [archiveIndex, entriesToBakeInArchive] : entriesToBake) {
		FileStream stream{outputPathStem + string::padNumber(archiveIndex, 3) + VPK_VTMB_EXTENSION.data(), FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// File data
		for (auto* entry : entriesToBakeInArchive) {
			if (auto binData = this->readEntry(*entry)) {
				entry->offset = stream.tell_out();
				stream.write(*binData);
			} else {
				entry->offset = 0;
				entry->length = 0;
			}
		}

		// Directory
		auto dirOffset = stream.tell_out();
		for (auto* entry : entriesToBakeInArchive) {
			stream.write<uint32_t>(entry->path.length());
			stream.write(entry->path, false);
			stream.write<uint32_t>(entry->offset);
			stream.write<uint32_t>(entry->length);

			if (callback) {
				callback(entry->getParentPath(), *entry);
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

std::vector<Attribute> VPK_VTMB::getSupportedEntryAttributes() const {
	using enum Attribute;
	return {LENGTH, ARCHIVE_INDEX};
}
