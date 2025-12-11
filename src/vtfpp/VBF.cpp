#include <vtfpp/VBF.h>

#include <vector>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vtfpp;

VBF::VBF(std::span<const std::byte> vbfData) {
	BufferStreamReadOnly stream{vbfData.data(), vbfData.size()};

	if (stream.read<uint32_t>() != VBF_SIGNATURE) {
		return;
	}

	stream >> this->version;
	if (this->version != 3) {
		this->version = 0;
		return;
	}

	stream >> this->pageSize[0] >> this->pageSize[1];
	stream >> this->maxGlyphSize[0] >> this->maxGlyphSize[1];
	stream >> this->flags >> this->ascent;

	const auto glyphCount = stream.read<uint16_t>();
	const auto glyphLUT = stream.read<std::array<uint8_t, 256>>();

	std::vector<Glyph> glyphList;
	glyphList.reserve(glyphCount);
	for (uint16_t i = 0; i < glyphCount; i++) {
		auto& [position, size, abcSpacing] = glyphList.emplace_back();
		stream >> position[0] >> position[1];
		stream >> size[0] >> size[1];
		stream >> abcSpacing[0] >> abcSpacing[1] >> abcSpacing[2];
	}
	for (int i = 0; i < this->glyphs.size(); i++) {
		this->glyphs[i] = glyphList[glyphLUT[i]];
	}
}

VBF::VBF(const std::filesystem::path& vbfPath)
		: VBF(fs::readFileBuffer(vbfPath)) {}

VBF::operator bool() const {
	return this->version == 3;
}

math::Vec2ui16 VBF::getPageSize() const {
	return this->pageSize;
}

math::Vec2ui16 VBF::getMaxGlyphSize() const {
	return this->maxGlyphSize;
}

VBF::Flags VBF::getFlags() const {
	return this->flags;
}

uint16_t VBF::getAscent() const {
	return this->ascent;
}

const std::array<VBF::Glyph, 256>& VBF::getGlyphs() const {
	return this->glyphs;
}
