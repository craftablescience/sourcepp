#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
	#define SOURCEPP_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
	#define SOURCEPP_EXPORT __attribute__((__visibility__("default")))
#else
	#define SOURCEPP_EXPORT
#endif

#ifdef __cplusplus
	#define SOURCEPP_API extern "C" SOURCEPP_EXPORT
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
	#define SOURCEPP_API SOURCEPP_EXPORT
	#define SOURCEPP_CAST_CTOR(name) (name)
	#define SOURCEPP_UNUSED(...) (void)(__VA_ARGS__)
	#include <stddef.h>
	#include <stdint.h>
#endif

// For CWrap language bindings
#define SOURCEPP_FUNCTN(ns, rt, fn, ...)     SOURCEPP_API rt ns##_##fn(__VA_ARGS__)
#define SOURCEPP_STATIC(ns, cs, rt, mt, ...) SOURCEPP_API rt ns##_##cs##_##mt(__VA_ARGS__)
#define SOURCEPP_METHOD(ns, cs, rt, mt)      SOURCEPP_API rt ns##_##cs##_##mt(ns##_##cs##_handle_t handle)
#define SOURCEPP_METHOE(ns, cs, rt, mt, ...) SOURCEPP_API rt ns##_##cs##_##mt(ns##_##cs##_handle_t handle, __VA_ARGS__)
#define SOURCEPP_HANDLE(ns, cs)              typedef void* ns##_##cs##_handle_t
#define SOURCEPP_TYPEDF(ns, cs, tp)          typedef tp ns##_##cs##_t
// todo: global values
// todo: types
// todo: enums
