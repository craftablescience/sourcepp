#pragma once

SOURCEPP_EXTERN typedef enum {
	VPKPP_ATTRIBUTE_NONE               = 0,
	VPKPP_ATTRIBUTE_ARCHIVE_INDEX      = 1 << 0,
	VPKPP_ATTRIBUTE_LENGTH             = 1 << 1,
	VPKPP_ATTRIBUTE_CRC32              = 1 << 2,
	VPKPP_ATTRIBUTE_PCK_MD5            = 1 << 3,
	VPKPP_ATTRIBUTE_VPK_PRELOADED_DATA = 1 << 4,
} vpkpp_attribute_e;

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/Attribute.h>

namespace sourceppc::convert {

inline vpkpp::Attribute cast(vpkpp_attribute_e value) {
	return static_cast<vpkpp::Attribute>(value);
}

inline vpkpp_attribute_e cast(vpkpp::Attribute value) {
	return static_cast<vpkpp_attribute_e>(value);
}

} // namespace sourceppc::convert

#endif
