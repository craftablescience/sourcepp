#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint32_t VPKPP_GMA_SIGNATURE;
SOURCEPP_EXTERNVAR const char* VPKPP_GMA_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gma_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/GMA.h>

#endif
