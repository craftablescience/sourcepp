#include <vpkppc/PackFile.h>

#include <cstring>
#include <memory>
#include <utility>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_EXECUTABLE_EXTENSION0 = EXECUTABLE_EXTENSION0.data();
const char* VPKPP_EXECUTABLE_EXTENSION1 = EXECUTABLE_EXTENSION1.data();
const char* VPKPP_EXECUTABLE_EXTENSION2 = EXECUTABLE_EXTENSION2.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open(const char* path, vpkpp_entry_callback_t callback, vpkpp_pack_file_open_property_request_t requestProperty) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PackFile::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr), requestProperty ? [requestProperty](PackFile* packFile_, PackFile::OpenProperty property) {
		return convert::fromBuffer<std::byte>(requestProperty(packFile_, convert::cast(property)));
	} : static_cast<PackFile::OpenPropertyRequest>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API sourcepp_string_array_t vpkpp_get_openable_extensions() {
	return convert::toStringArray(PackFile::getOpenableExtensions());
}

SOURCEPP_API int vpkpp_has_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->hasEntryChecksums();
}

SOURCEPP_API sourcepp_string_array_t vpkpp_verify_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	return convert::toStringArray(convert::handle<PackFile>(handle)->verifyEntryChecksums());
}

SOURCEPP_API int vpkpp_has_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->hasPackFileChecksum();
}

SOURCEPP_API int vpkpp_verify_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->verifyPackFileChecksum();
}

SOURCEPP_API int vpkpp_has_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->hasPackFileSignature();
}

SOURCEPP_API int vpkpp_verify_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->verifyPackFileSignature();
}

SOURCEPP_API int vpkpp_is_case_sensitive(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->isCaseSensitive();
}

SOURCEPP_API int vpkpp_has_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return convert::handle<PackFile>(handle)->hasEntry(path, includeUnbaked);
}

SOURCEPP_API vpkpp_entry_handle_t vpkpp_find_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto entry = convert::handle<PackFile>(handle)->findEntry(path, includeUnbaked);
	if (!entry) {
		return nullptr;
	}
	return new Entry(std::move(*entry));
}

SOURCEPP_API sourcepp_buffer_t vpkpp_read_entry(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_BUFFER_INVALID);

	if (const auto binary = convert::handle<PackFile>(handle)->readEntry(path)) {
		return convert::toBuffer(*binary);
	}
	return SOURCEPP_BUFFER_INVALID;
}

SOURCEPP_API sourcepp_string_t vpkpp_read_entry_text(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	if (const auto text = convert::handle<PackFile>(handle)->readEntryText(path)) {
		return convert::toString(*text);
	}
	return SOURCEPP_STRING_INVALID;
}

SOURCEPP_API int vpkpp_is_read_only(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PackFile>(handle)->isReadOnly();
}

SOURCEPP_API void vpkpp_add_entry_from_file(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(entryPath);
	SOURCEPP_EARLY_RETURN(filepath);

	convert::handle<PackFile>(handle)->addEntry(entryPath, filepath, {});
}

SOURCEPP_API void vpkpp_add_entry_from_file_with_options(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath, vpkpp_entry_options_t options) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(entryPath);
	SOURCEPP_EARLY_RETURN(filepath);

	convert::handle<PackFile>(handle)->addEntry(entryPath, filepath, convert::cast(options));
}

SOURCEPP_API void vpkpp_add_entry_from_mem(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);
	SOURCEPP_EARLY_RETURN(buffer);
	SOURCEPP_EARLY_RETURN(bufferLen);

	convert::handle<PackFile>(handle)->addEntry(path, {reinterpret_cast<const std::byte*>(buffer), bufferLen}, {});
}

SOURCEPP_API void vpkpp_add_entry_from_mem_with_options(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen, vpkpp_entry_options_t options) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);
	SOURCEPP_EARLY_RETURN(buffer);
	SOURCEPP_EARLY_RETURN(bufferLen);

	convert::handle<PackFile>(handle)->addEntry(path, {reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(options));
}

SOURCEPP_API void vpkpp_add_directory(vpkpp_pack_file_handle_t handle, const char* entryBaseDir, const char* dir, vpkpp_entry_creation_t creation) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(dir);

	convert::handle<PackFile>(handle)->addDirectory(entryBaseDir ? entryBaseDir : "", dir, creation ? [creation](const std::string& path) {
		return convert::cast(creation(path.c_str()));
	} : static_cast<PackFile::EntryCreation>(nullptr));
}

SOURCEPP_API void vpkpp_add_directory_with_options(vpkpp_pack_file_handle_t handle, const char* entryBaseDir, const char* dir, vpkpp_entry_options_t options) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(dir);

	convert::handle<PackFile>(handle)->addDirectory(entryBaseDir ? entryBaseDir : "", dir, convert::cast(options));
}

SOURCEPP_API int vpkpp_rename_entry(vpkpp_pack_file_handle_t handle, const char* oldPath, const char* newPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(oldPath, false);
	SOURCEPP_EARLY_RETURN_VAL(newPath, false);

	return convert::handle<PackFile>(handle)->renameEntry(oldPath, newPath);
}

SOURCEPP_API int vpkpp_rename_directory(vpkpp_pack_file_handle_t handle, const char* oldDir, const char* newDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(oldDir, false);
	SOURCEPP_EARLY_RETURN_VAL(newDir, false);

	return convert::handle<PackFile>(handle)->renameEntry(oldDir, newDir);
}

SOURCEPP_API int vpkpp_remove_entry(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return convert::handle<PackFile>(handle)->removeEntry(path);
}

SOURCEPP_API int vpkpp_remove_directory(vpkpp_pack_file_handle_t handle, const char* dirName) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(dirName, false);

	return convert::handle<PackFile>(handle)->removeEntry(dirName);
}

SOURCEPP_API int vpkpp_bake(vpkpp_pack_file_handle_t handle, const char* outputDir, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return convert::handle<PackFile>(handle)->bake(outputDir, {}, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
}

SOURCEPP_API int vpkpp_bake_with_options(vpkpp_pack_file_handle_t handle, const char* outputDir, vpkpp_bake_options_t options, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return convert::handle<PackFile>(handle)->bake(outputDir, convert::cast(options), callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
}

SOURCEPP_API int vpkpp_extract_entry(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filePath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(entryPath, false);
	SOURCEPP_EARLY_RETURN_VAL(filePath, false);

	return convert::handle<PackFile>(handle)->extractEntry(entryPath, filePath);
}

SOURCEPP_API int vpkpp_extract_directory(vpkpp_pack_file_handle_t handle, const char* dir, const char* outputDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(dir, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return convert::handle<PackFile>(handle)->extractDirectory(dir, outputDir);
}

SOURCEPP_API int vpkpp_extract_all(vpkpp_pack_file_handle_t handle, const char* outputDir, int createUnderPackFileDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return convert::handle<PackFile>(handle)->extractAll(outputDir, createUnderPackFileDir);
}

SOURCEPP_API int vpkpp_extract_all_if(vpkpp_pack_file_handle_t handle, const char* outputDir, vpkpp_entry_predicate_t predicate, int stripSharedDirs) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);
	SOURCEPP_EARLY_RETURN_VAL(predicate, false);

	return convert::handle<PackFile>(handle)->extractAll(outputDir, [predicate](const std::string& path, const Entry& entry) {
		return predicate(path.c_str(), const_cast<Entry*>(&entry));
	}, stripSharedDirs);
}

SOURCEPP_API size_t vpkpp_get_entry_count(vpkpp_pack_file_handle_t handle, int includeUnbaked) {
	return convert::handle<PackFile>(handle)->getEntryCount(includeUnbaked);
}

SOURCEPP_API void vpkpp_run_for_all_entries(vpkpp_pack_file_handle_t handle, vpkpp_entry_callback_t operation, int includeUnbaked) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(operation);

	return convert::handle<PackFile>(handle)->runForAllEntries([operation](const std::string& path, const Entry& entry) {
		return operation(path.c_str(), const_cast<Entry*>(&entry));
	}, includeUnbaked);
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filepath(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getFilepath());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filepath(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getTruncatedFilepath());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filename(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getFilename());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filename(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getTruncatedFilename());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_filestem(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getFilestem());
}

SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filestem(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<PackFile>(handle)->getTruncatedFilestem());
}

SOURCEPP_API vpkpp_attribute_e vpkpp_get_supported_entry_attributes(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_ATTRIBUTE_NONE);

	return convert::cast(convert::handle<PackFile>(handle)->getSupportedEntryAttributes());
}

SOURCEPP_API sourcepp_string_t vpkpp_to_string(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(std::string{*convert::handle<PackFile>(handle)});
}

SOURCEPP_API void vpkpp_close(vpkpp_pack_file_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	std::default_delete<PackFile>()(convert::handle<PackFile>(*handle));
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t vpkpp_escape_entry_path_for_write(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	return convert::toString(PackFile::escapeEntryPathForWrite(path));
}
