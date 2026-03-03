#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const uint32_t VPKPP_VPP_SIGNATURE_LIL;
VPKPP_EXTERNVAR const uint32_t VPKPP_VPP_SIGNATURE_BIG;
VPKPP_EXTERNVAR const uint32_t VPKPP_VPP_ALIGNMENT;
VPKPP_EXTERNVAR const char* VPKPP_VPP_EXTENSION;
VPKPP_EXTERNVAR const char* VPKPP_VPP_EXTENSION_PC;
VPKPP_EXTERNVAR const char* VPKPP_VPP_EXTENSION_XBOX2;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpp_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/VPP.h>

#endif
