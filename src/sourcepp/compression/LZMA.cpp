#include <sourcepp/compression/LZMA.h>

#include <BufferStream.h>
#include <lzma.h>

using namespace sourcepp;

std::optional<std::vector<std::byte>> compression::compressValveLZMA(std::span<const std::byte> data, uint8_t compressLevel) {
	// Shift over 4 bytes for Valve LZMA header signature
	std::vector<std::byte> compressedData(sizeof(uint32_t) + data.size() * 2);

	lzma_options_lzma options{};
	if (lzma_lzma_preset(&options, std::clamp<uint8_t>(compressLevel, 0, 9))) {
		return std::nullopt;
	}

	lzma_stream stream = LZMA_STREAM_INIT;

	if (lzma_alone_encoder(&stream, &options) != LZMA_OK) {
		lzma_end(&stream);
		return std::nullopt;
	}

	stream.next_in = reinterpret_cast<const uint8_t*>(data.data());
	stream.avail_in = data.size();
	stream.next_out = reinterpret_cast<uint8_t*>(compressedData.data() + sizeof(uint32_t));
	stream.avail_out = compressedData.size() - sizeof(uint32_t);

	lzma_ret ret = lzma_code(&stream, LZMA_FINISH);
	lzma_end(&stream);
	if (ret != LZMA_STREAM_END) {
		return std::nullopt;
	}

	// Switch out normal header with Valve one
	BufferStream compressedStream{compressedData};
	compressedStream.seek(0).write(VALVE_LZMA_SIGNATURE);
	const auto properties = compressedStream.read<uint8_t>();
	const auto dictionarySize = compressedStream.read<uint32_t>();
	compressedStream
		.seek_u(sizeof(uint32_t))
		.write<uint32_t>(data.size())
		.write(compressedData.size() - (sizeof(uint32_t) * 3) + (sizeof(uint8_t) * 5))
		.write(properties)
		.write(dictionarySize);

	compressedData.resize(stream.total_out + sizeof(uint32_t));
	return compressedData;
}

std::optional<std::vector<std::byte>> compression::decompressValveLZMA(std::span<const std::byte> data) {
	if (data.size() < 18) {
		// Valve LZMA header length + 1 null byte
		return std::nullopt;
	}

	std::vector<std::byte> compressedData{data.begin() + sizeof(uint32_t), data.end()};
	std::vector<std::byte> uncompressedData;
	{
		// Switch out Valve header with normal one
		BufferStreamReadOnly in{data};
		if (in.read<uint32_t>() != VALVE_LZMA_SIGNATURE) {
			return std::nullopt;
		}
		const uint64_t uncompressedLength = in.read<uint32_t>();
		in.skip<uint32_t>(); // compressed length is easily inferred
		const auto properties = in.read<uint8_t>();
		const auto dictionarySize = in.read<uint32_t>();
		BufferStream out{compressedData.data(), compressedData.size()};
		out
			.write(properties)
			.write(dictionarySize)
			.write(uncompressedLength);

		uncompressedData.resize(uncompressedLength);
	}

	lzma_stream stream = LZMA_STREAM_INIT;
	stream.next_in = reinterpret_cast<const uint8_t*>(compressedData.data());
	stream.avail_in = compressedData.size();
	stream.next_out = reinterpret_cast<uint8_t*>(uncompressedData.data());
	stream.avail_out = uncompressedData.size();

	if (lzma_alone_decoder(&stream, UINT64_MAX) != LZMA_OK) {
		lzma_end(&stream);
		return std::nullopt;
	}
	while (true) {
		const auto ret = lzma_code(&stream, LZMA_RUN);
		if (ret == LZMA_STREAM_END || stream.avail_out == 0) {
			break;
		}
		if (ret != LZMA_OK) {
			lzma_end(&stream);
			return std::nullopt;
		}
	}
	if (const auto ret = lzma_code(&stream, LZMA_FINISH); ret != LZMA_OK && ret != LZMA_STREAM_END) {
		lzma_end(&stream);
		return std::nullopt;
	}
	lzma_end(&stream);

	return uncompressedData;
}
