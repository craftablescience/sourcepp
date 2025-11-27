#include <sndpp/WAV.h>

using namespace sndpp;

WAV::WAV(uint32_t signature)
		: RIFF{WAV_TYPE, signature} {}

WAV::WAV(std::vector<std::byte>&& wavData)
		: RIFF{wavData, WAV_TYPE} {
	// FMT chunk and DATA chunk are required
	if (!this->hasChunk(CHUNK_FMT) || !this->hasChunk(CHUNK_DATA)) {
		this->signature = 0;
	}
}

WAV::WAV(std::span<const std::byte> wavData)
		: RIFF{wavData, WAV_TYPE} {}

WAV::WAV(const std::string& wavPath)
		: RIFF{wavPath, WAV_TYPE} {}
