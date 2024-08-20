#include <vpkppc/PackFile.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

#include <vpkpp/PackFile.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PackFile::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_callback(const char* path, EntryCallback callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(callback, nullptr);

	auto packFile = PackFile::open(path, {}, [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	});
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PackFile::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_options_and_callback(const char* path, vpkpp_pack_file_options_t options, EntryCallback callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(callback, nullptr);

	auto packFile = PackFile::open(path, Convert::optionsFromC(options), [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	});
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_type_e vpkpp_get_type(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_PACK_FILE_TYPE_UNKNOWN);

	return static_cast<vpkpp_pack_file_type_e>(Convert::packFile(handle)->getType());
}

SOURCEPP_API vpkpp_pack_file_options_t vpkpp_get_options(vpkpp_pack_file_handle_t handle) {
	return Convert::optionsToC(Convert::packFile(handle)->getOptions());
}

SOURCEPP_API int vpkpp_has_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasEntryChecksums();
}

SOURCEPP_API sourcepp_string_array_t vpkpp_verify_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	return Convert::toStringArray(Convert::packFile(handle)->verifyEntryChecksums());
}

SOURCEPP_API int vpkpp_has_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasPackFileChecksum();
}

SOURCEPP_API int vpkpp_verify_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->verifyPackFileChecksum();
}

SOURCEPP_API int vpkpp_has_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasPackFileSignature();
}

SOURCEPP_API int vpkpp_verify_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->verifyPackFileSignature();
}

SOURCEPP_API int vpkpp_is_case_sensitive(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->isCaseSensitive();
}

SOURCEPP_API int vpkpp_has_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return Convert::packFile(handle)->hasEntry(path, includeUnbaked);
}

SOURCEPP_API vpkpp_entry_handle_t vpkpp_find_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto entry = Convert::packFile(handle)->findEntry(path, includeUnbaked);
	if (!entry) {
		return nullptr;
	}
	return new Entry(std::move(*entry));
}

SOURCEPP_API sourcepp_buffer_t vpkpp_read_entry(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_BUFFER_INVALID);

	if (auto binary = Convert::packFile(handle)->readEntry(path)) {
		return Convert::toBuffer(*binary);
	}
	return SOURCEPP_BUFFER_INVALID;
}

SOURCEPP_API sourcepp_string_t vpkpp_read_entry_text(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	if (auto text = Convert::packFile(handle)->readEntryText(path)) {
		return Convert::toString(*text);
	}
	return SOURCEPP_STRING_INVALID;
}

SOURCEPP_API int vpkpp_is_read_only(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->isReadOnly();
}

SOURCEPP_API void vpkpp_add_entry_from_file(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(entryPath);
	SOURCEPP_EARLY_RETURN(filepath);

	Convert::packFile(handle)->addEntry(entryPath, filepath, {});
}

SOURCEPP_API void vpkpp_add_entry_from_file_with_options(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath, vpkpp_entry_options_t options) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(entryPath);
	SOURCEPP_EARLY_RETURN(filepath);

	Convert::packFile(handle)->addEntry(entryPath, filepath, Convert::optionsFromC(options));
}

SOURCEPP_API void vpkpp_add_entry_from_mem(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);
	SOURCEPP_EARLY_RETURN(buffer);
	SOURCEPP_EARLY_RETURN(bufferLen);

	Convert::packFile(handle)->addEntry(path, reinterpret_cast<const std::byte*>(buffer), bufferLen, {});
}

SOURCEPP_API void vpkpp_add_entry_from_mem_with_options(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen, vpkpp_entry_options_t options) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);
	SOURCEPP_EARLY_RETURN(buffer);
	SOURCEPP_EARLY_RETURN(bufferLen);

	Convert::packFile(handle)->addEntry(path, reinterpret_cast<const std::byte*>(buffer), bufferLen, Convert::optionsFromC(options));
}

SOURCEPP_API int vpkpp_rename_entry(vpkpp_pack_file_handle_t handle, const char* oldPath, const char* newPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(oldPath, false);
	SOURCEPP_EARLY_RETURN_VAL(newPath, false);

	return Convert::packFile(handle)->renameEntry(oldPath, newPath);
}

SOURCEPP_API int vpkpp_rename_directory(vpkpp_pack_file_handle_t handle, const char* oldDir, const char* newDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(oldDir, false);
	SOURCEPP_EARLY_RETURN_VAL(newDir, false);

	return Convert::packFile(handle)->renameEntry(oldDir, newDir);
}

SOURCEPP_API int vpkpp_remove_entry(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return Convert::packFile(handle)->removeEntry(path);
}

SOURCEPP_API int vpkpp_remove_directory(vpkpp_pack_file_handle_t handle, const char* dirName) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(dirName, false);

	return Convert::packFile(handle)->removeEntry(dirName);
}

SOURCEPP_API int vpkpp_bake(vpkpp_pack_file_handle_t handle, const char* outputDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->bake(outputDir, nullptr);
}

SOURCEPP_API int vpkpp_bake_with_callback(vpkpp_pack_file_handle_t handle, const char* outputDir, EntryCallback callback) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);
	SOURCEPP_EARLY_RETURN_VAL(callback, false);

	return Convert::packFile(handle)->bake(outputDir, [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	});
}

SOURCEPP_API int vpkpp_extract_entry(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filePath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(entryPath, false);
	SOURCEPP_EARLY_RETURN_VAL(filePath, false);

	return Convert::packFile(handle)->extractEntry(entryPath, filePath);
}

SOURCEPP_API int vpkpp_extract_directory(vpkpp_pack_file_handle_t handle, const char* dir, const char* outputDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(dir, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->extractDirectory(dir, outputDir);
}

SOURCEPP_API int vpkpp_extract_all(vpkpp_pack_file_handle_t handle, const char* outputDir, int createUnderPackFileDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->extractAll(outputDir, createUnderPackFileDir);
}

SOURCEPP_API int vpkpp_extract_all_if(vpkpp_pack_file_handle_t handle, const char* outputDir, EntryPredicate predicate, int stripSharedDirs) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);
	SOURCEPP_EARLY_RETURN_VAL(predicate, false);

	return Convert::packFile(handle)->extractAll(outputDir, [predicate](const std::string& path, const Entry& entry) {
		return predicate(path.c_str(), const_cast<Entry*>(&entry));
	}, stripSharedDirs);
}

SOURCEPP_API size_t vpkpp_get_entry_count(vpkpp_pack_file_handle_t handle, int includeUnbaked) {
	return Convert::packFile(handle)->getEntryCount(includeUnbaked);
}

SOURCEPP_API void vpkpp_run_for_all_entries(vpkpp_pack_file_handle_t handle, EntryCallback operation, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(operation);

	return Convert::packFile(handle)->runForAllEntries([operation](const std::string& path, const Entry& entry) {
		return operation(path.c_str(), const_cast<Entry*>(&entry));
	}, includeUnbaked);
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filepath(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getFilepath());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filepath(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getTruncatedFilepath());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filename(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getFilename());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filename(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getTruncatedFilename());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filestem(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getFilestem());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filestem(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::packFile(handle)->getTruncatedFilestem());
}

SOURCEPP_API vpkpp_attribute_e vpkpp_get_supported_entry_attributes(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_ATTRIBUTE_NONE);

	return static_cast<vpkpp_attribute_e>(Convert::packFile(handle)->getSupportedEntryAttributes());
}

SOURCEPP_API sourcepp_string_t vpkpp_to_string(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(std::string{*Convert::packFile(handle)});
}

SOURCEPP_API void vpkpp_close(vpkpp_pack_file_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	std::default_delete<PackFile>()(Convert::packFile(*handle));
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t vpkpp_escape_entry_path_for_write(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	return Convert::toString(PackFile::escapeEntryPathForWrite(path));
}

SOURCEPP_API sourcepp_string_array_t vpkpp_get_supported_file_types() {
	return Convert::toStringArray(PackFile::getSupportedFileTypes());
}
