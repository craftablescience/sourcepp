#pragma once

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace vicepp {

// https://developer.valvesoftware.com/wiki/ICE
namespace KnownCodes {

constexpr std::string_view DEFAULT = "x9Ke0BY7";
constexpr std::string_view HL2DM = DEFAULT;
constexpr std::string_view CSS = "d7NSuLq2";
constexpr std::string_view CSGO = CSS;
constexpr std::string_view DODS = "Wl0u5B3F";
constexpr std::string_view TF2 = "E2NcUkG2";
constexpr std::string_view TF2_ITEMS = "A5fSXbf7";
constexpr std::string_view L4D2 = "SDhfi878";
constexpr std::string_view THE_SHIP = "eb3A4m79";
constexpr std::string_view CONTAGION_WEAPONS = "fUk0fF69";
constexpr std::string_view CONTAGION_SCRIPTS = "5!rrFz6p";
constexpr std::string_view ZPS = "5R0ni0pZ";

} // namespace KnownCodes

[[nodiscard]] std::vector<std::byte> encrypt(std::span<const std::byte> data, std::string_view code);

[[nodiscard]] std::vector<std::byte> decrypt(std::span<const std::byte> data, std::string_view code);

} // namespace vicepp
