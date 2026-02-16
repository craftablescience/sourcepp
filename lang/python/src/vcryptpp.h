// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>

namespace py = nanobind;
using namespace py::literals;

#include <vcryptpp/vcryptpp.h>

namespace vcryptpp {

inline void register_python(py::module_& m) {
	auto vcryptpp = m.def_submodule("vcryptpp");
	using namespace vcryptpp;

	{
		auto VFONT = vcryptpp.def_submodule("VFONT");
		using namespace VFONT;

		VFONT.attr("SIGNATURE") = SIGNATURE;

		VFONT.attr("MAGIC") = MAGIC;

		VFONT.def("encrypt_bytes", [](const py::bytes& data, uint8_t saltSize = 2) {
			const auto d = encrypt({static_cast<const std::byte*>(data.data()), data.size()}, saltSize);
			return py::bytes{d.data(), d.size()};
		}, "data"_a, "salt_size"_a = 2);

		VFONT.def("decrypt_bytes", [](const py::bytes& data) {
			const auto d = decrypt({static_cast<const std::byte*>(data.data()), data.size()});
			return py::bytes{d.data(), d.size()};
		}, "data"_a);
	}

	{
		auto VICE = vcryptpp.def_submodule("VICE");
		using namespace VICE;

		{
			auto KnownCodes = VICE.def_submodule("KnownCodes");
			using namespace KnownCodes;

			KnownCodes.attr("DEFAULT")                         = py::bytes{DEFAULT.data(),                         DEFAULT.size()};
			KnownCodes.attr("BLOODY_GOOD_TIME")                = py::bytes{BLOODY_GOOD_TIME.data(),                BLOODY_GOOD_TIME.size()};
			KnownCodes.attr("CONTAGION_WEAPONS")               = py::bytes{CONTAGION_WEAPONS.data(),               CONTAGION_WEAPONS.size()};
			KnownCodes.attr("CONTAGION_SCRIPTS")               = py::bytes{CONTAGION_SCRIPTS.data(),               CONTAGION_SCRIPTS.size()};
			KnownCodes.attr("COUNTER_STRIKE_SOURCE")           = py::bytes{COUNTER_STRIKE_SOURCE.data(),           COUNTER_STRIKE_SOURCE.size()};
			KnownCodes.attr("COUNTER_STRIKE_GLOBAL_OFFENSIVE") = py::bytes{COUNTER_STRIKE_GLOBAL_OFFENSIVE.data(), COUNTER_STRIKE_GLOBAL_OFFENSIVE.size()};
			KnownCodes.attr("COUNTER_STRIKE_2")                = py::bytes{COUNTER_STRIKE_2.data(),                COUNTER_STRIKE_2.size()};
			KnownCodes.attr("COUNTER_STRIKE_PROMOD")           = py::bytes{COUNTER_STRIKE_PROMOD.data(),           COUNTER_STRIKE_PROMOD.size()};
			KnownCodes.attr("DAY_OF_DEFEAT_SOURCE")            = py::bytes{DAY_OF_DEFEAT_SOURCE.data(),            DAY_OF_DEFEAT_SOURCE.size()};
			KnownCodes.attr("DYSTOPIA_1_2")                    = py::bytes{DYSTOPIA_1_2.data(),                    DYSTOPIA_1_2.size()};
			KnownCodes.attr("DYSTOPIA_1_3")                    = py::bytes{DYSTOPIA_1_3.data(),                    DYSTOPIA_1_3.size()};
			KnownCodes.attr("FORTRESS_FOREVER_PRE_GREENLIGHT") = py::bytes{FORTRESS_FOREVER_PRE_GREENLIGHT.data(), FORTRESS_FOREVER_PRE_GREENLIGHT.size()};
			KnownCodes.attr("GOLDEN_EYE_SOURCE")               = py::bytes{GOLDEN_EYE_SOURCE.data(),               GOLDEN_EYE_SOURCE.size()};
			KnownCodes.attr("HALF_LIFE_2_CTF")                 = py::bytes{HALF_LIFE_2_CTF.data(),                 HALF_LIFE_2_CTF.size()};
			KnownCodes.attr("HALF_LIFE_2_DM")                  = py::bytes{HALF_LIFE_2_DM.data(),                  HALF_LIFE_2_DM.size()};
			KnownCodes.attr("INSURGENCY")                      = py::bytes{INSURGENCY.data(),                      INSURGENCY.size()};
			KnownCodes.attr("LEFT_4_DEAD_2")                   = py::bytes{LEFT_4_DEAD_2.data(),                   LEFT_4_DEAD_2.size()};
			KnownCodes.attr("NO_MORE_ROOM_IN_HELL")            = py::bytes{NO_MORE_ROOM_IN_HELL.data(),            NO_MORE_ROOM_IN_HELL.size()};
			KnownCodes.attr("NUCLEAR_DAWN")                    = py::bytes{NUCLEAR_DAWN.data(),                    NUCLEAR_DAWN.size()};
			KnownCodes.attr("TACTICAL_INTERVENTION")           = py::bytes{TACTICAL_INTERVENTION.data(),           TACTICAL_INTERVENTION.size()};
			KnownCodes.attr("TEAM_FORTRESS_2")                 = py::bytes{TEAM_FORTRESS_2.data(),                 TEAM_FORTRESS_2.size()};
			KnownCodes.attr("TEAM_FORTRESS_2_ITEMS")           = py::bytes{TEAM_FORTRESS_2_ITEMS.data(),           TEAM_FORTRESS_2_ITEMS.size()};
			KnownCodes.attr("FAIRY_TALE_BUSTERS")              = py::bytes{FAIRY_TALE_BUSTERS.data(),              FAIRY_TALE_BUSTERS.size()};
			KnownCodes.attr("THE_SHIP")                        = py::bytes{THE_SHIP.data(),                        THE_SHIP.size()};
			KnownCodes.attr("VALVE_TRACKER")                   = py::bytes{VALVE_TRACKER.data(),                   VALVE_TRACKER.size()};
			KnownCodes.attr("ZOMBIE_PANIC_SOURCE")             = py::bytes{ZOMBIE_PANIC_SOURCE.data(),             ZOMBIE_PANIC_SOURCE.size()};

			KnownCodes.attr("GPU_DEFAULT")                     = py::bytes{GPU_DEFAULT.data(),                     GPU_DEFAULT.size()};
			KnownCodes.attr("GPU_ALIEN_SWARM")                 = py::bytes{GPU_ALIEN_SWARM.data(),                 GPU_ALIEN_SWARM.size()};
			KnownCodes.attr("GPU_DOTA_2")                      = py::bytes{GPU_DOTA_2.data(),                      GPU_DOTA_2.size()};
			KnownCodes.attr("GPU_HALF_LIFE_2_EPISODE_2")       = py::bytes{GPU_HALF_LIFE_2_EPISODE_2.data(),       GPU_HALF_LIFE_2_EPISODE_2.size()};
			KnownCodes.attr("GPU_LEFT_4_DEAD_1")               = py::bytes{GPU_LEFT_4_DEAD_1.data(),               GPU_LEFT_4_DEAD_1.size()};
			KnownCodes.attr("GPU_LEFT_4_DEAD_2")               = py::bytes{GPU_LEFT_4_DEAD_2.data(),               GPU_LEFT_4_DEAD_2.size()};
			KnownCodes.attr("GPU_TEAM_FORTRESS_2")             = py::bytes{GPU_TEAM_FORTRESS_2.data(),             GPU_TEAM_FORTRESS_2.size()};
			KnownCodes.attr("GPU_PORTAL_2")                    = py::bytes{GPU_PORTAL_2.data(),                    GPU_PORTAL_2.size()};
		}

		VICE.def("decrypt_bytes", [](const py::bytes& data, const py::bytes& code = py::bytes{KnownCodes::DEFAULT.data(), KnownCodes::DEFAULT.size()}) {
			const auto d = decrypt({static_cast<const std::byte*>(data.data()), data.size()}, code.c_str());
			return py::bytes{d.data(), d.size()};
		}, "data"_a, "code"_a = KnownCodes::DEFAULT);

		VICE.def("decrypt_str", [](std::string_view data, const py::bytes& code = py::bytes{KnownCodes::DEFAULT.data(), KnownCodes::DEFAULT.size()}) -> std::string {
			const auto d = decrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code.c_str());
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		}, "data"_a, "code"_a = KnownCodes::DEFAULT);

		VICE.def("encrypt_bytes", [](const py::bytes& data, const py::bytes& code = py::bytes{KnownCodes::DEFAULT.data(), KnownCodes::DEFAULT.size()}) {
			const auto d = encrypt({static_cast<const std::byte*>(data.data()), data.size()}, code.c_str());
			return py::bytes{d.data(), d.size()};
		}, "data"_a, "code"_a = KnownCodes::DEFAULT);

		VICE.def("encrypt_str", [](std::string_view data, const py::bytes& code = py::bytes{KnownCodes::DEFAULT.data(), KnownCodes::DEFAULT.size()}) -> std::string {
			const auto d = encrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code.c_str());
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		}, "data"_a, "code"_a = KnownCodes::DEFAULT);
	}
}

} // namespace vcryptpp
