#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/Math.h>

namespace vtfpp {

class SHT {
public:
	struct Sequence {
		struct Frame {
			struct Bounds {
				float x1;
				float y1;
				float x2;
				float y2;
			};

			float duration;
			// A sprite frame can reference 1 or 4 images, depending on the version
			std::array<Bounds, 4> bounds;

			void setAllBounds(Bounds newBounds) {
				this->bounds[0] = newBounds;
				this->bounds[1] = newBounds;
				this->bounds[2] = newBounds;
				this->bounds[3] = newBounds;
			}
		};

		uint32_t id;
		bool loop;
		std::vector<Frame> frames;
		float durationTotal;
	};

	SHT();

	explicit SHT(std::span<const std::byte> shtData);

	explicit SHT(const std::string& shtPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getVersion() const;

	void setVersion(uint32_t v);

	[[nodiscard]] const std::vector<Sequence>& getSequences() const;

	[[nodiscard]] std::vector<Sequence>& getSequences();

	[[nodiscard]] const Sequence* getSequenceFromID(uint32_t id) const;

	[[nodiscard]] Sequence* getSequenceFromID(uint32_t id);

	[[nodiscard]] uint8_t getFrameBoundsCount() const;

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::string& shtPath) const; // NOLINT(*-use-nodiscard)

protected:
	bool opened;

	uint32_t version = 0;
	std::vector<Sequence> sequences;
};

} // namespace vtfpp
