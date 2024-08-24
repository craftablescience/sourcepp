#pragma once

#include <sourcepp/Macros.h>

namespace vpkpp {

enum class Attribute {
	NONE               = 0,
	ARCHIVE_INDEX      = 1 << 0,
	LENGTH             = 1 << 1,
	CRC32              = 1 << 2,
	PCK_MD5            = 1 << 3,
	VPK_PRELOADED_DATA = 1 << 4,
};
SOURCEPP_BITFLAGS_ENUM(Attribute)

} // namespace vpkpp
