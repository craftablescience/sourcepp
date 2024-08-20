#pragma once

#include <type_traits>

#define SOURCEPP_SETUP_BITWISE_ENUM_CLASS(Enum)                     \
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
