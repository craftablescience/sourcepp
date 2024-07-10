#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

#include "steampp.h"

namespace steampp {

class Steam;

} // namespace steampp

namespace Convert {

steampp::Steam* steam(steampp_steam_handle_t handle);

} // namespace Convert
