#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const char* VPKPP_007_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_007_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/OO7.h>

#endif
