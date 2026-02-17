#pragma once

#include <sourceppc/API.h>

SOURCEPP_EXTERN typedef enum {
	VPKPP_ENTRY_COMPRESSION_TYPE_NO_OVERRIDE = -1,
	VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS =  0,
	VPKPP_ENTRY_COMPRESSION_TYPE_DEFLATE     =  8,
	VPKPP_ENTRY_COMPRESSION_TYPE_BZIP2       = 12,
	VPKPP_ENTRY_COMPRESSION_TYPE_LZMA        = 14,
	VPKPP_ENTRY_COMPRESSION_TYPE_ZSTD        = 93,
	VPKPP_ENTRY_COMPRESSION_TYPE_XZ          = 95,
} vpkpp_entry_compression_type_e;

SOURCEPP_EXTERN typedef struct {
	int16_t zip_compressionTypeOverride;
	int16_t zip_compressionStrength;
	uint8_t gma_writeCRCs;
	uint8_t vpk_generateMD5Entries;
}  vpkpp_bake_options_t;

#define VPKPP_BAKE_OPTIONS_DEFAULT (SOURCEPP_CAST_CTOR(vpkpp_bake_options_t) { \
	.zip_compressionTypeOverride = VPKPP_ENTRY_COMPRESSION_TYPE_NO_OVERRIDE, \
	.zip_compressionStrength = 5, \
	.gma_writeCRCs = 1, \
	.vpk_generateMD5Entries = 0, \
})

SOURCEPP_EXTERN typedef struct {
	int16_t zip_compressionType;
	int16_t zip_compressionStrength;
	uint16_t vpk_preloadBytes;
	uint8_t vpk_saveToDirectory;
} vpkpp_entry_options_t;

#define VPKPP_ENTRY_OPTIONS_DEFAULT (SOURCEPP_CAST_CTOR(vpkpp_entry_options_t) { \
	.zip_compressionType = VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS, \
	.zip_compressionStrength = 5, \
	.vpk_preloadBytes = 0, \
	.vpk_saveToDirectory = 0, \
})

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/Options.h>

namespace sourceppc::convert {

inline vpkpp::EntryCompressionType cast(vpkpp_entry_compression_type_e value) {
	switch (value) {
		case VPKPP_ENTRY_COMPRESSION_TYPE_NO_OVERRIDE: return vpkpp::EntryCompressionType::NO_OVERRIDE;
		case VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS: return vpkpp::EntryCompressionType::NO_COMPRESS;
		case VPKPP_ENTRY_COMPRESSION_TYPE_DEFLATE:     return vpkpp::EntryCompressionType::DEFLATE;
		case VPKPP_ENTRY_COMPRESSION_TYPE_BZIP2:       return vpkpp::EntryCompressionType::BZIP2;
		case VPKPP_ENTRY_COMPRESSION_TYPE_LZMA:        return vpkpp::EntryCompressionType::LZMA;
		case VPKPP_ENTRY_COMPRESSION_TYPE_ZSTD:        return vpkpp::EntryCompressionType::ZSTD;
		case VPKPP_ENTRY_COMPRESSION_TYPE_XZ:          return vpkpp::EntryCompressionType::XZ;
	}
	return vpkpp::EntryCompressionType::NO_OVERRIDE;
}

inline vpkpp_entry_compression_type_e cast(vpkpp::EntryCompressionType value) {
	switch (value) {
		case vpkpp::EntryCompressionType::NO_OVERRIDE: return VPKPP_ENTRY_COMPRESSION_TYPE_NO_OVERRIDE;
		case vpkpp::EntryCompressionType::NO_COMPRESS: return VPKPP_ENTRY_COMPRESSION_TYPE_NO_COMPRESS;
		case vpkpp::EntryCompressionType::DEFLATE:     return VPKPP_ENTRY_COMPRESSION_TYPE_DEFLATE;
		case vpkpp::EntryCompressionType::BZIP2:       return VPKPP_ENTRY_COMPRESSION_TYPE_BZIP2;
		case vpkpp::EntryCompressionType::LZMA:        return VPKPP_ENTRY_COMPRESSION_TYPE_LZMA;
		case vpkpp::EntryCompressionType::ZSTD:        return VPKPP_ENTRY_COMPRESSION_TYPE_ZSTD;
		case vpkpp::EntryCompressionType::XZ:          return VPKPP_ENTRY_COMPRESSION_TYPE_XZ;
	}
	return VPKPP_ENTRY_COMPRESSION_TYPE_NO_OVERRIDE;
}

inline vpkpp::BakeOptions cast(const vpkpp_bake_options_t& value) {
	return {
		.zip_compressionTypeOverride = cast(static_cast<vpkpp_entry_compression_type_e>(value.zip_compressionTypeOverride)),
		.zip_compressionStrength = value.zip_compressionStrength,
		.gma_writeCRCs = static_cast<bool>(value.gma_writeCRCs),
		.vpk_generateMD5Entries = static_cast<bool>(value.vpk_generateMD5Entries),
	};
}

inline vpkpp_bake_options_t cast(const vpkpp::BakeOptions& value) {
	return {
		.zip_compressionTypeOverride = static_cast<int16_t>(cast(value.zip_compressionTypeOverride)),
		.zip_compressionStrength = value.zip_compressionStrength,
		.gma_writeCRCs = value.gma_writeCRCs,
		.vpk_generateMD5Entries = value.vpk_generateMD5Entries,
	};
}

inline vpkpp::EntryOptions cast(const vpkpp_entry_options_t& value) {
	return {
		.zip_compressionType = cast(static_cast<vpkpp_entry_compression_type_e>(value.zip_compressionType)),
		.zip_compressionStrength = value.zip_compressionStrength,
		.vpk_preloadBytes = value.vpk_preloadBytes,
		.vpk_saveToDirectory = static_cast<bool>(value.vpk_saveToDirectory),
	};
}

inline vpkpp_entry_options_t cast(const vpkpp::EntryOptions& value) {
	return {
		.zip_compressionType = static_cast<int16_t>(cast(value.zip_compressionType)),
		.zip_compressionStrength = value.zip_compressionStrength,
		.vpk_preloadBytes = value.vpk_preloadBytes,
		.vpk_saveToDirectory = value.vpk_saveToDirectory,
	};
}

} // namespace sourceppc::convert

#endif
