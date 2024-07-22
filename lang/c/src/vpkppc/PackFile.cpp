#include <vpkppc/PackFile.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

import vpkpp;

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PackFile::open(path);
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

SOURCEPP_API vpkpp_pack_file_type_e vpkpp_get_type(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_PACK_FILE_TYPE_UNKNOWN);

	return static_cast<vpkpp_pack_file_type_e>(Convert::packFile(handle)->getType());
}

SOURCEPP_API vpkpp_pack_file_options_t vpkpp_get_options(vpkpp_pack_file_handle_t handle) {
	return Convert::optionsToC(Convert::packFile(handle)->getOptions());
}

SOURCEPP_API bool vpkpp_has_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasEntryChecksums();
}

SOURCEPP_API sourcepp_string_array_t vpkpp_verify_entry_checksums(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_ARRAY_INVALID);

	return Convert::toStringArray(Convert::packFile(handle)->verifyEntryChecksums());
}

SOURCEPP_API bool vpkpp_has_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasPackFileChecksum();
}

SOURCEPP_API bool vpkpp_verify_pack_file_checksum(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->verifyPackFileChecksum();
}

SOURCEPP_API bool vpkpp_has_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->hasPackFileSignature();
}

SOURCEPP_API bool vpkpp_verify_pack_file_signature(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->verifyPackFileSignature();
}

SOURCEPP_API bool vpkpp_is_case_sensitive(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->isCaseSensitive();
}

SOURCEPP_API bool vpkpp_has_entry(vpkpp_pack_file_handle_t handle, const char* filename, bool includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(filename, false);

	return Convert::packFile(handle)->hasEntry(filename, includeUnbaked);
}

SOURCEPP_API vpkpp_entry_handle_t vpkpp_find_entry(vpkpp_pack_file_handle_t handle, const char* filename, bool includeUnbaked) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(filename, nullptr);

	auto entry = Convert::packFile(handle)->findEntry(filename, includeUnbaked);
	if (!entry) {
		return nullptr;
	}
	return new Entry(std::move(*entry));
}

SOURCEPP_API sourcepp_buffer_t vpkpp_read_entry(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(entry, SOURCEPP_BUFFER_INVALID);

	if (auto binary = Convert::packFile(handle)->readEntry(*Convert::entry(entry))) {
		return Convert::toBuffer(*binary);
	}
	return SOURCEPP_BUFFER_INVALID;
}

SOURCEPP_API sourcepp_string_t vpkpp_read_entry_text(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(entry, SOURCEPP_STRING_INVALID);

	if (auto text = Convert::packFile(handle)->readEntryText(*Convert::entry(entry))) {
		return Convert::toString(*text);
	}
	return SOURCEPP_STRING_INVALID;
}

SOURCEPP_API bool vpkpp_is_read_only(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::packFile(handle)->isReadOnly();
}

SOURCEPP_API void vpkpp_add_entry_from_file(vpkpp_pack_file_handle_t handle, const char* filename, const char* pathToFile) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(filename);
	SOURCEPP_EARLY_RETURN(pathToFile);

	Convert::packFile(handle)->addEntry(filename, pathToFile, {});
}

SOURCEPP_API void vpkpp_add_entry_from_mem(vpkpp_pack_file_handle_t handle, const char* filename, const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(filename);

	Convert::packFile(handle)->addEntry(filename, reinterpret_cast<const std::byte*>(buffer), bufferLen, {});
}

SOURCEPP_API bool vpkpp_remove_entry(vpkpp_pack_file_handle_t handle, const char* filename) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(filename, false);

	return Convert::packFile(handle)->removeEntry(filename);
}

SOURCEPP_API bool vpkpp_bake(vpkpp_pack_file_handle_t handle, const char* outputDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->bake(outputDir, nullptr);
}

SOURCEPP_API bool vpkpp_extract_entry(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry, const char* filePath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(entry, false);
	SOURCEPP_EARLY_RETURN_VAL(filePath, false);

	return Convert::packFile(handle)->extractEntry(*Convert::entry(entry), filePath);
}

SOURCEPP_API bool vpkpp_extract_directory(vpkpp_pack_file_handle_t handle, const char* dir, const char* outputDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(dir, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->extractDirectory(dir, outputDir);
}

SOURCEPP_API bool vpkpp_extract_all(vpkpp_pack_file_handle_t handle, const char* outputDir, bool createUnderPackFileDir) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);

	return Convert::packFile(handle)->extractAll(outputDir, createUnderPackFileDir);
}

SOURCEPP_API bool vpkpp_extract_all_if(vpkpp_pack_file_handle_t handle, const char* outputDir, bool(*predicate)(vpkpp_entry_handle_t)) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(outputDir, false);
	SOURCEPP_EARLY_RETURN_VAL(predicate, false);

	return Convert::packFile(handle)->extractAll(outputDir, [predicate](const Entry& entry) {
		return predicate(reinterpret_cast<vpkpp_entry_handle_t>(const_cast<Entry*>(&entry)));
	});
}

SOURCEPP_API vpkpp_entry_handle_array_t vpkpp_get_baked_entries(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_ENTRY_HANDLE_ARRAY_INVALID);

	std::vector<Entry*> heapEntries;
	for (const auto& [dir, entries] : Convert::packFile(handle)->getBakedEntries()) {
		for (const auto& entry : entries) {
			heapEntries.push_back(new Entry{entry});
		}
	}

	vpkpp_entry_handle_array_t array;
	array.size = static_cast<int64_t>(heapEntries.size());
	array.data = static_cast<vpkpp_entry_handle_t*>(std::malloc(sizeof(vpkpp_entry_handle_t) * array.size));

	for (size_t i = 0; i < array.size; i++) {
		array.data[i] = heapEntries[i];
	}
	return array;
}

SOURCEPP_API vpkpp_entry_handle_array_t vpkpp_get_unbaked_entries(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_ENTRY_HANDLE_ARRAY_INVALID);

	std::vector<Entry*> heapEntries;
	for (const auto& [dir, entries] : Convert::packFile(handle)->getUnbakedEntries()) {
		for (const auto& entry : entries) {
			heapEntries.push_back(new Entry{entry});
		}
	}

	vpkpp_entry_handle_array_t array;
	array.size = static_cast<int64_t>(heapEntries.size());
	array.data = static_cast<vpkpp_entry_handle_t*>(std::malloc(sizeof(vpkpp_entry_handle_t) * array.size));

	for (size_t i = 0; i < array.size; i++) {
		array.data[i] = heapEntries[i];
	}
	return array;
}

SOURCEPP_API size_t vpkpp_get_entry_count(vpkpp_pack_file_handle_t handle, bool includeUnbaked) {
	return Convert::packFile(handle)->getEntryCount(includeUnbaked);
}

SOURCEPP_API size_t vpkpp_get_filepath(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getFilepath(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_truncated_filepath(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getTruncatedFilepath(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_filename(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getFilename(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_truncated_filename(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getTruncatedFilename(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_filestem(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getFilestem(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_truncated_filestem(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::packFile(handle)->getTruncatedFilestem(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_get_supported_entry_attributes(vpkpp_pack_file_handle_t handle, vpkpp_attribute_e* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	auto attrs = Convert::packFile(handle)->getSupportedEntryAttributes();
	for (size_t i = 0; i < bufferLen; i++) {
		if (i < attrs.size()) {
			buffer[i] = static_cast<vpkpp_attribute_e>(attrs[i]);
		} else {
			buffer[i] = VPKPP_ATTRIBUTE_NONE;
		}
	}
	return std::min(attrs.size(), bufferLen);
}

SOURCEPP_API size_t vpkpp_to_string(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(std::string{*Convert::packFile(handle)}, buffer, bufferLen);
}

SOURCEPP_API void vpkpp_close(vpkpp_pack_file_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	std::default_delete<PackFile>()(Convert::packFile(*handle));
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t vpkpp_escape_entry_path(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	return Convert::toString(PackFile::escapeEntryPath(path));
}

SOURCEPP_API sourcepp_string_array_t vpkpp_get_supported_file_types() {
	return Convert::toStringArray(PackFile::getSupportedFileTypes());
}
