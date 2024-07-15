#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

#include <vpkpp/Options.h>

#include "Entry.h"
#include "PackFile.h"

namespace vpkpp {

class Entry;
class PackFile;

} // namespace vpkpp

namespace Convert {

vpkpp::PackFile* packFile(vpkpp_pack_file_handle_t handle);

vpkpp::Entry* entry(vpkpp_entry_handle_t handle);

vpkpp::PackFileOptions optionsFromC(vpkpp_pack_file_options_t options);

vpkpp_pack_file_options_t optionsToC(vpkpp::PackFileOptions options);

} // namespace Convert
