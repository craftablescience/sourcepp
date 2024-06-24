#pragma once

#include "../PackFile.h"

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path);

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open_with_options(const char* path, vpkpp_pack_file_options_t options);
