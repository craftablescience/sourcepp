#include "vtfpp/detail/Utility.h"

#include <cmath>

using namespace vtfpp;

uint32_t detail::getMipDim(uint8_t mip, uint16_t dim) {
	return static_cast<uint32_t>(std::ceil(static_cast<float>(dim) / static_cast<float>(1 << mip)));
}
