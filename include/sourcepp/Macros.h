#pragma once

#include <type_traits>

// Helpers
#define SOURCEPP_CONCAT_DETAIL(a, b) a##b
/// Token pasting outside a macro context.
#define SOURCEPP_CONCAT(a, b) SOURCEPP_CONCAT_DETAIL(a, b)

/// Create a breakpoint in debug
#ifdef DEBUG
	#if defined(_MSC_VER)
		#define SOURCEPP_DEBUG_BREAK __debugbreak()
	#elif defined(__linux__)
		#include <csignal>
		#define SOURCEPP_DEBUG_BREAK std::raise(SIGINT)
	#endif
#endif
#ifndef SOURCEPP_DEBUG_BREAK
	#define SOURCEPP_DEBUG_BREAK
#endif

/// Adds the current line number to the given base
#define SOURCEPP_UNIQUE_NAME(base) SOURCEPP_CONCAT(base, __LINE__)

/// Defines bitwise operators for an enum or enum class
#define SOURCEPP_BITFLAGS_ENUM(Enum)                          \
	template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
	inline constexpr Enum operator|(Enum lhs, T rhs) {        \
		return static_cast<Enum>(                             \
			static_cast<std::underlying_type_t<Enum>>(lhs) |  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
	}                                                         \
	template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
	inline constexpr Enum operator&(Enum lhs, T rhs) {        \
		return static_cast<Enum>(                             \
			static_cast<std::underlying_type_t<Enum>>(lhs) &  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
	}                                                         \
	template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
    inline constexpr Enum operator^(Enum lhs, T rhs) {        \
		return static_cast<Enum>(                             \
			static_cast<std::underlying_type_t<Enum>>(lhs) ^  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
    }                                                         \
    inline constexpr Enum operator~(Enum e) {                 \
		return static_cast<Enum>(                             \
			~static_cast<std::underlying_type_t<Enum>>(e));   \
    }                                                         \
    template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
    inline constexpr Enum& operator|=(Enum& lhs, T rhs) {     \
		return lhs = static_cast<Enum>(                       \
			static_cast<std::underlying_type_t<Enum>>(lhs) |  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
    }                                                         \
    template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
    inline constexpr Enum& operator&=(Enum& lhs, T rhs) {     \
		return lhs = static_cast<Enum>(                       \
			static_cast<std::underlying_type_t<Enum>>(lhs) &  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
    }                                                         \
    template<typename T>                                      \
	requires std::is_same_v<T, Enum> || std::is_integral_v<T> \
    inline constexpr Enum& operator^=(Enum& lhs, T rhs) {     \
		return lhs = static_cast<Enum>(                       \
			static_cast<std::underlying_type_t<Enum>>(lhs) ^  \
			static_cast<std::underlying_type_t<Enum>>(rhs));  \
    }

/// Indirected `()`.
#define SOURCEPP_UNIT ()

#define SOURCEPP_EXPAND8(...) SOURCEPP_EXPAND7(SOURCEPP_EXPAND7(SOURCEPP_EXPAND7(SOURCEPP_EXPAND7(__VA_ARGS__))))
#define SOURCEPP_EXPAND7(...) SOURCEPP_EXPAND6(SOURCEPP_EXPAND6(SOURCEPP_EXPAND6(SOURCEPP_EXPAND6(__VA_ARGS__))))
#define SOURCEPP_EXPAND6(...) SOURCEPP_EXPAND5(SOURCEPP_EXPAND5(SOURCEPP_EXPAND5(SOURCEPP_EXPAND5(__VA_ARGS__))))
#define SOURCEPP_EXPAND5(...) SOURCEPP_EXPAND4(SOURCEPP_EXPAND4(SOURCEPP_EXPAND4(SOURCEPP_EXPAND4(__VA_ARGS__))))
#define SOURCEPP_EXPAND4(...) SOURCEPP_EXPAND3(SOURCEPP_EXPAND3(SOURCEPP_EXPAND3(SOURCEPP_EXPAND3(__VA_ARGS__))))
#define SOURCEPP_EXPAND3(...) SOURCEPP_EXPAND2(SOURCEPP_EXPAND2(SOURCEPP_EXPAND2(SOURCEPP_EXPAND2(__VA_ARGS__))))
#define SOURCEPP_EXPAND2(...) SOURCEPP_EXPAND1(SOURCEPP_EXPAND1(SOURCEPP_EXPAND1(SOURCEPP_EXPAND1(__VA_ARGS__))))
#define SOURCEPP_EXPAND1(...) __VA_ARGS__

#define SOURCEPP_ID(...) __VA_ARGS__

/// Apply a unary macro to each of `__VA_ARGS__`.
/// \param sep Nullary function-like macro expected to expand to a separator. For rationale, see \ref SOURCEPP_THUNK_COMMA.
/// \param macro Unary macro.
/// \param ... List of first arguments per call to `macro`.
#define SOURCEPP_FOREACH0_SEP(sep, macro, ...) \
	__VA_OPT__(SOURCEPP_EXPAND5(SOURCEPP_FOREACH0_SEP_HELPER(sep, macro, __VA_ARGS__)))
#define SOURCEPP_FOREACH0_SEP_HELPER(sep, macro, x, ...) \
	macro(x) \
	__VA_OPT__(sep SOURCEPP_UNIT SOURCEPP_DEFER_FOREACH0_SEP_HELPER SOURCEPP_UNIT (sep, macro, __VA_ARGS__))
#define SOURCEPP_DEFER_FOREACH0_SEP_HELPER() SOURCEPP_FOREACH0_SEP_HELPER

/// Apply a binary macro to each of `__VA_ARGS__` with a set first argument.
/// \param sep Nullary function-like macro expected to expand to a separator. For rationale, see \ref SOURCEPP_THUNK_COMMA.
/// \param macro Binary macro.
/// \param a Always the first argument to `macro`.
/// \param ... List of second arguments per call to `macro`
#define SOURCEPP_FOREACH1_SEP(sep, macro, a, ...) \
	__VA_OPT__(SOURCEPP_EXPAND5(SOURCEPP_FOREACH1_SEP_HELPER(sep, macro, a, __VA_ARGS__)))
#define SOURCEPP_FOREACH1_SEP_HELPER(sep, macro, a, x, ...) \
	macro(a, x) \
	__VA_OPT__(sep SOURCEPP_UNIT SOURCEPP_DEFER_FOREACH1_SEP_HELPER SOURCEPP_UNIT (sep, macro, a, __VA_ARGS__))
#define SOURCEPP_DEFER_FOREACH1_SEP_HELPER() SOURCEPP_FOREACH1_SEP_HELPER

/// Nullary macro that expands to nothing.
#define SOURCEPP_THUNK_NOTHING()

/// Turn its operand into (effectively) a nullary function-like macro that expands to it.
#define SOURCEPP_THUNK(id) id SOURCEPP_THUNK_NOTHING
/// Nullary macro that expands to a comma. It is necessary to defer expansion to any commas in the
/// desired output of complex macro expansions, to prevent the preprocessor from interpreting such a comma itself.
#define SOURCEPP_THUNK_COMMA() ,

/// Convenience variant of SOURCEPP_FOREACH0_SEP with no separator.
#define SOURCEPP_FOREACH0(macro, ...) SOURCEPP_FOREACH0_SEP(SOURCEPP_THUNK_NOTHING, macro, __VA_ARGS__)
/// Convenience variant of SOURCEPP_FOREACH1_SEP with no separator.
#define SOURCEPP_FOREACH1(macro, a, ...) SOURCEPP_FOREACH1_SEP(SOURCEPP_THUNK_NOTHING, macro, a, __VA_ARGS__)

/// Callable parenthesization; identity function for 2-tuples when used bare as in:
///
///     SOURCEPP_CONS TUPLE
#define SOURCEPP_CONS(a, d) (a, d)
/// Called bare to destructure the first of a 2-tuple.
#define SOURCEPP_CAR(a, d) a
/// Called bare to destructure the second of a 2-tuple.
#define SOURCEPP_CDR(a, d) d
