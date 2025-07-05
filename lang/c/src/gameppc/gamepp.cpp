#include <gameppc/gamepp.h>

#include <gamepp/gamepp.h>

#include <gameppc/Convert.hpp>
#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>

using namespace gamepp;

SOURCEPP_STATIC(gamepp, game_instance, gamepp_game_instance_handle_t, find) {
	const auto instance = GameInstance::find();
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_STATIC(gamepp, game_instance, gamepp_game_instance_handle_t, find_with_name, const char* windowNameOverride) {
	SOURCEPP_EARLY_RETURN_VAL(windowNameOverride, nullptr);

	const auto instance = GameInstance::find(windowNameOverride);
	if (!instance) {
		return nullptr;
	}
	return new GameInstance{*instance};
}

SOURCEPP_STATIC(gamepp, game_instance, void, free, gamepp_game_instance_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::gameInstance(*handle);
	*handle = nullptr;
}

SOURCEPP_METHOD(gamepp, game_instance, sourcepp_string_t, get_window_title) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::gameInstance(handle)->getWindowTitle());
}

SOURCEPP_METHOD(gamepp, game_instance, int, get_window_pos_x) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowPos()[0];
}

SOURCEPP_METHOD(gamepp, game_instance, int, get_window_pos_y) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowPos()[1];
}

SOURCEPP_METHOD(gamepp, game_instance, int, get_window_width) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowSize()[0];
}

SOURCEPP_METHOD(gamepp, game_instance, int, get_window_height) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::gameInstance(handle)->getWindowSize()[1];
}

SOURCEPP_METHOE(gamepp, game_instance, void, command, const char* command) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->command(command));
}

SOURCEPP_METHOE(gamepp, game_instance, void, input_begin, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->inputBegin(input));
}

SOURCEPP_METHOE(gamepp, game_instance, void, input_end, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->inputEnd(input));
}

SOURCEPP_METHOE(gamepp, game_instance, void, input_once, const char* input) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->inputOnce(input));
}

SOURCEPP_METHOE(gamepp, game_instance, void, input_hold, const char* input, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->inputHold(input, sec));
}

SOURCEPP_METHOE(gamepp, game_instance, void, wait, double sec) {
	SOURCEPP_EARLY_RETURN(handle);

	SOURCEPP_UNUSED(Convert::gameInstance(handle)->wait(sec));
}
