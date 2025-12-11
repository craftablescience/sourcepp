#pragma once

#include <array>
#include <filesystem>

#include <sourcepp/parser/Binary.h>
#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

namespace vtfpp {

constexpr uint32_t VBF_SIGNATURE = sourcepp::parser::binary::makeFourCC("VFNT");

class VBF {
public:
	enum Flags : uint16_t {
		FLAG_NONE        = 0,
		FLAG_BOLD        = 1 << 0,
		FLAG_ITALIC      = 1 << 1,
		FLAG_OUTLINE     = 1 << 2,
		FLAG_DROP_SHADOW = 1 << 3,
		FLAG_BLUR        = 1 << 4,
		FLAG_SCANLINE    = 1 << 5,
		FLAG_ANTIALIASED = 1 << 6,
		FLAG_CUSTOM      = 1 << 7,
	};

	struct Glyph {
		sourcepp::math::Vec2ui16 position;
		sourcepp::math::Vec2ui16 size;
		sourcepp::math::Vec3i16 abcSpacing;
	};

	explicit VBF(std::span<const std::byte> vbfData);

	explicit VBF(const std::filesystem::path& vbfPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] sourcepp::math::Vec2ui16 getPageSize() const;

	[[nodiscard]] sourcepp::math::Vec2ui16 getMaxGlyphSize() const;

	[[nodiscard]] Flags getFlags() const;

	[[nodiscard]] uint16_t getAscent() const;

	[[nodiscard]] const std::array<Glyph, 256>& getGlyphs() const;

protected:
	uint32_t version = 0;
	sourcepp::math::Vec2ui16 pageSize{};
	sourcepp::math::Vec2ui16 maxGlyphSize{};
	Flags flags = FLAG_NONE;
	uint16_t ascent = 0;
	std::array<Glyph, 256> glyphs{};
};
SOURCEPP_BITFLAGS_ENUM(VBF::Flags)

} // namespace vtfpp
