#include <gameppc/gamepp.h>

#include <gamepp/gamepp.h>

#include <gameppc/Convert.hpp>
#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>

using namespace gamepp;

SOURCEPP_API gamepp_game_instance_handle_t gamepp_find_game_instance() {
	auto instance = GameInstance::find();
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_API gamepp_game_instance_handle_t gamepp_find_game_instance_with_name(const char* windowNameOverride) {
	auto instance = GameInstance::find(windowNameOverride);
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_API void gamepp_game_instance_free(gamepp_game_instance_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::gameInstance(*handle);
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t gamepp_get_window_title(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::gameInstance(handle)->getWindowTitle());
}

SOURCEPP_API int gamepp_get_window_pos_x(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowPos()[0];
}

SOURCEPP_API int gamepp_get_window_pos_y(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowPos()[1];
}

SOURCEPP_API int gamepp_get_window_width(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowSize()[0];
}

SOURCEPP_API int gamepp_get_window_height(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowSize()[1];
}

SOURCEPP_API void gamepp_command(gamepp_game_instance_handle_t handle, const char* command) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->command(command);
}

SOURCEPP_API void gamepp_input_begin(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->inputBegin(input);
}

SOURCEPP_API void gamepp_input_end(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->inputEnd(input);
}

SOURCEPP_API void gamepp_input_once(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->inputOnce(input);
}

SOURCEPP_API void gamepp_input_hold(gamepp_game_instance_handle_t handle, const char* input, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->inputHold(input, sec);
}

SOURCEPP_API void gamepp_wait(gamepp_game_instance_handle_t handle, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::gameInstance(handle)->wait(sec);
}
