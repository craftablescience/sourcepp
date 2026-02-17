#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint32_t VPKPP_XZP_HEADER_SIGNATURE;
SOURCEPP_EXTERNVAR const uint32_t VPKPP_XZP_FOOTER_SIGNATURE;
SOURCEPP_EXTERNVAR const char* VPKPP_XZP_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_xzp_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/XZP.h>

#endif
