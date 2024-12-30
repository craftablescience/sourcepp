#include <vpkppc/format/VPK.h>

#include <vpkpp/format/VPK.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_with_options(const char* path, uint32_t version) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::create(path, version);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_vpk_guid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(VPK::GUID);
}

SOURCEPP_API bool vpkpp_vpk_generate_keypair_files(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return VPK::generateKeyPairFiles(path);
}

SOURCEPP_API bool vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(filename, false);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return false;
	}
	return dynamic_cast<VPK*>(vpk)->sign(filename);
}

SOURCEPP_API bool vpkpp_vpk_sign_from_mem(vpkpp_pack_file_handle_t handle, const unsigned char* privateKeyBuffer, size_t privateKeyLen, const unsigned char* publicKeyBuffer, size_t publicKeyLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(privateKeyBuffer, false);
	SOURCEPP_EARLY_RETURN_VAL(privateKeyLen, false);
	SOURCEPP_EARLY_RETURN_VAL(publicKeyBuffer, false);
	SOURCEPP_EARLY_RETURN_VAL(publicKeyLen, false);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return false;
	}
	return dynamic_cast<VPK*>(vpk)->sign(
		{reinterpret_cast<const std::byte*>(privateKeyBuffer), reinterpret_cast<const std::byte*>(privateKeyBuffer + privateKeyLen)},
		{reinterpret_cast<const std::byte*>(publicKeyBuffer), reinterpret_cast<const std::byte*>(publicKeyBuffer + publicKeyLen)});
}

SOURCEPP_API uint32_t vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return 0;
	}
	return dynamic_cast<VPK*>(vpk)->getVersion();
}

SOURCEPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return;
	}
	dynamic_cast<VPK*>(vpk)->setVersion(version);
}

SOURCEPP_API uint32_t vpkpp_vpk_get_chunk_size(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return 0;
	}
	return dynamic_cast<VPK*>(vpk)->getChunkSize();
}

SOURCEPP_API void vpkpp_vpk_set_chunk_size(vpkpp_pack_file_handle_t handle, uint32_t chunkSize) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* vpk = Convert::packFile(handle);
	if (!vpk->isInstanceOf<VPK>()) {
		return;
	}
	dynamic_cast<VPK*>(vpk)->setChunkSize(chunkSize);
}
