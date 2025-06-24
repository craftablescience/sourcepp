#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

namespace vtfpp {

class HOT {
public:
	struct Rect {
		enum Flags : uint8_t {
			FLAG_NONE              = 0,
			FLAG_RANDOM_ROTATION   = 1 << 0,
			FLAG_RANDOM_REFLECTION = 1 << 1,
			FLAG_IS_ALTERNATE      = 1 << 2,
		} flags;
		uint16_t x1;
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
	};

	HOT();

	explicit HOT(std::span<const std::byte> hotData);

	explicit HOT(const std::string& hotPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint8_t getVersion() const;

	void setVersion(uint8_t v);

	[[nodiscard]] uint8_t getFlags() const;

	void setFlags(uint8_t f);

	[[nodiscard]] const std::vector<Rect>& getRects() const;

	[[nodiscard]] std::vector<Rect>& getRects();

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::string& hotPath) const; // NOLINT(*-use-nodiscard)

protected:
	bool opened;

	uint8_t version = 1;
	uint8_t flags = 0;
	std::vector<Rect> rects;
};
SOURCEPP_BITFLAGS_ENUM(HOT::Rect::Flags)

} // namespace vtfpp
