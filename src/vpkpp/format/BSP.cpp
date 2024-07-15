#include <vpkpp/format/BSP.h>

#include <filesystem>

#include <FileStream.h>
#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <sourcepp/fs/FS.h>
#include <sourcepp/string/String.h>

using namespace sourcepp;
using namespace vpkpp;

BSP::BSP(const std::string& fullFilePath_, PackFileOptions options_)
		: ZIP(fullFilePath_, options_)
		, tempBSPPakLumpPath((std::filesystem::temp_directory_path() / (string::generateUUIDv4() + ".zip")).string()) {
	this->type = PackFileType::BSP;
}

std::unique_ptr<PackFile> BSP::open(const std::string& path, PackFileOptions options, const Callback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* bsp = new BSP{path, options};
	auto packFile = std::unique_ptr<PackFile>(bsp);

	FileStream reader{bsp->fullFilePath};
	reader.seek_in(0);

	reader.read(bsp->header.signature);
	if (bsp->header.signature != BSP_SIGNATURE) {
		// File is not a BSP
		return nullptr;
	}
	reader.read(bsp->header.version);
	reader.read(bsp->header.lumps);
	reader.read(bsp->header.mapRevision);

	if (bsp->header.lumps[static_cast<int32_t>(BSPLump::PAKFILE)].offset == 0 || bsp->header.lumps[static_cast<int32_t>(BSPLump::PAKFILE)].length == 0) {
		// No paklump, create an empty zip
		void* writeStreamHandle = mz_stream_os_create();
		if (mz_stream_os_open(writeStreamHandle, bsp->tempBSPPakLumpPath.c_str(), MZ_OPEN_MODE_CREATE | MZ_OPEN_MODE_WRITE)) {
			return nullptr;
		}
		void* writeZipHandle = mz_zip_writer_create();
		if (mz_zip_writer_open(writeZipHandle, writeStreamHandle, 0)) {
			return nullptr;
		}
		if (mz_zip_writer_close(writeZipHandle)) {
			return nullptr;
		}
		mz_zip_writer_delete(&writeZipHandle);
		if (mz_stream_os_close(writeStreamHandle)) {
			return nullptr;
		}
		mz_stream_os_delete(&writeStreamHandle);
	} else {
		// Extract the paklump to a temp file
		reader.seek_in(bsp->header.lumps[static_cast<int32_t>(BSPLump::PAKFILE)].offset);
		auto binData = reader.read_bytes(bsp->header.lumps[static_cast<int32_t>(BSPLump::PAKFILE)].length);

		FileStream writer{bsp->tempBSPPakLumpPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		writer.write(binData);
	}

	if (!bsp->openZIP(bsp->tempBSPPakLumpPath)) {
		return nullptr;
	}

	for (auto code = mz_zip_goto_first_entry(bsp->zipHandle); code == MZ_OK; code = mz_zip_goto_next_entry(bsp->zipHandle)) {
		mz_zip_file* fileInfo = nullptr;
		if (mz_zip_entry_get_info(bsp->zipHandle, &fileInfo)) {
			return nullptr;
		}
		if (mz_zip_entry_is_dir(bsp->zipHandle) == MZ_OK) {
			continue;
		}

		Entry entry = createNewEntry();
		entry.path = fileInfo->filename;
		string::normalizeSlashes(entry.path, true);
		if (!bsp->isCaseSensitive()) {
			string::toLower(entry.path);
		}

		entry.flags = fileInfo->compression_method;
		entry.length = fileInfo->uncompressed_size;
		entry.compressedLength = fileInfo->compressed_size;
		entry.crc32 = fileInfo->crc;

		auto parentDir = std::filesystem::path{entry.path}.parent_path().string();
		string::normalizeSlashes(parentDir, true);
		if (!bsp->isCaseSensitive()) {
			string::toLower(parentDir);
		}

		if (!bsp->entries.contains(parentDir)) {
			bsp->entries[parentDir] = {};
		}
		bsp->entries[parentDir].push_back(entry);

		if (callback) {
			callback(parentDir, entry);
		}
	}

	return packFile;
}

bool BSP::bake(const std::string& outputDir_, const Callback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Use temp folder so we can read from the current ZIP
	if (!this->bakeTempZip(this->tempZIPPath, callback)) {
		return false;
	}
	this->mergeUnbakedEntries();

	// Close the ZIP
	this->closeZIP();

	// Write the pakfile lump
	this->writeLump(BSPLump::PAKFILE, fs::readFileBuffer(this->tempZIPPath));

	// If the output path is different, copy the entire BSP there
	if (outputPath != this->fullFilePath) {
		std::filesystem::copy_file(this->fullFilePath, outputPath, std::filesystem::copy_options::overwrite_existing);
	}

	// Rename and reopen the ZIP
	std::filesystem::rename(this->tempZIPPath, this->tempBSPPakLumpPath);
	if (!this->openZIP(this->tempBSPPakLumpPath)) {
		return false;
	}
	PackFile::setFullFilePath(outputDir);
	return true;
}

BSP::operator std::string() const {
	return PackFile::operator std::string() +
	       " | Version v" + std::to_string(this->header.version) +
	       " | Map Revision " + std::to_string(this->header.mapRevision);
}

std::optional<std::vector<std::byte>> BSP::readLump(BSPLump lump) const {
	auto lumpToRead = static_cast<uint32_t>(lump);
	if (this->header.lumps[lumpToRead].length == 0 || this->header.lumps[lumpToRead].offset == 0) {
		return std::nullopt;
	}
	FileStream reader{this->fullFilePath};
	reader.seek_in(this->header.lumps[lumpToRead].offset);
	return reader.read_bytes(this->header.lumps[lumpToRead].length);
}

void BSP::writeLump(BSPLump lumpIndex, const std::vector<std::byte>& data) {
	this->moveLumpToWritableSpace(lumpIndex, static_cast<int>(data.size()));

	FileStream writer{this->fullFilePath, FileStream::OPT_READ | FileStream::OPT_WRITE};
	writer.seek_out(0);

	writer.write(this->header.signature);
	writer.write(this->header.version);
	writer.write(this->header.lumps);
	writer.write(this->header.mapRevision);
	writer.seek_out(this->header.lumps[static_cast<uint32_t>(lumpIndex)].offset);
	writer.write(data);
}

void BSP::moveLumpToWritableSpace(BSPLump lumpIndex, int newSize) {
	auto lumpToMove = static_cast<uint32_t>(lumpIndex);
	this->header.lumps[lumpToMove].length = newSize;

	// If the zip is at the end of the file we just overwrite it, otherwise we have to shift some lumps over
	std::vector<int> lumpsAfterPaklumpIndices;
	for (int i = 0; i < this->header.lumps.size(); i++) {
		if (this->header.lumps[i].offset > this->header.lumps[lumpToMove].offset) {
			lumpsAfterPaklumpIndices.push_back(i);
		}
	}
	if (lumpsAfterPaklumpIndices.empty()) {
		return;
	}

	// Get the exact area to move
	int moveOffsetStart = INT_MAX, moveOffsetEnd = 0;
	for (int lumpsAfterPaklumpIndex : lumpsAfterPaklumpIndices) {
		if (this->header.lumps[lumpsAfterPaklumpIndex].offset < moveOffsetStart) {
			moveOffsetStart = this->header.lumps[lumpsAfterPaklumpIndex].offset;
		}
		if (auto offsetAndLength = this->header.lumps[lumpsAfterPaklumpIndex].offset + this->header.lumps[lumpsAfterPaklumpIndex].length; offsetAndLength > moveOffsetEnd) {
			moveOffsetEnd = offsetAndLength;
		}
	}

	// Get where to move it
	int lastLumpBeforePaklumpOffset = 0, lastLumpBeforePaklumpLength = 0;
	for (const Lump& lump : this->header.lumps) {
		if (lump.offset < this->header.lumps[lumpToMove].offset && lump.offset > lastLumpBeforePaklumpOffset) {
			lastLumpBeforePaklumpOffset = lump.offset;
			lastLumpBeforePaklumpLength = lump.length;
		}
	}

	// Move all the lumps after paklump back
	FileStream bsp{this->fullFilePath, FileStream::OPT_READ | FileStream::OPT_WRITE};
	bsp.seek_in(moveOffsetStart);
	auto lumpsData = bsp.read_bytes(moveOffsetEnd - moveOffsetStart);
	bsp.seek_out(lastLumpBeforePaklumpOffset + lastLumpBeforePaklumpLength);
	bsp.write(lumpsData);

	// Fix the offsets
	for (int lumpsAfterPaklumpIndex : lumpsAfterPaklumpIndices) {
		this->header.lumps[lumpsAfterPaklumpIndex].offset -= newSize;
	}
	this->header.lumps[lumpToMove].offset = lastLumpBeforePaklumpOffset + lastLumpBeforePaklumpLength + static_cast<int>(lumpsData.size());
}
