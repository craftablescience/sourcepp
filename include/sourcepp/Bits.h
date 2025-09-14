#pragma once

#include <type_traits>

#include <sourcepp/Math.h>

namespace sourcepp::bits {

/// A view of a given numeric type `A` that is always little-endian in memory.
/// Typically becomes no-op at `-O2` on LE targets.
///
/// Any instance of `*reinterpret_cast<NUMERIC_TYPE *>` etc., or use of numeric types
/// in structures destined for I/O buffers is suspect, and, in either case, should likely
/// be replaced by some invocation of this class. This can be syntactically cumbersome,
/// so convenient aliases are provided with f32le, etc.
///
/// Special provisions for arithmetic outside possible implicit conversions are intentionally omitted.
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
	/// Conversion to native byte order.
	/// `LERep<A>` ⇒ `A`.
	constexpr operator A() const {
		uint_according ret = 0;
		for (size_t offs = 0; auto &b : *this) {
			ret |= (static_cast<uint_according>(b) << offs) & (uint_according(0xFFu) << offs);
			offs += 8;
		}
		return *reinterpret_cast<A *>(&ret);
	}

	/// Conversion from native byte order.
	/// `A` ⇒ `LERep<A>`.
	constexpr LERep &operator=(const A &v) {
		auto in = *reinterpret_cast<const uint_according *>(&v);
		for (size_t offs = 0; auto &b : *this) {
			b = static_cast<std::byte>((in >> offs) & uint_according(0xFFu));
			offs += 8;
		}
		return *this;
	}

	/// Convenience for arithmetic conversion from some B.
	/// (`B` ⇒ `A`) ⇒ (`B` ⇒ `LERep<A>`).
	template<sourcepp::math::Arithmetic B> requires (std::convertible_to<B, A> || std::is_same_v<A, half>)
	constexpr LERep(const B &u) { this->operator=(static_cast<A>(u)); }

	/// Convenience for arithmetic conversion between LERep of distinct but arithmetically convertible types.
	/// (`B` ⇒ `A`) ⇒ (`LERep<B>` ⇒ `LERep<A>`).
	template<sourcepp::math::Arithmetic B> requires std::convertible_to<B, A>
	constexpr LERep(const LERep<B> &u) { this->operator=(u.operator A()); }

	/// Convenience for arithmetic conversion to some B.
	/// (`LERep<A>` ⇒ `A`) ∧ (`A` ⇒ `B`) ⇒ (`LERep<A>` ⇒ `B`).
	template<sourcepp::math::Arithmetic B> requires std::convertible_to<A, B>
	constexpr operator B() const { return static_cast<B>(this->operator A()); }

	/// Permits uninitialized LERep, for parity with the associated A.
	constexpr LERep() {}

	/// Construct a value from a byte buffer.
	constexpr explicit LERep(std::span<const std::byte, sizeof(A)> arr) {
		std::memcpy(this, arr.data(), sizeof(A));
	}
};

/// Read a given value of type A from a pointer into arbitrary data presumed to hold a little-endian representation of A.
/// Subject to the usual litany of cautions about
///
///     *reinterpret_cast<T *>(p)
///
/// , such invocations, where spuriously dependent upon host byte order, may be replaced with:
///
///     deref_as_le<T>(p)
template<sourcepp::math::Arithmetic A>
[[nodiscard]] constexpr A deref_as_le(const void *p) {
	return LERep<A>(std::span<const std::byte, sizeof(A)>(static_cast<const std::byte *>(p), sizeof(A))).operator A();
}

#define SOURCEPP_LEREP_DEFINE(N, V)\
	using N##le = LERep<V>
#define SOURCEPP_LEREP_DEFINE_P(PFX, N16, N32, N64) \
	SOURCEPP_LEREP_DEFINE(PFX##16, N16); \
	SOURCEPP_LEREP_DEFINE(PFX##32, N32); \
	SOURCEPP_LEREP_DEFINE(PFX##64, N64)

SOURCEPP_LEREP_DEFINE_P(i,  int16_t,  int32_t,  int64_t);
SOURCEPP_LEREP_DEFINE_P(ui, uint16_t, uint32_t, uint64_t);
SOURCEPP_LEREP_DEFINE_P(f,  half,     float,    double);

#undef SOURCEPP_LEREP_DEFINE_P
#undef SOURCEPP_LEREP_DEFINE

} // namespace sourcepp::bits
