#include <gameppc/gamepp.h>

#include <sourceppc/Helpers.h>

using namespace gamepp;
using namespace sourceppc;

SOURCEPP_API gamepp_game_instance_handle_t gamepp_game_instance_find() {
	const auto instance = GameInstance::find();
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_API gamepp_game_instance_handle_t gamepp_game_instance_find_with_name(const char* windowNameOverride) {
	SOURCEPP_EARLY_RETURN_VAL(windowNameOverride, nullptr);

	const auto instance = GameInstance::find(windowNameOverride);
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_API void gamepp_game_instance_free(gamepp_game_instance_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<GameInstance>(*handle);
	*handle = nullptr;
}

SOURCEPP_API sourcepp_string_t gamepp_game_instance_get_window_title(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<GameInstance>(handle)->getWindowTitle());
}

SOURCEPP_API int gamepp_game_instance_get_window_pos_x(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<GameInstance>(handle)->getWindowPos()[0];
}

SOURCEPP_API int gamepp_game_instance_get_window_pos_y(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<GameInstance>(handle)->getWindowPos()[1];
}

SOURCEPP_API int gamepp_game_instance_get_window_width(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<GameInstance>(handle)->getWindowSize()[0];
}

SOURCEPP_API int gamepp_game_instance_get_window_height(gamepp_game_instance_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<GameInstance>(handle)->getWindowSize()[1];
}

SOURCEPP_API void gamepp_game_instance_command(gamepp_game_instance_handle_t handle, const char* command) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->command(command));
}

SOURCEPP_API void gamepp_game_instance_input_begin(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->inputBegin(input));
}

SOURCEPP_API void gamepp_game_instance_input_end(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->inputEnd(input));
}

SOURCEPP_API void gamepp_game_instance_input_once(gamepp_game_instance_handle_t handle, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->inputOnce(input));
}

SOURCEPP_API void gamepp_game_instance_input_hold(gamepp_game_instance_handle_t handle, const char* input, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->inputHold(input, sec));
}

SOURCEPP_API void gamepp_game_instance_wait(gamepp_game_instance_handle_t handle, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(convert::handle<GameInstance>(handle)->wait(sec));
}
