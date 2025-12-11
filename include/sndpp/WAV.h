#pragma once

#include <array>
#include <optional>

#include <BufferStream.h>

#include "RIFF.h"

namespace sndpp {

constexpr auto WAV_TYPE = sourcepp::parser::binary::makeFourCC("WAVE");

class WAV : public RIFF {
public:
	enum ChunkType : uint32_t {
		CHUNK_FMT  = sourcepp::parser::binary::makeFourCC("fmt "), // Track details
		CHUNK_DATA = sourcepp::parser::binary::makeFourCC("data"), // The audio data
		CHUNK_CUE  = sourcepp::parser::binary::makeFourCC("cue "), // Cue points / markers
		CHUNK_PLST = sourcepp::parser::binary::makeFourCC("plst"), // Playlist for cue points
		CHUNK_FACT = sourcepp::parser::binary::makeFourCC("fact"), // More track details
		CHUNK_SMPL = sourcepp::parser::binary::makeFourCC("smpl"), // Sampler
		CHUNK_INST = sourcepp::parser::binary::makeFourCC("inst"), // Instrument
		CHUNK_CSET = sourcepp::parser::binary::makeFourCC("CSET"), // Character set
		CHUNK_MD5  = sourcepp::parser::binary::makeFourCC("MD5 "), // Checksums
		CHUNK_PAD  = sourcepp::parser::binary::makeFourCC("PAD "), // Useless
		CHUNK_JUNK = sourcepp::parser::binary::makeFourCC("JUNK"), // Useless
		CHUNK_FLLR = sourcepp::parser::binary::makeFourCC("FLLR"), // Useless

		//CHUNK_LIST = sourcepp::parser::binary::makeFourCC("LIST"), // Metadata
		//CHUNK_LIST_WAVL = sourcepp::parser::binary::makeFourCC("WAVL"), // Wave list

		//CHUNK_LIST_INFO = sourcepp::parser::binary::makeFourCC("INFO"), // Overall metadata
		//CHUNK_LIST_INFO_IARL = sourcepp::parser::binary::makeFourCC("IARL"), // Archive location
		//CHUNK_LIST_INFO_IGNR = sourcepp::parser::binary::makeFourCC("IGNR"), // Genre
		//CHUNK_LIST_INFO_IKEY = sourcepp::parser::binary::makeFourCC("IKEY"), // Keywords
		//CHUNK_LIST_INFO_ILGT = sourcepp::parser::binary::makeFourCC("ILGT"), // Lightness
		//CHUNK_LIST_INFO_IMED = sourcepp::parser::binary::makeFourCC("IMED"), // Medium
		//CHUNK_LIST_INFO_INAM = sourcepp::parser::binary::makeFourCC("INAM"), // Title
		//CHUNK_LIST_INFO_IPLT = sourcepp::parser::binary::makeFourCC("IPLT"), // Palette setting
		//CHUNK_LIST_INFO_IPRD = sourcepp::parser::binary::makeFourCC("IPRD"), // Product
		//CHUNK_LIST_INFO_ISBJ = sourcepp::parser::binary::makeFourCC("ISBJ"), // Subject
		//CHUNK_LIST_INFO_ISFT = sourcepp::parser::binary::makeFourCC("ISFT"), // Software used
		//CHUNK_LIST_INFO_ISHP = sourcepp::parser::binary::makeFourCC("ISHP"), // Sharpness
		//CHUNK_LIST_INFO_IART = sourcepp::parser::binary::makeFourCC("IART"), // Artist
		//CHUNK_LIST_INFO_ISRC = sourcepp::parser::binary::makeFourCC("ISRC"), // Source (file supplier)
		//CHUNK_LIST_INFO_ISRF = sourcepp::parser::binary::makeFourCC("ISRF"), // Source form
		//CHUNK_LIST_INFO_ITCH = sourcepp::parser::binary::makeFourCC("ITCH"), // Technician
		//CHUNK_LIST_INFO_ICMS = sourcepp::parser::binary::makeFourCC("ICMS"), // Commissioner
		//CHUNK_LIST_INFO_ICMT = sourcepp::parser::binary::makeFourCC("ICMT"), // Comments
		//CHUNK_LIST_INFO_ICOP = sourcepp::parser::binary::makeFourCC("ICOP"), // Copyright
		//CHUNK_LIST_INFO_ICRP = sourcepp::parser::binary::makeFourCC("ICRP"), // Crop
		//CHUNK_LIST_INFO_IDPI = sourcepp::parser::binary::makeFourCC("IDPI"), // Dots per inch
		//CHUNK_LIST_INFO_IENG = sourcepp::parser::binary::makeFourCC("IENG"), // Engineer
		//CHUNK_LIST_INFO_ISMP = sourcepp::parser::binary::makeFourCC("ISMP"), // Digitization start time (SMPTE time code)
		//CHUNK_LIST_INFO_IDIT = sourcepp::parser::binary::makeFourCC("IDIT"), // Digitization start time (26 characters, formatted like "Wed Jan 02 02:03:55 1990\n\0")
		//CHUNK_LIST_INFO_ICRD = sourcepp::parser::binary::makeFourCC("ICRD"), // Creation date (YYYY, YYYY-MM, YYYY-MM-DD)
		//CHUNK_LIST_INFO_ITRK = sourcepp::parser::binary::makeFourCC("ITRK"), // Track number

		//CHUNK_LIST_ADTL = sourcepp::parser::binary::makeFourCC("adtl"), // Cue points metadata
		//CHUNK_LIST_ADTL_LABL = sourcepp::parser::binary::makeFourCC("labl"), // Cue point label
		//CHUNK_LIST_ADTL_NOTE = sourcepp::parser::binary::makeFourCC("note"), // Cue point note
		//CHUNK_LIST_ADTL_LTXT = sourcepp::parser::binary::makeFourCC("ltxt"), // Section label
	};

	struct ChunkFMT {
		uint16_t format;
		uint16_t channels;
		uint32_t samplesPerSecond;
		uint32_t averageBytesPerSecond;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		std::vector<std::byte> extraCompressionInfo;
	};

	struct ChunkCUE {
		struct CuePoint {
			uint32_t id;
			int32_t position;
			ChunkType chunk;
			uint32_t chunkStart;
			uint32_t blockStart;
			uint32_t sampleOffset;
		};

		std::vector<CuePoint> cuePoints;
	};

	struct ChunkPLST {
		struct Segment {
			uint32_t id;
			uint32_t length;
			uint32_t repeats;
		};

		std::vector<Segment> segments;
	};

	struct ChunkFACT {
		uint32_t samples;
		std::vector<std::byte> extraCompressionInfo;
	};

	struct ChunkSMPL {
		struct SampleLoop {
			uint32_t id;
			uint32_t type;
			uint32_t start;
			uint32_t end;
			uint32_t fraction;
			uint32_t playCount;
		};

		uint32_t manufacturer;
		uint32_t product;
		uint32_t samplePeriod;
		uint32_t midiUnityNote;
		uint32_t midiPitchFraction;
		uint32_t smpteFormat;
		uint32_t smpteOffset;
		uint32_t sampleLoops;
		uint32_t samplerData;
		std::vector<SampleLoop> loops;
	};

	struct ChunkINST {
		uint8_t unshiftedNote;
		int8_t fineTune;
		int8_t gain;
		uint8_t lowNote;
		uint8_t highNote;
		uint8_t lowVelocity;
		uint8_t highVelocity;
	};

	struct ChunkCSET {
		uint16_t codePage;
		uint16_t countryCode;
		uint16_t language;
		uint16_t dialect;
	};

	struct ChunkMD5 {
		std::array<uint8_t, 16> md5;
	};

	//struct ChunkLISTINFO {
	//	struct META {
	//		ChunkType infoType;
	//		std::string metadata;
	//	};
	//	std::vector<META> metadata;
	//};

	//struct ChunkLISTADTL {
	//	struct LTXT {
	//		uint32_t sampleLength;
	//		uint32_t purpose;
	//		uint16_t country;
	//		uint16_t language;
	//		uint16_t dialect;
	//		uint16_t codePage;
	//		std::string label;
	//	};
	//	std::vector<std::string> labels;
	//	std::vector<std::string> notes;
	//	std::vector<LTXT> labelTexts;
	//};

	explicit WAV(uint32_t signature = RIFF_SIGNATURE);

	explicit WAV(std::vector<std::byte>&& wavData);

	explicit WAV(std::span<const std::byte> wavData);

	explicit WAV(const std::filesystem::path& wavPath);

	template<ChunkType Type>
	[[nodiscard]] auto getFirstWAVChunk() const {
		return this->getNthWAVChunk<Type>(0);
	}

	template<ChunkType Type>
	[[nodiscard]] auto getNthWAVChunk(uint32_t n) const {
		auto chunkData = this->getNthChunk(Type, n);
		BufferStreamReadOnly stream{chunkData};
		if constexpr (Type == CHUNK_FMT) {
			std::optional<ChunkFMT> chunk = std::nullopt;
			if (chunkData.size() >= sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4) {
				chunk = ChunkFMT{};
				const auto fmtExtraCompressionInfoLength = stream.size() - 16; // size of the standard FMT header
				stream >> chunk->format >> chunk->channels >> chunk->samplesPerSecond >> chunk->averageBytesPerSecond >> chunk->blockAlign >> chunk->bitsPerSample;
				if (fmtExtraCompressionInfoLength) {
					chunk->extraCompressionInfo = stream.read_bytes(fmtExtraCompressionInfoLength);
				}
			}
			return chunk;
		} else if constexpr (Type == CHUNK_CUE) {
			std::optional<ChunkCUE> chunk = std::nullopt;
			if (stream.size() >= sizeof(uint32_t) && stream.size() - sizeof(uint32_t) % sizeof(ChunkCUE::CuePoint) == 0) {
				chunk = ChunkCUE{};
				const auto cueNumPoints = stream.read<uint32_t>();
				for (uint32_t i = 0; i < cueNumPoints; i++) {
					ChunkCUE::CuePoint point{};
					stream >> point.id >> point.position >> point.chunk >> point.chunkStart >> point.blockStart >> point.sampleOffset;
					chunk->cuePoints.push_back(point);
				}
			}
			return chunk;
		} else if constexpr (Type == CHUNK_PLST) {
			std::optional<ChunkPLST> chunk = std::nullopt;
			if (stream.size() >= sizeof(uint32_t) && (stream.size() - sizeof(uint32_t)) % sizeof(ChunkPLST::Segment) == 0) {
				chunk = ChunkPLST{};
				const auto plstNumSegments = stream.read<uint32_t>();
				for (uint32_t i = 0; i < plstNumSegments; i++) {
					ChunkPLST::Segment segment{};
					stream >> segment.id >> segment.length >> segment.repeats;
					chunk->segments.push_back(segment);
				}
			}
			return chunk;
		} else if constexpr (Type == CHUNK_FACT) {
			std::optional<ChunkFACT> chunk = std::nullopt;
			if (stream.size() >= sizeof(uint32_t)) {
				chunk = ChunkFACT{};
				const auto factExtraCompressionInfoLength = stream.size() - sizeof(uint32_t);
				stream >> chunk->samples;
				if (factExtraCompressionInfoLength) {
					chunk->extraCompressionInfo = stream.read_bytes(factExtraCompressionInfoLength);
				}
			}
			return chunk;
		} else if constexpr (Type == CHUNK_SMPL) {
			std::optional<ChunkSMPL> chunk = std::nullopt;
			if (stream.size() >= sizeof(uint32_t) * 10) {
				chunk = ChunkSMPL{};
				stream >> chunk->manufacturer >> chunk->product >> chunk->samplePeriod >> chunk->midiUnityNote >> chunk->midiPitchFraction >> chunk->smpteFormat >> chunk->smpteOffset >> chunk->sampleLoops >> chunk->samplerData;
				const auto smplNumLoops = stream.read<uint32_t>();
				for (uint32_t i = 0; i < smplNumLoops; i++) {
					// ReSharper disable once CppUseStructuredBinding
					auto& sampleLoop = chunk->loops.emplace_back();
					stream >> sampleLoop.id >> sampleLoop.type >> sampleLoop.start >> sampleLoop.end >> sampleLoop.fraction >> sampleLoop.playCount;
				}
			}
			return chunk;
		} else if constexpr (Type == CHUNK_INST) {
			std::optional<ChunkINST> chunk = std::nullopt;
			if (stream.size() == sizeof(uint8_t) * 7) {
				chunk = ChunkINST{};
				stream >> chunk->unshiftedNote >> chunk->fineTune >> chunk->gain >> chunk->lowNote >> chunk->highNote >> chunk->lowVelocity >> chunk->highVelocity;
			}
			return chunk;
		} else if constexpr (Type == CHUNK_CSET) {
			std::optional<ChunkCSET> chunk = std::nullopt;
			if (stream.size() == sizeof(uint16_t) * 4) {
				chunk = ChunkCSET{};
				stream >> chunk->codePage >> chunk->countryCode >> chunk->language >> chunk->dialect;
			}
			return chunk;
		} else if constexpr (Type == CHUNK_MD5) {
			std::optional<ChunkMD5> chunk = std::nullopt;
			if (stream.size() == sizeof(uint8_t) * 16) {
				chunk = ChunkMD5{};
				stream >> chunk->md5;
			}
			return chunk;
		} else {
			return this->hasNthChunk(Type, n) ? std::optional{std::move(chunkData)} : std::nullopt;
		}
	}
};

} // namespace sndpp
