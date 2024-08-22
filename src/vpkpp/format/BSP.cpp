#include <vpkpp/format/BSP.h>

#include <filesystem>

#include <FileStream.h>
#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

using bsppp::BSPLump;

BSP::BSP(const std::string& fullFilePath_)
		: ZIP(fullFilePath_)
		, bsppp::BSP(fullFilePath_)
		, tempBSPPakLumpPath((std::filesystem::temp_directory_path() / (string::generateUUIDv4() + ".zip")).string()) {
	this->type = PackFileType::BSP;
}

std::unique_ptr<PackFile> BSP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* bsp = new BSP{path};
	auto packFile = std::unique_ptr<PackFile>(bsp);

	if (!(*bsp)) {
		// File failed to load, or has an invalid signature
		return nullptr;
	}

	if (auto pakFileLump = bsp->readLump(BSPLump::PAKFILE)) {
		// Extract the paklump to a temp file
		FileStream writer{bsp->tempBSPPakLumpPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		writer.write(*pakFileLump);
	} else {
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

		auto entryPath = bsp->cleanEntryPath(fileInfo->filename);

		Entry entry = createNewEntry();
		entry.flags = fileInfo->compression_method;
		entry.length = fileInfo->uncompressed_size;
		entry.compressedLength = fileInfo->compressed_size;
		entry.crc32 = fileInfo->crc;

		bsp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

bool BSP::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
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
	this->path = this->fullFilePath;
	return true;
}

BSP::operator std::string() const {
	return PackFile::operator std::string() +
	       " | Version v" + std::to_string(this->header.version) +
	       " | Map Revision " + std::to_string(this->header.mapRevision);
}
