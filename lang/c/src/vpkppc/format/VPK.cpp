#include <vpkppc/format/VPK.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

import vpkpp;

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_empty(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::createEmpty(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_empty_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PackFile::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_from_directory(const char* vpkPath, const char* contentPath, bool saveToDir) {
	SOURCEPP_EARLY_RETURN_VAL(vpkPath, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(contentPath, nullptr);

	auto packFile = VPK::createFromDirectory(vpkPath, contentPath, saveToDir);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_from_directory_with_options(const char* vpkPath, const char* contentPath, bool saveToDir, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(vpkPath, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(contentPath, nullptr);

	auto packFile = VPK::createFromDirectory(vpkPath, contentPath, saveToDir, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPK::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API bool vpkpp_vpk_generate_keypair_files(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, false);

	return VPK::generateKeyPairFiles(path);
}

SOURCEPP_API bool vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(filename, false);

	auto* vpk = Convert::packFile(handle);
	if (vpk->getType() != PackFileType::VPK) {
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
	if (vpk->getType() != PackFileType::VPK) {
		return false;
	}
	return dynamic_cast<VPK*>(vpk)->sign(
		{reinterpret_cast<const std::byte*>(privateKeyBuffer), reinterpret_cast<const std::byte*>(privateKeyBuffer + privateKeyLen)},
		{reinterpret_cast<const std::byte*>(publicKeyBuffer), reinterpret_cast<const std::byte*>(publicKeyBuffer + publicKeyLen)});
}

SOURCEPP_API uint32_t vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	auto* vpk = Convert::packFile(handle);
	if (vpk->getType() != PackFileType::VPK) {
		return 0;
	}
	return dynamic_cast<VPK*>(vpk)->getVersion();
}

SOURCEPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* vpk = Convert::packFile(handle);
	if (vpk->getType() != PackFileType::VPK) {
		return;
	}
	dynamic_cast<VPK*>(vpk)->setVersion(version);
}
