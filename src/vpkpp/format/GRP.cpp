#include <vpkpp/format/GRP.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/fs/FS.h>
#include <sourcepp/string/String.h>

using namespace sourcepp;
using namespace vpkpp;

GRP::GRP(const std::string& fullFilePath_, PackFileOptions options_)
		: PackFile(fullFilePath_, options_) {
	this->type = PackFileType::GRP;
}

std::unique_ptr<PackFile> GRP::open(const std::string& path, PackFileOptions options, const Callback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* grp = new GRP{path, options};
	auto packFile = std::unique_ptr<PackFile>(grp);

	FileStream reader{grp->fullFilePath};
	reader.seek_in(0);

	auto signature = reader.read_bytes<GRP_SIGNATURE.length()>();
	for (int i = 0; i < signature.size(); i++) {
		if (static_cast<unsigned char>(signature[i]) != GRP_SIGNATURE[i]) {
			// File is not a GRP
			return nullptr;
		}
	}

	auto fileCount = reader.read<uint32_t>();

	std::vector<Entry> entries;
	for (int i = 0; i < fileCount; i++) {
		Entry entry = createNewEntry();

		reader.read(entry.path, GRP_FILENAME_MAX_SIZE);
		string::normalizeSlashes(entry.path);
		if (!grp->isCaseSensitive()) {
			string::toLower(entry.path);
		}

		entry.length = reader.read<uint32_t>();

		entries.push_back(entry);
	}

	// At this point we've reached the file data section, calculate the offsets and then add the entries
	std::size_t offset = reader.tell_in();
	if (!grp->entries.contains("")) {
		grp->entries[""] = {};
	}
	for (auto& entry : entries) {
		entry.offset = offset;
		offset += entry.length;

		grp->entries[""].push_back(entry);

		if (callback) {
			callback("", entry);
		}
	}

	return packFile;
}

std::optional<std::vector<std::byte>> GRP::readEntry(const Entry& entry) const {
	if (entry.unbaked) {
		// Get the stored data
		for (const auto& [unbakedEntryDir, unbakedEntryList] : this->unbakedEntries) {
			for (const Entry& unbakedEntry : unbakedEntryList) {
				if (unbakedEntry.path == entry.path) {
					std::vector<std::byte> unbakedData;
					if (isEntryUnbakedUsingByteBuffer(unbakedEntry)) {
						unbakedData = std::get<std::vector<std::byte>>(getEntryUnbakedData(unbakedEntry));
					} else {
						unbakedData = fs::readFileBuffer(std::get<std::string>(getEntryUnbakedData(unbakedEntry)));
					}
					return unbakedData;
				}
			}
		}
		return std::nullopt;
	}
	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry.offset);
	return stream.read_bytes(entry.length);
}

Entry& GRP::addEntryInternal(Entry& entry, const std::string& filename_, std::vector<std::byte>& buffer, EntryOptions options_) {
	auto filename = filename_;
	if (!this->isCaseSensitive()) {
		string::toLower(filename);
	}

	entry.path = filename;
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;

	if (!this->unbakedEntries.contains("")) {
		this->unbakedEntries[""] = {};
	}
	this->unbakedEntries.at("").push_back(entry);
	return this->unbakedEntries.at("").back();
}

bool GRP::bake(const std::string& outputDir_, const Callback& callback) {
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

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	for (auto* entry : entriesToBake) {
		if (auto binData = this->readEntry(*entry)) {
			fileData.insert(fileData.end(), binData->begin(), binData->end());
		} else {
			entry->length = 0;
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// Signature
		stream.write(std::string{GRP_SIGNATURE}, false);

		// Number of files
		stream.write(static_cast<uint32_t>(entriesToBake.size()));

		// File tree
		for (auto entry : entriesToBake) {
			stream.write(entry->path, false, GRP_FILENAME_MAX_SIZE);
			stream.write(static_cast<uint32_t>(entry->length));

			if (callback) {
				callback(entry->getParentPath(), *entry);
			}
		}

		// Fix offsets
		std::size_t offset = stream.tell_out();
		for (auto* entry : entriesToBake) {
			entry->offset = offset;
			offset += entry->length;
		}

		// File data
		stream.write(fileData);
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

std::vector<Attribute> GRP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return {LENGTH};
}
