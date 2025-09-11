#pragma once

#include <type_traits>

#include <sourcepp/Math.h>

namespace sourcepp::bits {

template<sourcepp::math::Arithmetic A>
class LERep : public std::array<std::byte, sizeof(A)> {
	using uint_according = typename std::conditional<sizeof(A) == 8,
		uint64_t,
	typename std::conditional<sizeof(A) == 4,
		uint32_t,
	typename std::conditional<sizeof(A) == 2,
		uint16_t,
	uint8_t>::type>::type>::type;
public:
	constexpr operator A() const {
		uint_according ret = 0;
		for (size_t offs = 0; auto &b : *this) {
			ret |= (static_cast<uint_according>(b) << offs) & (uint_according(0xFFu) << offs);
			offs += 8;
		}
		return *reinterpret_cast<A *>(&ret);
	}
	constexpr LERep &operator=(const A &v) {
		auto in = *reinterpret_cast<const uint_according *>(&v);
		for (size_t offs = 0; auto &b : *this) {
			b = static_cast<std::byte>((in >> offs) & uint_according(0xFFu));
			offs += 8;
		}
		return *this;
	}
	template<sourcepp::math::Arithmetic B> requires (std::convertible_to<B, A> || std::is_same_v<A, half>)
	constexpr LERep(const B &u) { this->operator=(static_cast<A>(u)); }

	template<sourcepp::math::Arithmetic B> requires std::convertible_to<B, A>
	constexpr LERep(const LERep<B> &u) { this->operator=(u.operator A()); }

	template<sourcepp::math::Arithmetic B> requires std::convertible_to<A, B>
	constexpr operator B() const { return static_cast<B>(this->operator A()); }
};

} // namespace sourcepp::bits
