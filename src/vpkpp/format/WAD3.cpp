//
// Header: (size=12)
//	fourcc identity
//	int32 lump_count	// Number of lump infos in the array
//	int32 lump_offset	// Offset to the start of the lump info array
//
// NOTE: Are the compression fields actually used in WAD3?
// Lump Infos: (size=32)
//	int32 offset
//	int32 size
//	int32 size_uncompressed
//	int8  type
//	int8  compression
//	int16 padding
//	char  name[16]
//
#include <vpkpp/format/WAD3.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

enum WAD3FileTypes
{
	WFT_FIRST = 64,

	WFT_PALETTE = WFT_FIRST,
	WFT_COLORMAP,
	WFT_QPIC,
	WFT_MIPTEX,
	WFT_RAW,
	WFT_COLORMAP2,
	WFT_FONT,

	WFT_COUNT
};

constexpr std::string_view k_FileTypeNames[] = {
	"palette",
	"colormap",
	"qpic",
	"miptex",
	"raw",
	"colormap2",
	"font",
};

WAD3::WAD3(const std::string& fullFilePath_)
		: PackFile(fullFilePath_) {
	this->type = PackFileType::WAD3;
}

std::unique_ptr<PackFile> WAD3::create(const std::string& path) {
	FileStream stream { path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT };
	stream
		.write(WAD3_SIGNATURE)
		.write<uint32_t>(0)
		.write<uint32_t>(0);
	return std::unique_ptr<PackFile>(new WAD3{path});
}

std::unique_ptr<PackFile> WAD3::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* wad = new WAD3{path};
	auto packFile = std::unique_ptr<PackFile>(wad);

	FileStream reader{wad->fullFilePath};
	reader.seek_in(0);

	// Verify the identity
	if (auto signature = reader.read<uint32_t>(); signature != WAD3_SIGNATURE) {
		return nullptr;
	}

	// Treating counts as unsigned to simplify some logic... No reason to really have a negative count here anyways
	auto lumpCount  = reader.read<uint32_t>();
	auto lumpOffset = reader.read<uint32_t>();

	// Read in all lump entries
	reader.seek_in(lumpOffset);
	for (uint32_t i = 0; i < lumpCount; i++) {
		// Read all entry data
		auto offset				= reader.read<int32_t>();
		auto size				= reader.read<int32_t>();
		auto size_uncompressed	= reader.read<int32_t>();
		auto type				= reader.read<int8_t>();
		auto compression		= reader.read<int8_t>();
		auto padding			= reader.read<int16_t>();
		auto name				= reader.read_string(WAD3_FILENAME_MAX_SIZE);

		// We'll append the type onto the name as an extension
		if(type >= WFT_FIRST && type < WFT_COUNT) {
			name += ".";
			name += k_FileTypeNames[type - WFT_FIRST];
		}

		// Create the entry
		Entry entry = createNewEntry();
		entry.offset = offset;
		entry.length = size_uncompressed;
		entry.compressedLength = size;
		entry.unbaked = false;

		auto entryPath = wad->cleanEntryPath(name);
		wad->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

// Read and return entry file data from disk or from memory
std::optional<std::vector<std::byte>> WAD3::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}

	// We have it in memory, no need to read from disk
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk. Open it, read it, send it back
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);
	return stream.read_bytes(entry->compressedLength);
}

void WAD3::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.length = entry.compressedLength = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool WAD3::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// File output stream
	FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	stream.seek_out(0);

	// File data to be appended at the end
	std::vector<std::byte> fileData;

	// We'll quickly tally up and preallocate our vector so we don't need to do extra allocations at run time
	uint32_t totalDataLength = 0;
	for (auto [_, entry] : this->iterate())
		totalDataLength += entry->compressedLength;
	fileData.reserve(totalDataLength);

	// Counter to keep track of where we're writing data to
	uint32_t offsetpos = sizeof(uint32_t) * 3;

	// Write out the header
	stream.write(WAD3_SIGNATURE);
	stream.write<uint32_t>(this->getEntryCount());
	stream.write<uint32_t>(offsetpos);

	// Push forward to end of lump info array
	offsetpos += this->getEntryCount() * 32;


	for (auto [path, entry] : this->iterate()) {

		// Append the lump data to the end of the data vector
		if (auto binData = this->readEntry(path)) {
			entry->offset = offsetpos + fileData.size();
			fileData.insert(fileData.end(), binData->begin(), binData->end());
		}
		else {
			entry->offset = 0;
			entry->length = 0;
			entry->compressedLength = 0;
		}

		// Convert the extension back into the type
		int type = 0;
		std::size_t pos = path.find_last_of('.');
		if (pos > 0) {
			std::string_view ext = path.c_str() + pos + 1;
			for (int i = WFT_FIRST; i < WFT_COUNT; i++) {
				if (string::iequals(ext, k_FileTypeNames[i - WFT_FIRST])) {
					type = i;
					break;
				}
			}
			// Chop off the extension
			path = path.substr(0, pos);
		}

		// Write the lump header
		stream.write<uint32_t>(entry->offset);
		stream.write<uint32_t>(entry->compressedLength);
		stream.write<uint32_t>(entry->length);
		stream.write<uint8_t>(type);  // type
		stream.write<uint8_t>(0);  // compression
		stream.write<uint16_t>(0); // padding
		stream.write(path, false, WAD3_FILENAME_MAX_SIZE);

		if (callback) {
			callback(path, *entry);
		}
	}

	// File data
	stream.write(fileData);

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute WAD3::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}
