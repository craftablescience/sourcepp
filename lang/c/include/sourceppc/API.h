#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
	// Little hacky but works great. NO C++ ALLOWED!!
    #ifdef __cplusplus
		#define SOURCEPP_EXPORT __declspec(dllexport)
	#else
		#define SOURCEPP_EXPORT __declspec(dllimport)
	#endif
#elif defined(__GNUC__)
	#define SOURCEPP_EXPORT __attribute__((__visibility__("default")))
#else
	#define SOURCEPP_EXPORT
#endif

#ifdef __cplusplus
	#define SOURCEPP_EXTERN extern "C"
	#define SOURCEPP_EXTERNVAR SOURCEPP_EXTERN SOURCEPP_EXPORT
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
	#define SOURCEPP_EXTERN
	#define SOURCEPP_EXTERNVAR extern SOURCEPP_EXPORT
	#define SOURCEPP_CAST_CTOR(name) (name)
	#define SOURCEPP_UNUSED(...) (void)(__VA_ARGS__)
	#include <stddef.h>
	#include <stdint.h>
#endif

#define SOURCEPP_API SOURCEPP_EXTERN SOURCEPP_EXPORT

// C++ conversion routines
#ifdef __cplusplus

namespace sourceppc::convert {

template<typename T, typename U>
[[nodiscard]] T* handle(U* handle) {
	return static_cast<T*>(handle);
}

} // namespace sourceppc::convert

#endif
