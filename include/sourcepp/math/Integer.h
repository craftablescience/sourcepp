#pragma once

#include <cstdint>
#include <type_traits>

// Integer types are intentionally outside the sourcepp namespace
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

namespace sourcepp::math {

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

} // namespace sourcepp::math
