#pragma once

#include <sourceppc/String.h>

SOURCEPP_HANDLE(gamepp, game_instance);
SOURCEPP_STATIC(gamepp, game_instance, gamepp_game_instance_handle_t, find); // REQUIRES MANUAL FREE: gamepp_game_instance_free
SOURCEPP_STATIC(gamepp, game_instance, gamepp_game_instance_handle_t, find_with_name, const char* windowNameOverride); // REQUIRES MANUAL FREE: gamepp_game_instance_free
SOURCEPP_STATIC(gamepp, game_instance,                          void, free, gamepp_game_instance_handle_t* handle);
SOURCEPP_METHOD(gamepp, game_instance,             sourcepp_string_t, get_window_title); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_METHOD(gamepp, game_instance,                           int, get_window_pos_x);
SOURCEPP_METHOD(gamepp, game_instance,                           int, get_window_pos_y);
SOURCEPP_METHOD(gamepp, game_instance,                           int, get_window_width);
SOURCEPP_METHOD(gamepp, game_instance,                           int, get_window_height);
SOURCEPP_METHOE(gamepp, game_instance,                          void, command,     const char* command);
SOURCEPP_METHOE(gamepp, game_instance,                          void, input_begin, const char* input);
SOURCEPP_METHOE(gamepp, game_instance,                          void, input_end,   const char* input);
SOURCEPP_METHOE(gamepp, game_instance,                          void, input_once,  const char* input);
SOURCEPP_METHOE(gamepp, game_instance,                          void, input_hold,  const char* input, double sec);
SOURCEPP_METHOE(gamepp, game_instance,                          void, wait,        double sec);
