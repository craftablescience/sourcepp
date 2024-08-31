#pragma once

#include <type_traits>

// Helpers
#define SOURCEPP_CONCAT_INNER(a, b) a##b
#define SOURCEPP_CONCAT(a, b) SOURCEPP_CONCAT_INNER(a, b)

/// Adds the current line number to the given base
#define SOURCEPP_UNIQUE_NAME(base) SOURCEPP_CONCAT(base, __LINE__)

/// Defines bitwise operators for an enum or enum class
#define SOURCEPP_BITFLAGS_ENUM(Enum)                                \
    inline constexpr Enum operator|(Enum lhs, Enum rhs) {           \
        return static_cast<Enum>(                                   \
            static_cast<std::underlying_type_t<Enum>>(lhs) |        \
            static_cast<std::underlying_type_t<Enum>>(rhs));        \
    }                                                               \
    inline constexpr Enum operator&(Enum lhs, Enum rhs) {           \
        return static_cast<Enum>(                                   \
            static_cast<std::underlying_type_t<Enum>>(lhs) &        \
            static_cast<std::underlying_type_t<Enum>>(rhs));        \
    }                                                               \
    inline constexpr Enum operator^(Enum lhs, Enum rhs) {           \
        return static_cast<Enum>(                                   \
            static_cast<std::underlying_type_t<Enum>>(lhs) ^        \
            static_cast<std::underlying_type_t<Enum>>(rhs));        \
    }                                                               \
    inline constexpr Enum operator~(Enum e) {                       \
        return static_cast<Enum>(                                   \
            ~static_cast<std::underlying_type_t<Enum>>(e));         \
    }                                                               \
    inline Enum& operator|=(Enum& lhs, Enum rhs) {                  \
        return lhs = static_cast<Enum>(                             \
                   static_cast<std::underlying_type_t<Enum>>(lhs) | \
                   static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }                                                               \
    inline Enum& operator&=(Enum& lhs, Enum rhs) {                  \
        return lhs = static_cast<Enum>(                             \
                   static_cast<std::underlying_type_t<Enum>>(lhs) & \
                   static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }                                                               \
    inline Enum& operator^=(Enum& lhs, Enum rhs) {                  \
        return lhs = static_cast<Enum>(                             \
                   static_cast<std::underlying_type_t<Enum>>(lhs) ^ \
                   static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }
