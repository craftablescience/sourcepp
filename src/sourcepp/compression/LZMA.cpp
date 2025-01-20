#include <sourcepp/compression/LZMA.h>

#include <BufferStream.h>
#include <lzma.h>
#include <sourcepp/parser/Binary.h>

using namespace sourcepp;

namespace {

constexpr auto LZMA_VALVE_SIGNATURE = parser::binary::makeFourCC("LZMA");

} // namespace

std::optional<std::vector<std::byte>> compression::compressValveLZMA(std::span<const std::byte> data, uint8_t compressLevel) {
	// Preallocate extra 4 bytes for Valve LZMA header signature
	std::vector<std::byte> compressedData(sizeof(uint32_t));
	std::array<std::byte, 8192> compressedChunk{};

	lzma_stream stream{
		.next_in = reinterpret_cast<const uint8_t*>(data.data()),
		.avail_in = data.size(),
		.next_out = reinterpret_cast<uint8_t*>(compressedChunk.data()),
		.avail_out = compressedChunk.size(),
	};

	lzma_options_lzma options{};
	lzma_lzma_preset(&options, std::clamp<uint8_t>(compressLevel, 0, 9));

	if (lzma_alone_encoder(&stream, &options) != LZMA_OK) {
		lzma_end(&stream);
		return std::nullopt;
	}

	lzma_ret ret;
	do {
		stream.next_out = reinterpret_cast<uint8_t*>(compressedChunk.data());
		stream.avail_out = compressedChunk.size();

		ret = lzma_code(&stream, LZMA_RUN);
		compressedData.insert(compressedData.end(), compressedChunk.begin(), compressedChunk.begin() + compressedChunk.size() - static_cast<std::ptrdiff_t>(stream.avail_out));
	} while (ret == LZMA_OK);

	if (auto code = lzma_code(&stream, LZMA_FINISH); code != LZMA_OK && code != LZMA_STREAM_END) {
		lzma_end(&stream);
		return std::nullopt;
	}
	lzma_end(&stream);

	{
		// Switch out normal header with Valve one
		BufferStream compressedStream{compressedData};
		compressedStream << LZMA_VALVE_SIGNATURE;
		const auto properties = compressedStream.read<std::array<uint8_t, 5>>();
		compressedStream
			.seek_u(sizeof(uint32_t))
			.write<uint32_t>(data.size())
			.write(compressedData.size() - (sizeof(uint32_t) * 3) + (sizeof(uint8_t) * 5))
			.write(properties);
	}
	return compressedData;
}

std::optional<std::vector<std::byte>> compression::decompressValveLZMA(std::span<const std::byte> data) {
	std::vector<std::byte> compressedData{data.begin(), data.end()};
	std::vector<std::byte> uncompressedData;
	{
		// Switch out Valve header with normal one
		BufferStreamReadOnly in{data.data(), data.size()};
		if (in.read<uint32_t>() != LZMA_VALVE_SIGNATURE) {
			return std::nullopt;
		}
		const auto uncompressedLength = in.read<uint32_t>();
		in.skip<uint32_t>();
		const auto properties = in.read<std::array<uint8_t, 5>>();
		BufferStream out{compressedData};
		out
			.write(properties)
			.write<uint64_t>(uncompressedLength);
		uncompressedData.resize(uncompressedLength);
	}

	lzma_stream stream{
		.next_in = reinterpret_cast<const uint8_t*>(compressedData.data()),
		.avail_in = compressedData.size(),
		.next_out = reinterpret_cast<uint8_t*>(uncompressedData.data()),
		.avail_out = uncompressedData.size(),
	};

	if (lzma_alone_decoder(&stream, UINT64_MAX) != LZMA_OK) {
		lzma_end(&stream);
		return std::nullopt;
	}
	if (auto ret = lzma_code(&stream, LZMA_RUN); ret != LZMA_OK && ret != LZMA_STREAM_END) {
		lzma_end(&stream);
		return std::nullopt;
	}
	if (auto ret = lzma_code(&stream, LZMA_FINISH); ret != LZMA_OK && ret != LZMA_STREAM_END) {
		lzma_end(&stream);
		return std::nullopt;
	}
	lzma_end(&stream);

	return uncompressedData;
}
