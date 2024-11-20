#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* gamepp_game_instance_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: gamepp_game_instance_free
SOURCEPP_API gamepp_game_instance_handle_t gamepp_find_game_instance();

// REQUIRES MANUAL FREE: gamepp_game_instance_free
SOURCEPP_API gamepp_game_instance_handle_t gamepp_find_game_instance_with_name(const char* windowNameOverride);

SOURCEPP_API void gamepp_game_instance_free(gamepp_game_instance_handle_t* handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t gamepp_get_window_title(gamepp_game_instance_handle_t handle);

SOURCEPP_API int gamepp_get_window_pos_x(gamepp_game_instance_handle_t handle);

SOURCEPP_API int gamepp_get_window_pos_y(gamepp_game_instance_handle_t handle);

SOURCEPP_API int gamepp_get_window_width(gamepp_game_instance_handle_t handle);

SOURCEPP_API int gamepp_get_window_height(gamepp_game_instance_handle_t handle);

SOURCEPP_API void gamepp_command(gamepp_game_instance_handle_t handle, const char* command);

SOURCEPP_API void gamepp_input_begin(gamepp_game_instance_handle_t handle, const char* input);

SOURCEPP_API void gamepp_input_end(gamepp_game_instance_handle_t handle, const char* input);

SOURCEPP_API void gamepp_input_once(gamepp_game_instance_handle_t handle, const char* input);

SOURCEPP_API void gamepp_input_hold(gamepp_game_instance_handle_t handle, const char* input, double sec);

SOURCEPP_API void gamepp_wait(gamepp_game_instance_handle_t handle, double sec);
