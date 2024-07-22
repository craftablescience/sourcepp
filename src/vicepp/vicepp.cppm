module;

#include <vicepp/vicepp.h>

export module vicepp;

export {

	// vicepp.h
	namespace vicepp {
		constexpr auto CODE_DEFAULT = KnownCodes::DEFAULT;
		constexpr auto CODE_CONTAGION_WEAPONS = KnownCodes::CONTAGION_WEAPONS;
		constexpr auto CODE_CONTAGION_SCRIPTS = KnownCodes::CONTAGION_SCRIPTS;
		constexpr auto CODE_COUNTER_STRIKE_SOURCE = KnownCodes::COUNTER_STRIKE_SOURCE;
		constexpr auto CODE_COUNTER_STRIKE_GLOBAL_OFFENSIVE = KnownCodes::COUNTER_STRIKE_GLOBAL_OFFENSIVE;
		constexpr auto CODE_COUNTER_STRIKE_2 = KnownCodes::COUNTER_STRIKE_2;
		constexpr auto CODE_COUNTER_STRIKE_PROMOD = KnownCodes::COUNTER_STRIKE_PROMOD;
		constexpr auto CODE_DAY_OF_DEFEAT_SOURCE = KnownCodes::DAY_OF_DEFEAT_SOURCE;
		constexpr auto CODE_DYSTOPIA_1_2 = KnownCodes::DYSTOPIA_1_2;
		constexpr auto CODE_DYSTOPIA_1_3 = KnownCodes::DYSTOPIA_1_3;
		constexpr auto CODE_GOLDEN_EYE_SOURCE = KnownCodes::GOLDEN_EYE_SOURCE;
		constexpr auto CODE_HALF_LIFE_2_CTF = KnownCodes::HALF_LIFE_2_CTF;
		constexpr auto CODE_HALF_LIFE_2_DM = KnownCodes::HALF_LIFE_2_DM;
		constexpr auto CODE_INSURGENCY = KnownCodes::INSURGENCY;
		constexpr auto CODE_LEFT_4_DEAD_2 = KnownCodes::LEFT_4_DEAD_2;
		constexpr auto CODE_NO_MORE_ROOM_IN_HELL = KnownCodes::NO_MORE_ROOM_IN_HELL;
		constexpr auto CODE_NUCLEAR_DAWN = KnownCodes::NUCLEAR_DAWN;
		constexpr auto CODE_TACTICAL_INTERVENTION = KnownCodes::TACTICAL_INTERVENTION;
		constexpr auto CODE_TEAM_FORTRESS_2 = KnownCodes::TEAM_FORTRESS_2;
		constexpr auto CODE_TEAM_FORTRESS_2_ITEMS = KnownCodes::TEAM_FORTRESS_2_ITEMS;
		constexpr auto CODE_THE_SHIP = KnownCodes::THE_SHIP;
		constexpr auto CODE_ZOMBIE_PANIC_SOURCE = KnownCodes::ZOMBIE_PANIC_SOURCE;
		using vicepp::encrypt;
		using vicepp::decrypt;
	} // namespace vicepp

} // export
