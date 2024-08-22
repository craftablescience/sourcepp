#include <vpkpp/format/ZIP.h>

#include <cstring>
#include <filesystem>

#include <mz.h>
#include <mz_strm.h>
#ifdef VPKPP_ZIP_COMPRESSION
#include <mz_strm_lzma.h>
#endif
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <FileStream.h>
#include <sourcepp/crypto/CRC32.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace vpkpp;

ZIP::ZIP(const std::string& fullFilePath_)
		: PackFile(fullFilePath_)
		, tempZIPPath((std::filesystem::temp_directory_path() / (string::generateUUIDv4() + ".zip")).string()) {
	this->type = PackFileType::ZIP;
}

ZIP::~ZIP() {
	this->closeZIP();
}

std::unique_ptr<PackFile> ZIP::create(const std::string& path) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};

		// I just created an empty zip with Windows and pasted the result because minizip-ng wasn't behaving
		stream
			.write('P')
			.write('K')
			.write<uint8_t>(5)
			.write<uint8_t>(6)
			.write<std::array<uint16_t, 4>>({})
			.write<std::array<uint32_t, 2>>({})
			.write<uint16_t>(0);
	}
	return ZIP::open(path);
}

std::unique_ptr<PackFile> ZIP::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* zip = new ZIP{path};
	auto packFile = std::unique_ptr<PackFile>(zip);

	if (!zip->openZIP(zip->fullFilePath)) {
		return nullptr;
	}

	for (auto code = mz_zip_goto_first_entry(zip->zipHandle); code == MZ_OK; code = mz_zip_goto_next_entry(zip->zipHandle)) {
		mz_zip_file* fileInfo = nullptr;
		if (mz_zip_entry_get_info(zip->zipHandle, &fileInfo)) {
			return nullptr;
		}
		if (mz_zip_entry_is_dir(zip->zipHandle) == MZ_OK) {
			continue;
		}

		auto entryPath = zip->cleanEntryPath(fileInfo->filename);

		Entry entry = createNewEntry();
		entry.flags = fileInfo->compression_method;
		entry.length = fileInfo->uncompressed_size;
		entry.compressedLength = fileInfo->compressed_size;
		entry.crc32 = fileInfo->crc;

		zip->entries.insert(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	}

	return packFile;
}

std::vector<std::string> ZIP::verifyEntryChecksums() const {
	return this->verifyEntryChecksumsUsingCRC32();
}

std::optional<std::vector<std::byte>> ZIP::readEntry(const std::string& path_) const {
	auto path = this->cleanEntryPath(path_);
	auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	if (!this->streamOpen || !this->zipOpen) {
		return std::nullopt;
	}
	if (mz_zip_locate_entry(this->zipHandle, path.c_str(), !this->isCaseSensitive()) != MZ_OK) {
		return std::nullopt;
	}
	if (mz_zip_entry_read_open(this->zipHandle, 0, nullptr) != MZ_OK) {
		return std::nullopt;
	}
	std::vector<std::byte> out;
	out.resize(entry->length);
	mz_zip_entry_read(this->zipHandle, out.data(), static_cast<int>(entry->length));
	mz_zip_entry_close(this->zipHandle);
	return out;
}

void ZIP::addEntryInternal(Entry& entry, const std::string& path, std::vector<std::byte>& buffer, EntryOptions options) {
	entry.length = buffer.size();
	entry.compressedLength = 0;
	entry.crc32 = crypto::computeCRC32(buffer);
}

bool ZIP::bake(const std::string& outputDir_, BakeOptions options, const EntryCallback& callback) {
	// Get the proper file output folder
	std::string outputDir = this->getBakeOutputDir(outputDir_);
	std::string outputPath = outputDir + '/' + this->getFilename();

	// Use temp folder so we can read from the current ZIP
	if (!this->bakeTempZip(this->tempZIPPath, callback)) {
		return false;
	}
	this->mergeUnbakedEntries();

	// Close our ZIP and reopen it
	this->closeZIP();
	std::filesystem::rename(this->tempZIPPath, outputPath);
	if (!this->openZIP(outputPath)) {
		return false;
	}
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute ZIP::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH | CRC32;
}

#ifdef VPKPP_ZIP_COMPRESSION
uint16_t ZIP::getCompressionMethod() const {
	return this->options.zip_compressionMethod;
}

void ZIP::setCompressionMethod(uint16_t compressionMethod) {
	this->options.zip_compressionMethod = compressionMethod;
}
#endif

bool ZIP::bakeTempZip(const std::string& writeZipPath, const EntryCallback& callback) {
	void* writeStreamHandle;
#ifdef VPKPP_ZIP_COMPRESSION
	if (this->options.zip_compressionMethod != MZ_COMPRESS_METHOD_STORE) {
		writeStreamHandle = mz_stream_lzma_create();
	} else {
#endif
		writeStreamHandle = mz_stream_os_create();
#ifdef VPKPP_ZIP_COMPRESSION
	}
#endif
	if (mz_stream_open(writeStreamHandle, writeZipPath.c_str(), MZ_OPEN_MODE_CREATE | MZ_OPEN_MODE_WRITE)) {
		return false;
	}

	void* writeZipHandle = mz_zip_writer_create();
	if (mz_zip_writer_open(writeZipHandle, writeStreamHandle, 0)) {
		return false;
	}

#ifdef VPKPP_ZIP_COMPRESSION
	if (this->options.zip_compressionMethod != MZ_COMPRESS_METHOD_STORE) {
		mz_zip_writer_set_compress_level(writeZipHandle, MZ_COMPRESS_LEVEL_DEFAULT);
		mz_zip_writer_set_compress_method(writeZipHandle, MZ_COMPRESS_METHOD_LZMA);
	}
#endif

	bool noneFailed = true;
	this->runForAllEntries([this, &callback, writeZipHandle, &noneFailed](const std::string& path, const Entry& entry) {
		auto binData = this->readEntry(path);
		if (!binData) {
			return;
		}

		mz_zip_file fileInfo;
		std::memset(&fileInfo, 0, sizeof(mz_zip_entry));
		fileInfo.flag = MZ_ZIP_FLAG_DATA_DESCRIPTOR;
		fileInfo.filename = path.c_str();
		fileInfo.filename_size = path.length();
		fileInfo.uncompressed_size = static_cast<int64_t>(entry.length);
		fileInfo.compressed_size = static_cast<int64_t>(entry.compressedLength);
		fileInfo.crc = entry.crc32;
		fileInfo.compression_method = MZ_COMPRESS_METHOD_STORE;
		if (mz_zip_writer_add_buffer(writeZipHandle, binData->data(), static_cast<int>(binData->size()), &fileInfo)) {
			noneFailed = false;
			return;
		}

		if (callback) {
			callback(path, entry);
		}
	});

	if (mz_zip_writer_close(writeZipHandle)) {
		return false;
	}
	mz_zip_writer_delete(&writeZipHandle);

	if (mz_stream_close(writeStreamHandle)) {
		return false;
	}
	mz_stream_delete(&writeStreamHandle);

	return noneFailed;
}

bool ZIP::openZIP(std::string_view path) {
	this->streamHandle = mz_stream_os_create();
	if (mz_stream_open(this->streamHandle, path.data(), MZ_OPEN_MODE_READ) != MZ_OK) {
		mz_stream_delete(&this->streamHandle);
		return false;
	}
	this->streamOpen = true;

	this->zipHandle = mz_zip_create();
	if (mz_zip_open(this->zipHandle, this->streamHandle, MZ_OPEN_MODE_READ) != MZ_OK) {
		mz_zip_delete(&this->zipHandle);
		return false; // No need to delete the stream, it's done when we destruct
	}
	this->zipOpen = true;

	return true;
}

void ZIP::closeZIP() {
	if (this->zipOpen) {
		mz_zip_close(this->zipHandle);
		mz_zip_delete(&this->zipHandle);
	}
	if (this->streamOpen) {
		mz_stream_close(this->streamHandle);
		mz_stream_delete(&this->streamHandle);
	}
}
