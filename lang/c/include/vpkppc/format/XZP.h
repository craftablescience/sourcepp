#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_xzp_open(const char* path, vpkpp_entry_callback_t callback);
