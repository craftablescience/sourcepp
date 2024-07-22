#include <steamppc/Convert.hpp>

import steampp;

using namespace steampp;

Steam* Convert::steam(steampp_steam_handle_t handle) {
	return static_cast<Steam*>(handle);
}
