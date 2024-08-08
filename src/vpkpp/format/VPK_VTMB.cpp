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

	FileStream reader{vpkVTMB->fullFilePath};
	reader.seek_in(-static_cast<int64_t>(sizeof(uint32_t) * 2 + sizeof(uint8_t)), std::ios::end);

	auto fileCount = reader.read<uint32_t>();
	auto dirOffset = reader.read<uint32_t>();

	// Make 100% sure
	auto version = reader.read<uint8_t>();
	if (version != 0) {
		return nullptr;
	}

	// Ok now let's load this thing
	reader.seek_in(dirOffset);
	for (uint32_t i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		entry.path = reader.read_string(reader.read<uint32_t>());
		string::normalizeSlashes(entry.path, true);
		if (!vpkVTMB->isCaseSensitive()) {
			string::toLower(entry.path);
		}

		entry.offset = reader.read<uint32_t>();
		entry.length = reader.read<uint32_t>();

		auto parentDir = std::filesystem::path{entry.path}.parent_path().string();
		if (!vpkVTMB->entries.contains(parentDir)) {
			vpkVTMB->entries[parentDir] = {};
		}
		vpkVTMB->entries[parentDir].push_back(entry);

		if (callback) {
			callback(parentDir, entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> VPK_VTMB::readEntry(const Entry& entry) const {
	if (entry.unbaked) {
		return this->readUnbakedEntry(entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
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
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<Entry*> entriesToBake;
	for (auto& [entryDir, entryList] : this->entries) {
		for (auto& entry : entryList) {
			entriesToBake.push_back(&entry);
		}
	}
	for (auto& [entryDir, entryList] : this->unbakedEntries) {
		for (auto& entry : entryList) {
			entriesToBake.push_back(&entry);
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// File data
		for (auto* entry : entriesToBake) {
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
		for (auto entry : entriesToBake) {
			stream.write<uint32_t>(entry->path.length());
			stream.write(entry->path, false);
			stream.write<uint32_t>(entry->offset);
			stream.write<uint32_t>(entry->length);

			if (callback) {
				callback(entry->getParentPath(), *entry);
			}
		}

		// Footer
		stream.write<uint32_t>(this->entries.size() + this->unbakedEntries.size());
		stream.write<uint32_t>(dirOffset);
		stream.write<uint8_t>(0);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

std::vector<Attribute> VPK_VTMB::getSupportedEntryAttributes() const {
	using enum Attribute;
	return {LENGTH};
}
