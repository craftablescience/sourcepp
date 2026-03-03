#pragma once

#define SOURCEPP_EARLY_RETURN(var) \
	do {                           \
		if (!(var)) {              \
			return;                \
		}                          \
	} while (0)

#define SOURCEPP_EARLY_RETURN_VAL(var, value) \
	do {                                      \
		if (!(var)) {                         \
			return value;                     \
		}                                     \
	} while (0)

#ifdef __cplusplus
	#define SOURCEPP_CAST_CTOR(name) name
	#define SOURCEPP_UNUSED(...) static_cast<void>(__VA_ARGS__)
	#include <cstddef>
	#include <cstdint>
	using std::size_t;
	using std::int8_t;
	using std::uint8_t;
	using std::int16_t;
	using std::uint16_t;
	using std::int32_t;
	using std::uint32_t;
	using std::int64_t;
	using std::uint64_t;
#else
	#define SOURCEPP_CAST_CTOR(name) (name)
	#define SOURCEPP_UNUSED(...) (void)(__VA_ARGS__)
	#include <stddef.h>
	#include <stdint.h>
#endif

// C++ conversion routines
#ifdef __cplusplus

namespace sourceppc::convert {

template<typename T, typename U>
[[nodiscard]] T* handle(U* handle) {
	return static_cast<T*>(handle);
}

} // namespace sourceppc::convert

#endif
