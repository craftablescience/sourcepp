#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace sndpp {

constexpr auto RIFF_SIGNATURE = sourcepp::parser::binary::makeFourCC("RIFF");
constexpr auto RIFX_SIGNATURE = sourcepp::parser::binary::makeFourCC("RIFX");
constexpr auto FFIR_SIGNATURE = sourcepp::parser::binary::makeFourCC("FFIR");

class WAV {
public:
	static constexpr auto RIFF_TYPE = sourcepp::parser::binary::makeFourCC("WAVE");

	enum class ChunkType : uint32_t {
		FMT  = sourcepp::parser::binary::makeFourCC("fmt "), // Track details
		DATA = sourcepp::parser::binary::makeFourCC("data"), // The audio data
		CUE  = sourcepp::parser::binary::makeFourCC("cue "), // Cue points / markers
		PLST = sourcepp::parser::binary::makeFourCC("plst"), // Playlist for cue points
		FACT = sourcepp::parser::binary::makeFourCC("fact"), // More track details
		SMPL = sourcepp::parser::binary::makeFourCC("smpl"), // Sampler
		INST = sourcepp::parser::binary::makeFourCC("inst"), // Instrument
		CSET = sourcepp::parser::binary::makeFourCC("CSET"), // Character set
		MD5  = sourcepp::parser::binary::makeFourCC("MD5 "), // Checksums
		JUNK = sourcepp::parser::binary::makeFourCC("JUNK"), // Useless
		PAD  = sourcepp::parser::binary::makeFourCC("PAD "), // Useless
		FLLR = sourcepp::parser::binary::makeFourCC("FLLR"), // Useless
		LIST = sourcepp::parser::binary::makeFourCC("LIST"), // Metadata
	};

	enum class ChunkListType : uint32_t {
		WAVL = sourcepp::parser::binary::makeFourCC("WAVL"), // Wave list
		INFO = sourcepp::parser::binary::makeFourCC("INFO"), // Overall metadata
		ADTL = sourcepp::parser::binary::makeFourCC("adtl"), // Cue points metadata
	};

	enum class ChunkListInfoType : uint32_t {
		IARL = sourcepp::parser::binary::makeFourCC("IARL"), // Archive location
		IGNR = sourcepp::parser::binary::makeFourCC("IGNR"), // Genre
		IKEY = sourcepp::parser::binary::makeFourCC("IKEY"), // Keywords
		ILGT = sourcepp::parser::binary::makeFourCC("ILGT"), // Lightness
		IMED = sourcepp::parser::binary::makeFourCC("IMED"), // Medium
		INAM = sourcepp::parser::binary::makeFourCC("INAM"), // Title
		IPLT = sourcepp::parser::binary::makeFourCC("IPLT"), // Palatte setting
		IPRD = sourcepp::parser::binary::makeFourCC("IPRD"), // Product
		ISBJ = sourcepp::parser::binary::makeFourCC("ISBJ"), // Subject
		ISFT = sourcepp::parser::binary::makeFourCC("ISFT"), // Software used
		ISHP = sourcepp::parser::binary::makeFourCC("ISHP"), // Sharpness
		IART = sourcepp::parser::binary::makeFourCC("IART"), // Artist
		ISRC = sourcepp::parser::binary::makeFourCC("ISRC"), // Source (file supplier)
		ISRF = sourcepp::parser::binary::makeFourCC("ISRF"), // Source form
		ITCH = sourcepp::parser::binary::makeFourCC("ITCH"), // Technician
		ICMS = sourcepp::parser::binary::makeFourCC("ICMS"), // Commissioner
		ICMT = sourcepp::parser::binary::makeFourCC("ICMT"), // Comments
		ICOP = sourcepp::parser::binary::makeFourCC("ICOP"), // Copyright
		ICRP = sourcepp::parser::binary::makeFourCC("ICRP"), // Crop
		IDPI = sourcepp::parser::binary::makeFourCC("IDPI"), // Dots per inch
		IENG = sourcepp::parser::binary::makeFourCC("IENG"), // Engineer
		ISMP = sourcepp::parser::binary::makeFourCC("ISMP"), // Digitization start time (SMPTE time code)
		IDIT = sourcepp::parser::binary::makeFourCC("IDIT"), // Digitization start time (26 characters, formatted like "Wed Jan 02 02:03:55 1990\n\0")
		ICRD = sourcepp::parser::binary::makeFourCC("ICRD"), // Creation date (YYYY, YYYY-MM, YYYY-MM-DD)
		ITRK = sourcepp::parser::binary::makeFourCC("ITRK"), // Track number
	};

	enum class ChunkListADTLType {
		LABL = sourcepp::parser::binary::makeFourCC("labl"), // Cue point label
		NOTE = sourcepp::parser::binary::makeFourCC("note"), // Cue point note
		LTXT = sourcepp::parser::binary::makeFourCC("ltxt"), // Section label
	};

	struct ChunkBase {
		virtual ~ChunkBase() = default;

		ChunkType type = ChunkType::JUNK;
	};

	struct ChunkFMT : ChunkBase {
		uint16_t format;
		uint16_t channels;
		uint32_t samplesPerSecond;
		uint32_t averageBytesPerSecond;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		std::vector<std::byte> extraCompressionInfo;
	};

	struct ChunkDATA : ChunkBase {
		std::vector<std::byte> data;
	};

	struct ChunkCUE : ChunkBase {
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

	struct ChunkPLST : ChunkBase {
		struct Segment {
			uint32_t id;
			uint32_t length;
			uint32_t repeats;
		};

		std::vector<Segment> segments;
	};

	struct ChunkFACT : ChunkBase {
		uint32_t samples;
		std::vector<std::byte> extraCompressionInfo;
	};

	struct ChunkSMPL : ChunkBase {
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

	struct ChunkINST : ChunkBase {
		uint8_t unshiftedNote;
		int8_t fineTune;
		int8_t gain;
		uint8_t lowNote;
		uint8_t highNote;
		uint8_t lowVelocity;
		uint8_t highVelocity;
	};

	struct ChunkCSET : ChunkBase {
		uint16_t codePage;
		uint16_t countryCode;
		uint16_t language;
		uint16_t dialect;
	};

	struct ChunkMD5 : ChunkBase {
		std::array<uint8_t, 16> md5;
	};

	struct ChunkJUNK : ChunkBase {
		std::vector<std::byte> junk;
	};

	struct ChunkPAD : ChunkBase {
		std::vector<std::byte> padding;
	};

	struct ChunkFLLR : ChunkBase {
		std::vector<std::byte> filler;
	};

	struct ChunkLIST : ChunkBase {};

	struct ChunkINFO : ChunkLIST {
		struct META {
			ChunkListInfoType type;
			std::string metadata;
		};

		std::vector<META> metadata;
	};

	struct ChunkADTL : ChunkLIST {
		struct LABL {
			uint32_t id;
			std::string label;
		};

		struct NOTE {
			uint32_t id;
			std::string note;
		};

		struct LTXT {
			uint32_t id;
			uint32_t sampleLength;
			uint32_t purpose;
			uint16_t country;
			uint16_t language;
			uint16_t dialect;
			uint16_t codePage;
			std::string label;
		};

		std::vector<LABL> labels;
		std::vector<NOTE> notes;
		std::vector<LTXT> labelTexts;
	};

	struct ChunkUnknown : ChunkBase {
		std::vector<std::byte> data;
	};

	WAV();

	explicit WAV(std::span<const std::byte> wavData);

	explicit WAV(const std::string& wavPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] const std::vector<std::unique_ptr<ChunkBase>>& getChunks() const;

	[[nodiscard]] std::vector<std::unique_ptr<ChunkBase>>& getChunks();

	[[nodiscard]] const ChunkBase* getFirstChunk(WAV::ChunkType type) const;

	[[nodiscard]] const ChunkFMT*  getChunkFMT()  const;
	[[nodiscard]] const ChunkDATA* getChunkDATA() const;
	[[nodiscard]] const ChunkCUE*  getChunkCUE()  const;
	[[nodiscard]] const ChunkPLST* getChunkPLST() const;
	[[nodiscard]] const ChunkFACT* getChunkFACT() const;
	[[nodiscard]] const ChunkSMPL* getChunkSMPL() const;
	[[nodiscard]] const ChunkINST* getChunkINST() const;
	[[nodiscard]] const ChunkCSET* getChunkCSET() const;
	[[nodiscard]] const ChunkMD5*  getChunkMD5()  const;
	[[nodiscard]] const ChunkJUNK* getChunkJUNK() const;
	[[nodiscard]] const ChunkPAD*  getChunkPAD()  const;
	[[nodiscard]] const ChunkFLLR* getChunkFLLR() const;
	[[nodiscard]] const ChunkLIST* getChunkLIST() const;

	[[nodiscard]] uint32_t getSignature() const;

	void setSignature(uint32_t newSignature);

protected:
	std::vector<std::unique_ptr<ChunkBase>> chunks;
	uint32_t signature;
};

} // namespace sndpp
