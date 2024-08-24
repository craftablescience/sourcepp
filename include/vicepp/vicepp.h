#pragma once

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace vicepp {

// https://developer.valvesoftware.com/wiki/ICE
// https://developer.valvesoftware.com/wiki/Encrypted_Key_Values
// https://wiki.alliedmods.net/Vice_keys
namespace KnownCodes {

constexpr std::string_view DEFAULT = "x9Ke0BY7";
constexpr std::string_view CONTAGION_WEAPONS = "fUk0fF69";
constexpr std::string_view CONTAGION_SCRIPTS = "5!rrFz6p";
constexpr std::string_view COUNTER_STRIKE_SOURCE = "d7NSuLq2";
constexpr std::string_view COUNTER_STRIKE_GLOBAL_OFFENSIVE = COUNTER_STRIKE_SOURCE;
constexpr std::string_view COUNTER_STRIKE_2 = COUNTER_STRIKE_GLOBAL_OFFENSIVE;
constexpr std::string_view COUNTER_STRIKE_PROMOD = "H1aRQ0n1";
constexpr std::string_view DAY_OF_DEFEAT_SOURCE = "Wl0u5B3F";
constexpr std::string_view DYSTOPIA_1_2 = "pH3apO8w";
constexpr std::string_view DYSTOPIA_1_3 = "G8stUh3F";
constexpr std::string_view GOLDEN_EYE_SOURCE = "Gr3naDes";
constexpr std::string_view HALF_LIFE_2_CTF = "R1dj3axP";
constexpr std::string_view HALF_LIFE_2_DM = DEFAULT;
constexpr std::string_view INSURGENCY = "DrA5e3EB";
constexpr std::string_view LEFT_4_DEAD_2 = "SDhfi878";
constexpr std::string_view NO_MORE_ROOM_IN_HELL = "lREeeapA";
constexpr std::string_view NUCLEAR_DAWN = "TA+*veh9";
constexpr std::string_view TACTICAL_INTERVENTION = "71B4Dt1Z";
constexpr std::string_view TEAM_FORTRESS_2 = "E2NcUkG2";
constexpr std::string_view TEAM_FORTRESS_2_ITEMS = "A5fSXbf7";
constexpr std::string_view THE_SHIP = "eb3A4m79";
constexpr std::string_view ZOMBIE_PANIC_SOURCE = "5R0ni0pZ";

constexpr std::string_view EKV_GPU_DEFAULT = "X8bU2qll";
constexpr std::string_view EKV_GPU_ALIEN_SWARM = "sW9.JupP";
constexpr std::string_view EKV_GPU_LEFT_4_DEAD_1 = "zp14Hi(]";
constexpr std::string_view EKV_GPU_LEFT_4_DEAD_2 = EKV_GPU_LEFT_4_DEAD_1;
constexpr std::string_view EKV_GPU_PORTAL_2 = "UrE66!Ap";

} // namespace KnownCodes

[[nodiscard]] std::vector<std::byte> encrypt(std::span<const std::byte> data, std::string_view code = KnownCodes::DEFAULT);

[[nodiscard]] std::vector<std::byte> decrypt(std::span<const std::byte> data, std::string_view code = KnownCodes::DEFAULT);

} // namespace vicepp
