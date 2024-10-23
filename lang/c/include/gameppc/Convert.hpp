#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

#include "gamepp.h"

namespace gamepp {

class GameInstance;

} // namespace gamepp

namespace Convert {

gamepp::GameInstance* gameInstance(gamepp_game_instance_handle_t handle);

} // namespace Convert
