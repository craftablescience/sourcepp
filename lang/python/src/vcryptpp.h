#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>

namespace py = nanobind;

#include <vcryptpp/vcryptpp.h>

namespace vcryptpp {

inline void register_python(py::module_& m) {
	auto vcryptpp = m.def_submodule("vcryptpp");
	using namespace vcryptpp;

	{
		auto VFONT = vcryptpp.def_submodule("VFONT");
		using namespace VFONT;

		VFONT.attr("IDENTIFIER") = IDENTIFIER;

		VFONT.attr("MAGIC") = MAGIC;

		VFONT.def("decrypt_bytes", [](const py::bytes& data) {
			const auto d = decrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()});
			return py::bytes{d.data(), d.size()};
		}, py::arg("data"));
	}

	{
		auto VICE = vcryptpp.def_submodule("VICE");
		using namespace VICE;

		{
			auto KnownCodes = VICE.def_submodule("KnownCodes");
			using namespace KnownCodes;

			KnownCodes.attr("DEFAULT")                         = DEFAULT;
			KnownCodes.attr("CONTAGION_WEAPONS")               = CONTAGION_WEAPONS;
			KnownCodes.attr("CONTAGION_SCRIPTS")               = CONTAGION_SCRIPTS;
			KnownCodes.attr("COUNTER_STRIKE_SOURCE")           = COUNTER_STRIKE_SOURCE;
			KnownCodes.attr("COUNTER_STRIKE_GLOBAL_OFFENSIVE") = COUNTER_STRIKE_GLOBAL_OFFENSIVE;
			KnownCodes.attr("COUNTER_STRIKE_2")                = COUNTER_STRIKE_2;
			KnownCodes.attr("COUNTER_STRIKE_PROMOD")           = COUNTER_STRIKE_PROMOD;
			KnownCodes.attr("DAY_OF_DEFEAT_SOURCE")            = DAY_OF_DEFEAT_SOURCE;
			KnownCodes.attr("DYSTOPIA_1_2")                    = DYSTOPIA_1_2;
			KnownCodes.attr("DYSTOPIA_1_3")                    = DYSTOPIA_1_3;
			KnownCodes.attr("GOLDEN_EYE_SOURCE")               = GOLDEN_EYE_SOURCE;
			KnownCodes.attr("HALF_LIFE_2_CTF")                 = HALF_LIFE_2_CTF;
			KnownCodes.attr("HALF_LIFE_2_DM")                  = HALF_LIFE_2_DM;
			KnownCodes.attr("INSURGENCY")                      = INSURGENCY;
			KnownCodes.attr("LEFT_4_DEAD_2")                   = LEFT_4_DEAD_2;
			KnownCodes.attr("NO_MORE_ROOM_IN_HELL")            = NO_MORE_ROOM_IN_HELL;
			KnownCodes.attr("NUCLEAR_DAWN")                    = NUCLEAR_DAWN;
			KnownCodes.attr("TACTICAL_INTERVENTION")           = TACTICAL_INTERVENTION;
			KnownCodes.attr("TEAM_FORTRESS_2")                 = TEAM_FORTRESS_2;
			KnownCodes.attr("TEAM_FORTRESS_2_ITEMS")           = TEAM_FORTRESS_2_ITEMS;
			KnownCodes.attr("THE_SHIP")                        = THE_SHIP;
			KnownCodes.attr("ZOMBIE_PANIC_SOURCE")             = ZOMBIE_PANIC_SOURCE;

			KnownCodes.attr("EKV_GPU_DEFAULT")       = EKV_GPU_DEFAULT;
			KnownCodes.attr("EKV_GPU_ALIEN_SWARM")   = EKV_GPU_ALIEN_SWARM;
			KnownCodes.attr("EKV_GPU_LEFT_4_DEAD_1") = EKV_GPU_LEFT_4_DEAD_1;
			KnownCodes.attr("EKV_GPU_LEFT_4_DEAD_2") = EKV_GPU_LEFT_4_DEAD_2;
			KnownCodes.attr("EKV_GPU_PORTAL_2")      = EKV_GPU_PORTAL_2;
		}

		VICE.def("decrypt_bytes", [](const py::bytes& data, std::string_view code = KnownCodes::DEFAULT) {
			const auto d = decrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code);
			return py::bytes{d.data(), d.size()};
		}, py::arg("data"), py::arg("code") = KnownCodes::DEFAULT);

		VICE.def("decrypt_str", [](std::string_view data, std::string_view code = KnownCodes::DEFAULT) -> std::string {
			const auto d = decrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code);
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		}, py::arg("data"), py::arg("code") = KnownCodes::DEFAULT);

		VICE.def("encrypt_bytes", [](const py::bytes& data, std::string_view code = KnownCodes::DEFAULT) {
			const auto d = encrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code);
			return py::bytes{d.data(), d.size()};
		}, py::arg("data"), py::arg("code") = KnownCodes::DEFAULT);

		VICE.def("encrypt_str", [](std::string_view data, std::string_view code = KnownCodes::DEFAULT) -> std::string {
			const auto d = encrypt({reinterpret_cast<const std::byte*>(data.data()), data.size()}, code);
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		}, py::arg("data"), py::arg("code") = KnownCodes::DEFAULT);
	}
}

} // namespace vcryptpp
