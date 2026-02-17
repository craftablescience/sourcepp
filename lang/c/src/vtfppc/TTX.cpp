#include <vtfppc/TTX.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

const uint32_t VTFPP_TTH_SIGNATURE = TTH_SIGNATURE;

SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_create(vtfpp_vtf_handle_t vtf) {
	SOURCEPP_EARLY_RETURN_VAL(vtf, nullptr);

	auto vtfCopy = *convert::handle<VTF>(vtf);
	return new TTX{std::move(vtfCopy)};
}

SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_open_from_mem(const unsigned char* tthData, size_t tthLen, const unsigned char* ttzData, size_t ttzLen) {
	SOURCEPP_EARLY_RETURN_VAL(tthData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(tthLen, nullptr);

	if (!ttzData || !ttzLen) {
		return new TTX{{reinterpret_cast<const std::byte*>(tthData), tthLen}};
	}
	return new TTX{{reinterpret_cast<const std::byte*>(tthData), tthLen}, {reinterpret_cast<const std::byte*>(ttzData), ttzLen}};
}

SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_open_from_file(const char* tthPath, const char* ttzPath) {
	SOURCEPP_EARLY_RETURN_VAL(tthPath, nullptr);

	if (!ttzPath) {
		return new TTX{tthPath};
	}
	return new TTX{tthPath, ttzPath};
}

SOURCEPP_API void vtfpp_ttx_free(vtfpp_ttx_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<TTX>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_ttx_is_valid(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<TTX>(handle)->operator bool();
}

SOURCEPP_API uint8_t vtfpp_ttx_get_major_version(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<TTX>(handle)->getMajorVersion();
}

SOURCEPP_API uint8_t vtfpp_ttx_get_minor_version(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<TTX>(handle)->getMinorVersion();
}

SOURCEPP_API void vtfpp_ttx_set_version(vtfpp_ttx_handle_t handle, uint8_t majorVersion, uint8_t minorVersion) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TTX>(handle)->setVersion(majorVersion, minorVersion);
}

SOURCEPP_API void vtfpp_ttx_set_major_version(vtfpp_ttx_handle_t handle, uint8_t majorVersion) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TTX>(handle)->setMajorVersion(majorVersion);
}

SOURCEPP_API void vtfpp_ttx_set_minor_version(vtfpp_ttx_handle_t handle, uint8_t minorVersion) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TTX>(handle)->setMinorVersion(minorVersion);
}

SOURCEPP_API uint8_t vtfpp_ttx_get_aspect_ratio_type(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<TTX>(handle)->getAspectRatioType();
}

SOURCEPP_API void vtfpp_ttx_set_aspect_ratio_type(vtfpp_ttx_handle_t handle, uint8_t aspectRatioType) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TTX>(handle)->setAspectRatioType(aspectRatioType);
}

SOURCEPP_API sourcepp_buffer_uint64_t vtfpp_ttx_get_mip_flags(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<TTX>(handle)->getMipFlags());
}

SOURCEPP_API void vtfpp_ttx_set_mip_flags(vtfpp_ttx_handle_t handle, const uint64_t* mipFlags, size_t mipFlagsLen) {
	SOURCEPP_EARLY_RETURN(handle);

	if (!mipFlags || !mipFlagsLen) {
		convert::handle<TTX>(handle)->getMipFlags().clear();
	} else {
		std::span mipFlagsSpan{mipFlags, mipFlagsLen};
		convert::handle<TTX>(handle)->getMipFlags().assign(mipFlagsSpan.begin(), mipFlagsSpan.end());
	}
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_ttx_get_vtf(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return &convert::handle<TTX>(handle)->getVTF();
}

SOURCEPP_API void vtfpp_ttx_set_vtf(vtfpp_ttx_handle_t handle, vtfpp_vtf_handle_t vtf) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(vtf);

	convert::handle<TTX>(handle)->getVTF() = *convert::handle<VTF>(vtf);
}

SOURCEPP_API int16_t vtfpp_ttx_get_compression_level(vtfpp_ttx_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<TTX>(handle)->getCompressionLevel();
}

SOURCEPP_API void vtfpp_ttx_set_compression_level(vtfpp_ttx_handle_t handle, int16_t compressionLevel) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TTX>(handle)->setCompressionLevel(compressionLevel);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ttx_bake(vtfpp_ttx_handle_t handle, size_t* tthLen, size_t* ttzLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(tthLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(ttzLen, SOURCEPP_BUFFER_INVALID);

	auto [tthData, ttzData] = convert::handle<TTX>(handle)->bake();
	*tthLen = tthData.size();
	*ttzLen = ttzData.size();

	std::vector<std::byte> out;
	out.insert(out.end(), tthData.begin(), tthData.end());
	out.insert(out.end(), ttzData.begin(), ttzData.end());
	return convert::toBuffer(out);
}

SOURCEPP_API int vtfpp_ttx_bake_to_file(vtfpp_ttx_handle_t handle, const char* tthPath, const char* ttzPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(tthPath, false);
	SOURCEPP_EARLY_RETURN_VAL(ttzPath, false);

	return convert::handle<TTX>(handle)->bake(tthPath, ttzPath);
}
