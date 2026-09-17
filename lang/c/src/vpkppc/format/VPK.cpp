#include <vpkppc/format/VPK.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_FPX_SIGNATURE = FPX_SIGNATURE;
const char* VPKPP_FPX_DIR_SUFFIX   = FPX_DIR_SUFFIX.data();
const char* VPKPP_FPX_EXTENSION    = FPX_EXTENSION.data();

const uint32_t VPKPP_VPK_SIGNATURE = VPK_SIGNATURE;
const char* VPKPP_VPK_DIR_SUFFIX   = VPK_DIR_SUFFIX.data();
const char* VPKPP_VPK_EXTENSION    = VPK_EXTENSION.data();

const uint16_t VPKPP_VPK_DIR_ARCHIVE_INDEX = VPK_DIR_ARCHIVE_INDEX;

const uint16_t VPKPP_VPK_MAX_PRELOAD_BYTES = VPK_MAX_PRELOAD_BYTES;

const uint32_t VPKPP_VPK_DEFAULT_CHUNK_SIZE = VPK_DEFAULT_CHUNK_SIZE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_with_options(const char* path, vpkpp_vpk_version_e version) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::create(path, convert::cast(version));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API void vpkpp_vpk_generate_keypair_files(const char* base, vpkpp_vpk_signature_type_e signatureType) {
	SOURCEPP_EARLY_RETURN(base);

	return VPK::generateKeyPairFiles(base, convert::cast(signatureType));
}

VPKPP_API int vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename, vpkpp_vpk_signature_type_e signatureType) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(filename, false);

	auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(vpk, false);

	return vpk->sign(filename, convert::cast(signatureType));
}

VPKPP_API int vpkpp_vpk_sign_from_mem(vpkpp_pack_file_handle_t handle, const unsigned char* privateKeyBuffer, size_t privateKeyLen, const unsigned char* publicKeyBuffer, size_t publicKeyLen, vpkpp_vpk_signature_type_e signatureType) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(privateKeyBuffer, false);
	SOURCEPP_EARLY_RETURN_VAL(privateKeyLen, false);
	SOURCEPP_EARLY_RETURN_VAL(publicKeyBuffer, false);
	SOURCEPP_EARLY_RETURN_VAL(publicKeyLen, false);

	auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(vpk, false);

	return vpk->sign(
		{reinterpret_cast<const std::byte*>(privateKeyBuffer), reinterpret_cast<const std::byte*>(privateKeyBuffer + privateKeyLen)},
		{reinterpret_cast<const std::byte*>(publicKeyBuffer), reinterpret_cast<const std::byte*>(publicKeyBuffer + publicKeyLen)},
		convert::cast(signatureType));
}

VPKPP_API vpkpp_vpk_version_e vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_VPK_VERSION_VALVE_V0);

	const auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(vpk, VPKPP_VPK_VERSION_VALVE_V0);

	return convert::cast(vpk->getVersion());
}

VPKPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, vpkpp_vpk_version_e version) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN(vpk);

	vpk->setVersion(convert::cast(version));
}

VPKPP_API uint32_t vpkpp_vpk_get_chunk_size(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	const auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(vpk, 0);

	return vpk->getChunkSize();
}

VPKPP_API void vpkpp_vpk_set_chunk_size(vpkpp_pack_file_handle_t handle, uint32_t chunkSize) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* vpk = dynamic_cast<VPK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN(vpk);

	vpk->setChunkSize(chunkSize);
}
