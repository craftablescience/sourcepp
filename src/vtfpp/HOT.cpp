#include <vtfpp/HOT.h>

#include <BufferStream.h>

using namespace sourcepp;
using namespace vtfpp;

HOT::HOT()
		: opened(true) {}

HOT::HOT(std::span<const std::byte> hotData) {
    BufferStreamReadOnly stream{hotData};

	stream >> this->version >> this->flags;
	if (this->version != 1) {
		return;
	}

	this->rects.resize(stream.read<uint16_t>());
    for (auto& [flags_, x1, y1, x2, y2] : this->rects) {
		stream >> flags_ >> x1 >> y1 >> x2 >> y2;
    }

    this->opened = true;
}

HOT::HOT(const std::string& hotPath)
		: HOT(fs::readFileBuffer(hotPath)) {}

HOT::operator bool() const {
    return this->opened;
}

uint8_t HOT::getVersion() const {
	return this->version;
}

void HOT::setVersion(uint8_t v) {
	if (v != 1) {
		return;
	}
	this->version = v;
}

uint8_t HOT::getFlags() const {
	return this->flags;
}

void HOT::setFlags(uint8_t f) {
	this->flags = f;
}

const std::vector<HOT::Rect>& HOT::getRects() const {
	return this->rects;
}

std::vector<HOT::Rect>& HOT::getRects() {
	return this->rects;
}

std::vector<std::byte> HOT::bake() const {
    if (!this->opened) {
	    return {};
    }

	std::vector<std::byte> hotspotData;
    BufferStream stream{hotspotData};

    stream << this->version << this->flags;
	stream.write<uint16_t>(this->rects.size());

    for (const auto& [flags_, x1, y1, x2, y2] : this->rects) {
        stream << flags_ << x1 << y1 << x2 << y2;
    }

    hotspotData.resize(stream.tell());
    return hotspotData;
}

bool HOT::bake(const std::string& hotPath) const {
    return fs::writeFileBuffer(hotPath, this->bake());
}
