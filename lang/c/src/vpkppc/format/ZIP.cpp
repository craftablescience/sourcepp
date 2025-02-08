#include <vpkppc/format/ZIP.h>

#include <vpkpp/format/ZIP.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = ZIP::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = ZIP::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_zip_guid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(ZIP::GUID);
}

SOURCEPP_API vpkpp_entry_compression_type_e vpkpp_zip_get_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS);
	SOURCEPP_EARLY_RETURN_VAL(path, VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS);

	auto* zip = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(zip->isInstanceOf<ZIP>(), VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS);

	return static_cast<vpkpp_entry_compression_type_e>(dynamic_cast<ZIP*>(zip)->getEntryCompressionType(path));
}

SOURCEPP_API void vpkpp_zip_set_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path, vpkpp_entry_compression_type_e type) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);

	auto* zip = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN(zip->isInstanceOf<ZIP>());

	dynamic_cast<ZIP*>(zip)->setEntryCompressionType(path, static_cast<EntryCompressionType>(type));
}

SOURCEPP_API int16_t vpkpp_zip_get_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(path, 0);

	auto* zip = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(zip->isInstanceOf<ZIP>(), 0);

	return dynamic_cast<ZIP*>(zip)->getEntryCompressionStrength(path);
}

SOURCEPP_API void vpkpp_zip_set_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path, int16_t strength) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);

	auto* zip = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN(zip->isInstanceOf<ZIP>());

	dynamic_cast<ZIP*>(zip)->setEntryCompressionStrength(path, strength);
}
