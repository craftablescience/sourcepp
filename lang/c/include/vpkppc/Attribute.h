#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VPKPP_ATTRIBUTE_NONE               = 0,
	VPKPP_ATTRIBUTE_ARCHIVE_INDEX      = 1 << 0,
	VPKPP_ATTRIBUTE_LENGTH             = 1 << 1,
	VPKPP_ATTRIBUTE_CRC32              = 1 << 2,
	VPKPP_ATTRIBUTE_PCK_MD5            = 1 << 3,
	VPKPP_ATTRIBUTE_VPK_PRELOADED_DATA = 1 << 4,
} vpkpp_attribute_e;

#ifdef __cplusplus
} // extern "C"
#endif
