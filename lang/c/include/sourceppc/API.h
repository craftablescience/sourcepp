#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
	#define SOURCEPP_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
	#define SOURCEPP_EXPORT __attribute__((__visibility__("default")))
#else
	#define SOURCEPP_EXPORT
#endif

#ifdef __cplusplus
	#define SOURCEPP_EXTERN extern "C"
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
	#define SOURCEPP_CAST_CTOR(name) (name)
	#define SOURCEPP_UNUSED(...) (void)(__VA_ARGS__)
	#include <stddef.h>
	#include <stdint.h>
#endif

#define SOURCEPP_API SOURCEPP_EXTERN SOURCEPP_EXPORT

// For CWrap language bindings
#define SOURCEPP_FUNCTION(ns, rt, fn, ...)      SOURCEPP_API rt ns##_##fn(__VA_ARGS__)
#define SOURCEPP_STATIC(ns, cs, rt, mt, ...)    SOURCEPP_API rt ns##_##cs##_##mt(__VA_ARGS__)
#define SOURCEPP_METHOD(ns, cs, rt, mt)         SOURCEPP_API rt ns##_##cs##_##mt(ns##_##cs##_handle_t handle)
#define SOURCEPP_METHOE(ns, cs, rt, mt, ...)    SOURCEPP_API rt ns##_##cs##_##mt(ns##_##cs##_handle_t handle, __VA_ARGS__)

#define SOURCEPP_HANDLE(ns, cs)                 typedef void* ns##_##cs##_handle_t
#define SOURCEPP_TYPEDF(ns, cs, tp)             typedef tp ns##_##cs##_t

#define SOURCEPP_STRUCT_BEGIN()                 SOURCEPP_EXTERN typedef struct
#define SOURCEPP_STRUCT_FIELD(tp, nm, df)       tp nm
#define SOURCEPP_STRUCT_END(ns, st)             ns##_##st##_t

#define SOURCEPP_ENUM_BEGIN()                   SOURCEPP_EXTERN typedef enum
#define SOURCEPP_ENUM_VALUD(ns, en, nm)         ns##_##en##_##nm
#define SOURCEPP_ENUM_VALUE(ns, en, nm, vl)     ns##_##en##_##nm = vl
#define SOURCEPP_ENUM_END(ns, en)               ns##_##en##_e

#define SOURCEPP_GLOBAL_EXT(ns, tp, nm, vl)     extern tp ns##_##nm;
#define SOURCEPP_GLOBAL_INT(ns, tp, nm, vl)     tp ns##_##nm = vl;
#define SOURCEPP_CLSVAR_EXT(ns, cs, tp, nm, vl) extern tp ns##_##cs##_##nm;
#define SOURCEPP_CLSVAR_INT(ns, cs, tp, nm, vl) tp ns##_##cs##_##nm = vl;
