#include <gameppc/Convert.hpp>

#include <gamepp/gamepp.h>

using namespace gamepp;

GameInstance* Convert::gameInstance(gamepp_game_instance_handle_t handle) {
	return static_cast<GameInstance*>(handle);
}
