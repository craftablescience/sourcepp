#include <steamppc/Convert.hpp>

#include <steampp/steampp.h>

using namespace steampp;

Steam* Convert::steam(steampp_steam_handle_t handle) {
	return static_cast<Steam*>(handle);
}
