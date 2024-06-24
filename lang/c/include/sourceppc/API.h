#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
	#define SOURCEPP_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
	#define SOURCEPP_EXPORT  __attribute__((__visibility__("default")))
#else
	#define SOURCEPP_EXPORT
#endif

#ifdef __cplusplus
	#define SOURCEPP_API extern "C" SOURCEPP_EXPORT
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
	#include <stddef.h>
	#include <stdint.h>
#endif
