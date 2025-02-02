#include <sndpp/WAV.h>

#include <utility>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sndpp;
using namespace sourcepp;

WAV::WAV(std::span<const std::byte> wavData) {
	goto start;
fail:
	this->signature = 0;
	return;
start:
	BufferStreamReadOnly stream{wavData.data(), wavData.size()};

	this->signature = stream.read<uint32_t>();
	if (this->signature == RIFX_SIGNATURE || this->signature == FFIR_SIGNATURE) {
		stream.set_big_endian(true);
	} else if (this->signature != RIFF_SIGNATURE) {
		goto fail;
	}

	const auto totalLength = stream.read<uint32_t>();
	if (stream.read<uint32_t>() != RIFF_TYPE) {
		goto fail;
	}

	while (stream.tell() < totalLength) {
		std::unique_ptr<ChunkBase> ptr;

		#define SNDPP_CHUNK_SETUP(ID) \
			auto* ID = new Chunk##ID; \
			ID->type = type; \
			ptr.reset(ID)

		const auto type = stream.read<ChunkType>();
		switch (type) {
			using enum ChunkType;
			case FMT: {
				SNDPP_CHUNK_SETUP(FMT);
				// NOLINTNEXTLINE(*-sizeof-container)
				const auto fmtExtraCompressionInfoLength = stream.read<uint32_t>() - 16; // size of the standard FMT header
				stream >> FMT->format >> FMT->channels >> FMT->samplesPerSecond >> FMT->averageBytesPerSecond >> FMT->blockAlign >> FMT->bitsPerSample;
				if (fmtExtraCompressionInfoLength) {
					FMT->extraCompressionInfo = stream.read_bytes(fmtExtraCompressionInfoLength);
				}
				break;
			}
			case DATA: {
				SNDPP_CHUNK_SETUP(DATA);
				DATA->data = stream.read_bytes(stream.read<uint32_t>());
				break;
			}
			case CUE: {
				SNDPP_CHUNK_SETUP(CUE);
				if ((stream.read<uint32_t>() - sizeof(uint32_t)) % sizeof(ChunkCUE::CuePoint) != 0) {
					goto fail;
				}
				const auto cueNumPoints = stream.read<uint32_t>();
				for (uint32_t i = 0; i < cueNumPoints; i++) {
					ChunkCUE::CuePoint point{};
					stream >> point.id >> point.position >> point.chunk >> point.chunkStart >> point.blockStart >> point.sampleOffset;
					CUE->cuePoints.push_back(point);
				}
				break;
			}
			case PLST: {
				SNDPP_CHUNK_SETUP(PLST);
				if ((stream.read<uint32_t>() - sizeof(uint32_t)) % sizeof(ChunkPLST::Segment) != 0) {
					goto fail;
				}
				const auto plstNumSegments = stream.read<uint32_t>();
				for (uint32_t i = 0; i < plstNumSegments; i++) {
					ChunkPLST::Segment segment{};
					stream >> segment.id >> segment.length >> segment.repeats;
					PLST->segments.push_back(segment);
				}
				break;
			}
			case FACT: {
				SNDPP_CHUNK_SETUP(FACT);
				const auto factExtraCompressionInfoLength = stream.read<uint32_t>() - sizeof(uint32_t);
				stream >> FACT->samples;
				if (factExtraCompressionInfoLength) {
					FACT->extraCompressionInfo = stream.read_bytes(factExtraCompressionInfoLength);
				}
				break;
			}
			case SMPL: {
				SNDPP_CHUNK_SETUP(SMPL);
				stream.skip<uint32_t>();
				stream >> SMPL->manufacturer >> SMPL->product >> SMPL->samplePeriod >> SMPL->midiUnityNote >> SMPL->midiPitchFraction
				       >> SMPL->smpteFormat >> SMPL->smpteOffset >> SMPL->sampleLoops >> SMPL->samplerData;
				const auto smplNumLoops = stream.read<uint32_t>();
				for (uint32_t i = 0; i < smplNumLoops; i++) {
					ChunkSMPL::SampleLoop sampleLoop{};
					stream >> sampleLoop.id >> sampleLoop.type >> sampleLoop.start >> sampleLoop.end >> sampleLoop.fraction >> sampleLoop.playCount;
					SMPL->loops.push_back(sampleLoop);
				}
				break;
			}
			case INST: {
				SNDPP_CHUNK_SETUP(INST);
				if (stream.read<uint32_t>() != sizeof(ChunkINST)) {
					goto fail;
				}
				stream >> INST->unshiftedNote >> INST->fineTune >> INST->gain >> INST->lowNote >> INST->highNote >> INST->lowVelocity >> INST->highVelocity;
				break;
			}
			case CSET: {
				SNDPP_CHUNK_SETUP(CSET);
				if (stream.read<uint32_t>() != sizeof(ChunkCSET)) {
					goto fail;
				}
				stream >> CSET->codePage >> CSET->countryCode >> CSET->language >> CSET->dialect;
				break;
			}
			case MD5: {
				SNDPP_CHUNK_SETUP(MD5);
				if (stream.read<uint32_t>() != sizeof(ChunkMD5::md5)) {
					goto fail;
				}
				stream >> MD5->md5;
				break;
			}
			case JUNK: {
				SNDPP_CHUNK_SETUP(JUNK);
				JUNK->junk = stream.read_bytes(stream.read<uint32_t>());
				break;
			}
			case PAD: {
				SNDPP_CHUNK_SETUP(PAD);
				PAD->padding = stream.read_bytes(stream.read<uint32_t>());
				break;
			}
			case FLLR: {
				SNDPP_CHUNK_SETUP(FLLR);
				FLLR->filler = stream.read_bytes(stream.read<uint32_t>());
				break;
			}
			case LIST: {
				const auto listLength = stream.read<uint32_t>();
				const auto listType = stream.read<ChunkListType>();
				switch (listType) {
					using enum ChunkListType;
					default: // Not in the "spec"
					case WAVL: // No wave lists!
						goto fail;
					case INFO: {
						SNDPP_CHUNK_SETUP(INFO);
						const auto infoStart = stream.tell() + listLength - sizeof(ChunkListInfoType);
						while (stream.tell() < infoStart) {
							const auto infoType = stream.read<ChunkListInfoType>();
							const auto metadata = stream.read_string(stream.read<uint32_t>());
							INFO->metadata.push_back({infoType, metadata});
						}
						break;
					}
					case ADTL: {
						SNDPP_CHUNK_SETUP(ADTL);
						const auto adtlStart = stream.tell() + listLength - sizeof(ChunkListADTLType);
						while (stream.tell() < adtlStart) {
							const auto adtlType = stream.read<ChunkListADTLType>();
							switch (adtlType) {
								using enum ChunkListADTLType;
								case LABL: {
									const auto labelLength = stream.read<uint32_t>();
									const auto labelID = stream.read<uint32_t>();
									const auto labelText = stream.read_string(labelLength - sizeof(uint32_t));
									ADTL->labels.push_back({labelID, labelText});
									break;
								}
								case NOTE: {
									const auto noteLength = stream.read<uint32_t>();
									const auto noteID = stream.read<uint32_t>();
									const auto noteText = stream.read_string(noteLength - sizeof(uint32_t));
									ADTL->notes.push_back({noteID, noteText});
									break;
								}
								case LTXT: {
									const auto ltxtLength = stream.read<uint32_t>();
									ChunkADTL::LTXT ltxt;
									stream >> ltxt.id >> ltxt.sampleLength >> ltxt.purpose >> ltxt.country >> ltxt.language >> ltxt.dialect >> ltxt.codePage;
									ltxt.label = stream.read_string(ltxtLength - (sizeof(ChunkADTL::LTXT) - sizeof(std::string)));
									ADTL->labelTexts.push_back(ltxt);
									break;
								}
							}
						}
						break;
					}
				}
			}
			default: {
				SNDPP_CHUNK_SETUP(Unknown);
				Unknown->data = stream.read_bytes(stream.read<uint32_t>());
				break;
			}
		}

		#undef SNDPP_CHUNK_SETUP

		if (ptr) {
			this->chunks.push_back(std::move(ptr));
		}

		if (stream.tell() % 2 != 0) {
			stream.skip();
		}
	}

	// FMT chunk and DATA chunk are required
	if (!this->getFirstChunk(WAV::ChunkType::FMT) || !this->getFirstChunk(WAV::ChunkType::DATA)) {
		goto fail;
	}
}

WAV::WAV()
		: signature(RIFF_SIGNATURE) {}

WAV::WAV(const std::string& wavPath)
		: WAV(fs::readFileBuffer(wavPath)) {}

WAV::operator bool() const {
	return this->signature;
}

const std::vector<std::unique_ptr<WAV::ChunkBase>>& WAV::getChunks() const {
	return this->chunks;
}

std::vector<std::unique_ptr<WAV::ChunkBase>>& WAV::getChunks() {
	return this->chunks;
}

const WAV::ChunkBase* WAV::getFirstChunk(WAV::ChunkType type) const {
	if (const auto it = std::find_if(this->chunks.begin(), this->chunks.end(), [type](const std::unique_ptr<ChunkBase>& chunk) {
		return chunk->type == type;
	}); it != this->chunks.end()) {
		return it->get();
	}
	return nullptr;
}

uint32_t WAV::getSignature() const {
	return this->signature;
}

void WAV::setSignature(uint32_t newSignature) {
	this->signature = newSignature;
}

#define SNDPP_CHUNK_GETTER(TYPE) \
	const WAV::Chunk##TYPE* WAV::getChunk##TYPE() const { \
		return dynamic_cast<const Chunk##TYPE*>(this->getFirstChunk(WAV::ChunkType::TYPE)); \
	}

SNDPP_CHUNK_GETTER(FMT)
SNDPP_CHUNK_GETTER(DATA)
SNDPP_CHUNK_GETTER(CUE)
SNDPP_CHUNK_GETTER(PLST)
SNDPP_CHUNK_GETTER(FACT)
SNDPP_CHUNK_GETTER(SMPL)
SNDPP_CHUNK_GETTER(INST)
SNDPP_CHUNK_GETTER(CSET)
SNDPP_CHUNK_GETTER(MD5)
SNDPP_CHUNK_GETTER(JUNK)
SNDPP_CHUNK_GETTER(PAD)
SNDPP_CHUNK_GETTER(FLLR)
SNDPP_CHUNK_GETTER(LIST)

#undef SNDPP_CHUNK_GETTER
