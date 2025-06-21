#include <vtfpp/SHT.h>

#include <BufferStream.h>

using namespace sourcepp;
using namespace vtfpp;

SHT::SHT()
		: opened(true)
		, version(0) {}

SHT::SHT(std::span<const std::byte> shtData) {
    BufferStreamReadOnly stream{shtData.data(), shtData.size()};

	stream >> this->version;

	this->sequences.resize(stream.read<uint32_t>());
    for (auto& sequence : this->sequences) {
		stream >> sequence.id;
        sequence.loop = stream.read<uint32_t>();
        sequence.frames.resize(stream.read<uint32_t>());
        stream >> sequence.durationTotal;

        for (auto& frame : sequence.frames) {
            frame.duration = stream.read<float>();
            for (uint8_t i = 0; i < this->getFrameBoundsCount(); i++) {
                auto& bounds = frame.bounds[i];
	            stream >> bounds.x1 >> bounds.y1 >> bounds.x2 >> bounds.y2;
            }
        }
    }

    this->opened = true;
}

SHT::SHT(const std::string& shtPath)
		: SHT(fs::readFileBuffer(shtPath)) {}

SHT::operator bool() const {
    return this->opened;
}

uint32_t SHT::getVersion() const {
	return this->version;
}

void SHT::setVersion(uint32_t v) {
	if (v != 0 && v != 1) {
		return;
	}
	this->version = v;
}

const std::vector<SHT::Sequence>& SHT::getSequences() const {
	return this->sequences;
}

std::vector<SHT::Sequence>& SHT::getSequences() {
	return this->sequences;
}

const SHT::Sequence* SHT::getSequenceFromID(uint32_t id) const {
	if (auto pos = std::find_if(this->sequences.begin(), this->sequences.end(), [id](const Sequence& sequence) {
		return sequence.id == id;
	}); pos != this->sequences.end()) {
		return &*pos;
	}
	return nullptr;
}

SHT::Sequence* SHT::getSequenceFromID(uint32_t id) {
	if (auto pos = std::find_if(this->sequences.begin(), this->sequences.end(), [id](const Sequence& sequence) {
			return sequence.id == id;
		}); pos != this->sequences.end()) {
		return &*pos;
	}
	return nullptr;
}

uint8_t SHT::getFrameBoundsCount() const {
	return (this->version > 0) ? 4 : 1;
}

std::vector<std::byte> SHT::bake() const {
    if (!this->opened) {
	    return {};
    }

	std::vector<std::byte> sheetData;
    BufferStream stream{sheetData};

    stream
		.write<uint32_t>(this->version)
        .write<uint32_t>(this->sequences.size());

    for (const auto& sequence : this->sequences) {
        stream
			.write<uint32_t>(sequence.id)
            .write<uint32_t>(sequence.loop)
            .write<uint32_t>(sequence.frames.size())
            .write<float>(sequence.durationTotal);

        for (const auto& frame : sequence.frames) {
            stream.write<float>(frame.duration);

            for (uint8_t i = 0; i < this->getFrameBoundsCount(); i++) {
	            auto& bounds = frame.bounds[i];
                stream << bounds.x1 << bounds.y1 << bounds.x2 << bounds.y2;
            }
        }
    }

    sheetData.resize(stream.tell());
    return sheetData;
}

bool SHT::bake(const std::string& shtPath) const {
    return fs::writeFileBuffer(shtPath, this->bake());
}
