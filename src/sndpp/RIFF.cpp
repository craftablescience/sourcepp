#include <sndpp/RIFF.h>

#include <algorithm>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sndpp;
using namespace sourcepp;

RIFF::RIFF(uint32_t type, uint32_t signature)
		: signature{signature}
		, type{type} {
	if (this->signature != RIFF_SIGNATURE && this->signature != RIFX_SIGNATURE && this->signature != FFIR_SIGNATURE) {
		this->signature = 0;
	}
}

RIFF::RIFF(std::vector<std::byte>&& riffData, uint32_t type_)
		: type{type_}
		, data{std::move(riffData)} {
	BufferStreamReadOnly stream{this->data};

	stream >> this->signature;
	if (signature == RIFX_SIGNATURE || signature == FFIR_SIGNATURE) {
		stream.set_big_endian(true);
	} else if (signature != RIFF_SIGNATURE) {
		this->signature = 0;
		return;
	}

	const auto fileLength = stream.read<uint32_t>();
	if (stream.read<uint32_t>() != this->type) {
		this->signature = 0;
		return;
	}

	while (stream.tell() < fileLength) {
		auto& [chunkType, chunkData] = this->chunks.emplace_back();
		stream.read(chunkType).read(chunkData, stream.read<uint32_t>());
		if (stream.tell() % 2) {
			stream.skip();
		}
	}
}

RIFF::RIFF(std::span<const std::byte> riffData, uint32_t type_)
		: RIFF{std::vector<std::byte>{riffData.begin(), riffData.end()}, type_} {}

RIFF::RIFF(const std::filesystem::path& riffPath, uint32_t type_)
		: RIFF{fs::readFileBuffer(riffPath), type_} {}

RIFF::operator bool() const {
	return this->signature;
}

uint32_t RIFF::getSignature() const {
	return this->signature;
}

bool RIFF::isBigEndian() const {
	return this->signature == RIFX_SIGNATURE || this->signature == FFIR_SIGNATURE;
}

const std::vector<std::pair<uint32_t, std::span<std::byte>>>& RIFF::getChunks() const {
	return this->chunks;
}

bool RIFF::hasChunk(uint32_t chunkType) const {
	return std::ranges::find_if(this->chunks, [chunkType](const std::pair<uint32_t, std::span<std::byte>>& chunk) {
		return chunk.first == chunkType;
	}) != this->chunks.end();
}

std::vector<std::byte> RIFF::getFirstChunk(uint32_t chunkType) const {
	if (const auto it = std::ranges::find_if(this->chunks, [chunkType](const std::pair<uint32_t, std::span<std::byte>>& chunk) {
		return chunk.first == chunkType;
	}); it != this->chunks.end()) {
		return {it->second.begin(), it->second.end()};
	}
	return {};
}

bool RIFF::hasNthChunk(uint32_t chunkType, uint32_t n) const {
	uint32_t i = 0;
	return std::ranges::find_if(this->chunks, [chunkType, n, &i](const std::pair<uint32_t, std::span<std::byte>>& chunk) {
		return chunk.first == chunkType && i++ == n;
	}) != this->chunks.end();
}

std::vector<std::byte> RIFF::getNthChunk(uint32_t chunkType, uint32_t n) const {
	uint32_t i = 0;
	if (const auto it = std::ranges::find_if(this->chunks, [chunkType, n, &i](const std::pair<uint32_t, std::span<std::byte>>& chunk) {
		return chunk.first == chunkType && i++ == n;
	}); it != this->chunks.end()) {
		return {it->second.begin(), it->second.end()};
	}
	return {};
}
