#include <sourcepp/math/Integer.h>

using namespace sourcepp;

uint16_t math::getPaddingForAlignment(uint16_t alignment, uint64_t n) {
	if (const auto rest = n % alignment; rest > 0) {
		return alignment - rest;
	}
	return 0;
}
