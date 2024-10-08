#include <vpkpp/format/BSP.h>

#include <filesystem>

#include <bsppp/bsppp.h>
#include <FileStream.h>
#include <mz.h>
#include <mz_zip.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

using bsppp::BSPLump;

BSP::BSP(const std::string& fullFilePath_)
		: ZIP(fullFilePath_)
		, tempPakLumpPath((std::filesystem::temp_directory_path() / (string::generateUUIDv4() + ".zip")).string()) {
	this->type = PackFileType::BSP;
}

BSP::~BSP() {
	// Pull this in from the ZIP dtor, have to do it before deleting the file
	this->closeZIP();
	std::filesystem::remove(this->tempPakLumpPath);
}

std::unique_ptr<PackFile> BSP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* bsp = new BSP{path};
	auto packFile = std::unique_ptr<PackFile>(bsp);

	{
		bsppp::BSP reader{path};
		if (!reader) {
			// File failed to load, or has an invalid signature
			return nullptr;
		}

		bsp->version = reader.getVersion();
		bsp->mapRevision = reader.getMapRevision();

		if (auto pakFileLump = reader.readLump(BSPLump::PAKFILE)) {
			// Extract the paklump to a temp file
			FileStream writer{bsp->tempPakLumpPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
			writer.write(*pakFileLump);
		} else {
			// No paklump, create an empty zip
			if (!ZIP::create(bsp->tempPakLumpPath)) {
				return nullptr;
			}
		}
	}

	if (!bsp->openZIP(bsp->tempPakLumpPath)) {
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

		bsp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

bool BSP::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// If the output path is different, copy the entire BSP there
	if (outputPath != this->fullFilePath) {
		std::filesystem::copy_file(this->fullFilePath, outputPath, std::filesystem::copy_options::overwrite_existing);
		PackFile::setFullFilePath(outputDir);
	}

	// Use temp folder so we can read from the current ZIP
	if (!this->bakeTempZip(this->tempZIPPath, options, callback)) {
		return false;
	}
	this->mergeUnbakedEntries();

	// Close the ZIP
	this->closeZIP();

	// Write the new lump
	{
		bsppp::BSP writer{this->fullFilePath};
		if (!writer) {
			return false;
		}
		writer.writeLump(BSPLump::PAKFILE, fs::readFileBuffer(this->tempZIPPath));
	}

	// Rename and reopen the ZIP
	std::filesystem::rename(this->tempZIPPath, this->tempPakLumpPath);
	return this->openZIP(this->tempPakLumpPath);
}

BSP::operator std::string() const {
	return PackFile::operator std::string() +
	       " | Version v" + std::to_string(this->version) +
	       " | Map Revision " + std::to_string(this->mapRevision);
}
