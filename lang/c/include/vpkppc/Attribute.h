#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VPKPP_ATTRIBUTE_NONE = -1,
	VPKPP_ATTRIBUTE_LENGTH = 0,
	VPKPP_ATTRIBUTE_VPK_PRELOADED_DATA_LENGTH,
	VPKPP_ATTRIBUTE_VPK_ARCHIVE_INDEX,
	VPKPP_ATTRIBUTE_CRC32,
	VPKPP_ATTRIBUTE_PCK_MD5,
	VPKPP_ATTRIBUTE_COUNT,
} vpkpp_attribute_e;

#ifdef __cplusplus
} // extern "C"
#endif
