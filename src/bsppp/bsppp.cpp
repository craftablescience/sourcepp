#include <bsppp/bsppp.h>

#include <algorithm>
#include <filesystem>
#include <type_traits>
#include <utility>

#include <FileStream.h>
#include <lzma.h>

using namespace bsppp;
using namespace sourcepp;

namespace {

template<BufferStreamPODType T>
[[nodiscard]] std::vector<T> readLumpContents(const BSP& bsp, BSPLump lump, void(*callback)(const BSP&, BufferStreamReadOnly&, std::vector<T>&) = [](const BSP&, BufferStreamReadOnly& stream, std::vector<T>& out) {
	stream.read(out, stream.size() / sizeof(T));
}) {
	auto data = bsp.readLump(lump);
	if (!data) {
		return {};
	}

	BufferStreamReadOnly stream{*data};
	std::vector<T> out;
	callback(bsp, stream, out);
	return out;
}

template<BufferStreamPODType Old, BufferStreamPODType New>
requires requires(New) { {New::upgrade(Old{})} -> std::same_as<New>; }
void readAndUpgrade(BufferStreamReadOnly& stream, std::vector<New>& out) {
	std::vector<Old> old;
	stream.read(old, stream.size() / sizeof(Old));
	for (const auto& elem : old) {
		out.push_back(New::upgrade(elem));
	}
}

} // namespace

BSP::BSP(std::string path_)
		: path(std::move(path_)) {
	FileStream reader{this->path};
	if (!reader) {
		this->path.clear();
		return;
	}

	if (auto signature = reader.seek_in(0).read<uint32_t>(); signature != BSP_SIGNATURE) {
		// File is not a BSP
		this->path.clear();
		return;
	}
	this->header.version = reader.read<int32_t>();

	// Contagion funny
	if (this->header.version == 27) {
		reader.skip_in<int32_t>();
	}

	reader >> this->header.lumps;

	// If no offsets are larger than 1024 (less than the size of the BSP header, but greater than any lump version),
	// it's probably a L4D2 BSP and those are lump versions!
	if (this->header.version == 21) {
		int i;
		for (i = 0; i < BSP_LUMP_COUNT; i++) {
			if (this->header.lumps[i].offset > 1024) {
				break;
			}
		}
		this->isL4D2 = i == BSP_LUMP_COUNT;
		if (this->isL4D2) {
			// Swap fields around
			for (i = 0; i < BSP_LUMP_COUNT; i++) {
				std::swap(this->header.lumps[i].offset, this->header.lumps[i].version);
				std::swap(this->header.lumps[i].offset, this->header.lumps[i].length);
			}
		}
	}

	reader >> this->header.mapRevision;
	this->stagedHeader = this->header;
	this->stagedGameLumps = this->readGameLumps();
}

BSP::operator bool() const {
	return !this->path.empty();
}

BSP BSP::create(std::string path, int32_t version, int32_t mapRevision) {
	{
		FileStream writer{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		writer << BSP_SIGNATURE << BSP::Header{
			.version = version,
			.lumps = {},
			.mapRevision = mapRevision,
		};
	}
	return BSP{std::move(path)};
}

int32_t BSP::getVersion() const {
	return this->header.version;
}

void BSP::setVersion(int32_t version) {
	this->stagedHeader.version = version;
}

int32_t BSP::getMapRevision() const {
	return this->header.mapRevision;
}

void BSP::setMapRevision(int32_t mapRevision) {
	this->stagedHeader.mapRevision = mapRevision;
}

bool BSP::hasLump(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return false;
	}
    const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	return this->header.lumps[lump].length != 0 && this->header.lumps[lump].offset != 0;
}

bool BSP::isLumpCompressed(BSPLump lumpIndex) const {
    if (this->hasLump(lumpIndex)){
        return this->header.lumps[static_cast<std::underlying_type_t<BSPLump>>(lumpIndex)].fourCC > 0;
    }
    return false;
}

int32_t BSP::getLumpVersion(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return 0;
	}
	return this->header.lumps[static_cast<std::underlying_type_t<BSPLump>>(lumpIndex)].version;
}

void BSP::setLumpVersion(BSPLump lumpIndex, int32_t version) {
	this->stagedHeader.lumps[static_cast<std::underlying_type_t<BSPLump>>(lumpIndex)].version = version;
}

std::optional<std::vector<std::byte>> BSP::readLump(BSPLump lumpIndex, bool readRaw, bool stagedVersion) const {
    if (this->path.empty() || !this->hasLump(lumpIndex)) {
		return std::nullopt;
	}

	auto lumpToRead = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	std::vector<std::byte> lumpBytes;

	if (stagedVersion) {
		lumpBytes = this->stagedLumps[lumpToRead];
	}
	else{
		FileStream reader{this->path};
		lumpBytes = reader
					.seek_in(this->header.lumps[lumpToRead].offset)
					.read_bytes(this->header.lumps[lumpToRead].length);
	}

    if ( this->isLumpCompressed(lumpIndex) && !readRaw) {
        return decompressLumpData(lumpBytes);
    }
    else {
        // Not Compressed and we dont need to do anything
        return lumpBytes;
    }

}

bool BSP::stageLump(BSPLump lumpIndex, const std::span<const std::byte> data, uint8_t compressLevel) {

	if (this->path.empty() || lumpIndex == BSPLump::UNKNOWN) {
        return false;
	}

	auto lumpToStage = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);

    if (compressLevel > 0) {
        auto newLump = compressLumpData(data, compressLevel);
        if (!newLump.has_value())
            return false;

        this->stagedHeader.lumps[lumpToStage].fourCC = data.size();
        this->stagedLumps[lumpToStage].clear();
        this->stagedLumps[lumpToStage].insert(stagedLumps[lumpToStage].begin(), newLump.value().begin(), newLump.value().end());
    }
    else {
        this->stagedHeader.lumps[lumpToStage].fourCC = 0;
        this->stagedLumps[lumpToStage].clear();
        this->stagedLumps[lumpToStage].insert(stagedLumps[lumpToStage].begin(), data.begin(), data.end());
    }

    return true;
}

bool BSP::stageLump(std::span<BSPEntity> entities, uint8_t compressLevel) {
    if (this->path.empty()) {
        return false;
    }

    std::vector<std::byte> byteVec;
    for (auto e : entities)
    {
        byteVec.push_back((std::byte)'{');
        for (auto it = e.begin(); it != e.end(); it++) {
            byteVec.push_back((std::byte)'\"');

            for (char c : it->first){
                byteVec.push_back((std::byte)c);
            }
            byteVec.push_back((std::byte)'\"');
            byteVec.push_back((std::byte)' ');
            byteVec.push_back((std::byte)'\"');
            for (char c : it->second){
                byteVec.push_back((std::byte)c);
            }
            byteVec.push_back((std::byte)'\"');
        }
        byteVec.push_back((std::byte)'}');
    }
    return this->stageLump(BSPLump::ENTITIES, byteVec, compressLevel);
}

void BSP::writeChangesToDisk() {
    uint32_t offset = sizeof(BSP_SIGNATURE) + sizeof(BSP::Header);
    this->header = this->stagedHeader;

    for ( int32_t i = 0; i < BSP_LUMP_COUNT; i++){ // Fill empty space with lumps we're keeping
        if (i == static_cast<int32_t>(BSPLump::GAME_LUMP)
            && this->stagedGameLumps.size() > 0) {// Need special handling since its split up

            uint32_t gameLumpOffset = offset;
            if (this->stagedGameLumps[0].data.size() != this->stagedGameLumps[0].length) {                                                                                           //
            // length member is decompressed size, need to add an empty item with an offset so the compressed size is known later
                BSPGameLump dummy{0,0,0,0,0};
                this->stagedGameLumps.push_back(dummy);
            }

            auto &lump = this->stagedLumps[i];

            lump.clear();
            BufferStream stream(lump);
            stream.write<int32_t>(this->stagedGameLumps.size());
            gameLumpOffset += sizeof(int32_t);
            gameLumpOffset += (sizeof(BSPGameLump) - sizeof(BSPGameLump::data)) * this->stagedGameLumps.size();
            //gameLumpOffset += 128 * this->stagedGameLumps.size();

            for (auto gameLump : this->stagedGameLumps) {
                if (gameLump.id == 0) {
                    gameLump.fileoffset = 0;
                }
                else {
                    gameLump.fileoffset = gameLumpOffset;
                    gameLumpOffset += gameLump.data.size();
                }
                stream.write(gameLump.id)
                        .write(gameLump.flags)
                        .write(gameLump.version)
                        .write(gameLump.fileoffset)
                        .write(gameLump.length);
            }
            for (auto gameLump : this->stagedGameLumps)
            {
                if (gameLump.id == 0)
                    continue;
                stream.write(gameLump.data);
            }
            this->header.lumps[i].offset = offset;
            this->header.lumps[i].length = lump.size();
            offset += lump.size();

        }
        else {
            auto &lump = this->stagedLumps[i];
            if (lump.size() == 0){
                if (this->hasLump(static_cast<BSPLump>(i)))
                    lump = this->readLump(static_cast<BSPLump>(i), true).value();
            }

            if (lump.size() == 0)
                this->header.lumps[i].offset = 0;
            else{
                this->header.lumps[i].offset = offset;
                offset += lump.size();
            }
            this->header.lumps[i].length = lump.size();
        }
    }

    this->writeHeader();
    FileStream writer{ this->path, FileStream::OPT_READ | FileStream::OPT_WRITE };

    for ( int i = 0; i < BSP_LUMP_COUNT; i++){
        if (this->stagedLumps[i].size())
            writer.seek_out(this->header.lumps[i].offset)
                  .write(this->stagedLumps[i]);
    }
}

void BSP::flushChanges(BSPLump lumpIndex)
{
	if (lumpIndex == BSPLump::UNKNOWN) {
		for (int i = 0; i < static_cast<std::underlying_type_t<BSPLump>>(BSPLump::COUNT); i++) {
			this->flushChanges(static_cast<BSPLump>(i));
		}
		return;
	}


	if (lumpIndex == BSPLump::GAME_LUMP) {
		this->stagedGameLumps = this->readGameLumps();
	}
	auto lumpToFlush = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	this->stagedLumps[lumpToFlush].clear();
	this->stagedHeader.lumps[lumpToFlush] = this->header.lumps[lumpToFlush];
}

bool BSP::stageLumpPatchFile(const std::string& lumpFilePath) {
	if (this->path.empty()) {
		return false;
	}

	FileStream reader{lumpFilePath};
	if (!reader) {
		return false;
	}

	const auto offset = reader.read<int32_t>();
	const auto index = reader.read<int32_t>();
	const auto version = reader.read<int32_t>();
	const auto length = reader.read<int32_t>();
	if (index < 0 || index > BSP_LUMP_COUNT || offset <= 0 || length <= 0) {
		return false;
	}

	this->header.lumps[index].version = version;
    this->stageLump(static_cast<BSPLump>(index), reader.seek_in(offset).read_bytes(length));
	return true;
}

void BSP::createLumpPatchFile(BSPLump lumpIndex) const {
	auto lumpData = this->readLump(lumpIndex);
	if (!lumpData) {
		return;
	}

	const auto& [lumpOffset, lumpLength, lumpVersion, lumpFourCC] = this->header.lumps.at(static_cast<std::underlying_type_t<BSPLump>>(lumpIndex));

	const auto fsPath = std::filesystem::path{this->path};
	const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string() + "_l_";
	int nonexistentNumber;
	for (nonexistentNumber = 0; true; nonexistentNumber++) {
		if (!std::filesystem::exists(fsStem + std::to_string(nonexistentNumber) + ".lmp")) {
			break;
		}
	}

	FileStream writer{fsStem + std::to_string(nonexistentNumber) + ".lmp", FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	writer
		.seek_out(0)
		.write<int32_t>(sizeof(int32_t) * 5)
		.write(lumpIndex)
		.write(lumpVersion)
		.write(lumpLength)
		.write(this->header.mapRevision)
		.write(*lumpData);
}

bool BSP::stageGameLump(char id[4], uint16_t flags, uint16_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	return this->stageGameLump(id[0] | (id[1] << 8) | (id[2] << 16) | (id[3] << 24)
								, flags, version, data, compressLevel);
}

bool BSP::stageGameLump(int32_t id, uint16_t flags, uint16_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	BSPGameLump gameLump;
	gameLump.id = id;
	gameLump.flags = flags;
	gameLump.version = version;
	gameLump.length = data.size();

	if (compressLevel){
		auto compressed = compressLumpData(data, compressLevel);
		if (!compressed.has_value())
			return false;
		gameLump.data = compressed.value();
	} else {
		gameLump.data.clear();
		gameLump.data.insert(gameLump.data.begin(), data.begin(), data.end());
	}
	this->stagedGameLumps.push_back(gameLump);
	return true;
}

void BSP::writeHeader() const {
	FileStream writer{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
	writer.seek_out(0) << BSP_SIGNATURE << this->header.version;

	// Contagion funny
	if (this->header.version == 27) {
		writer.write<int32_t>(0);
	}

	if (!this->isL4D2) {
		writer << this->header.lumps;
	} else {
		for (int i = 0; i < static_cast<std::underlying_type_t<BSPLump>>(BSPLump::COUNT); i++) {
			writer << this->header.lumps[i].version << this->header.lumps[i].offset << this->header.lumps[i].length << this->header.lumps[i].fourCC;
		}
	}

	writer << this->header.mapRevision;
}

std::vector<BSPEntity> BSP::parseEntities() const {
	auto lumpData = this->readLump(BSPLump::ENTITIES);

	std::vector<BSPEntity> ents;

	if (!lumpData.has_value())
	{
		return ents;
	}

	auto lumpDataValue = lumpData.value();

	BSPEntity e;
	std::string key, value;
	bool quoted = false;
	bool keying = true;

	for (uint32_t i = 0; i <lumpDataValue.size(); i++){
		char c = (char)lumpDataValue[i];

		if ( c == '\\') {
			i++;
			continue;
		}
		if ( c == '\"') {
			quoted = !quoted;

			if (!quoted && !keying) {
				e[key] = value;
				key.erase();
				value.erase();
			}

			if (!quoted) {
				keying = !keying;
			}
			continue;
		}
		if ( c == '}') {
			ents.push_back(e);
			e.clear();
			continue;
		}
		if (!quoted)
			continue;

		if ( keying ) {
			key += c;
		}
		else {
			value += c;
		}
	}

	if ( quoted || !keying ) // somethings messed up
		ents.clear();

	return ents;
}

std::vector<BSPPlane> BSP::readPlanes() const {
	return ::readLumpContents<BSPPlane>(*this, BSPLump::PLANES);
}

std::vector<BSPTextureData> BSP::readTextureData() const {
	return ::readLumpContents<BSPTextureData>(*this, BSPLump::TEXDATA);
}

std::vector<BSPVertex> BSP::readVertices() const {
	return ::readLumpContents<BSPVertex>(*this, BSPLump::VERTEXES);
}

std::vector<BSPTextureInfo> BSP::readTextureInfo() const {
	return ::readLumpContents<BSPTextureInfo>(*this, BSPLump::TEXINFO);
}

std::vector<BSPFace> BSP::readFaces() const {
	return ::readLumpContents<BSPFace>(*this, BSPLump::FACES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPFace>& out) {
		if (bsp.getLumpVersion(BSPLump::FACES) > 1) {
			stream.read(out, stream.size() / sizeof(BSPFace_v2));
		} else {
			::readAndUpgrade<BSPFace_v1>(stream, out);
		}
	});
}

std::vector<BSPEdge> BSP::readEdges() const {
	return ::readLumpContents<BSPEdge>(*this, BSPLump::EDGES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPEdge>& out) {
		if (bsp.getLumpVersion(BSPLump::EDGES) > 0) {
			stream.read(out, stream.size() / sizeof(BSPEdge_v1));
		} else {
			::readAndUpgrade<BSPEdge_v0>(stream, out);
		}
	});
}

std::vector<BSPSurfEdge> BSP::readSurfEdges() const {
	return ::readLumpContents<BSPSurfEdge>(*this, BSPLump::SURFEDGES);
}

std::vector<BSPBrushModel> BSP::readBrushModels() const {
	return ::readLumpContents<BSPBrushModel>(*this, BSPLump::MODELS);
}

std::vector<BSPFace> BSP::readOriginalFaces() const {
	return ::readLumpContents<BSPFace>(*this, BSPLump::ORIGINALFACES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPFace>& out) {
		// ORIGINALFACES lump version is always 0?
		if (bsp.getLumpVersion(BSPLump::FACES) > 1) {
			stream.read(out, stream.size() / sizeof(BSPFace_v2));
		} else {
			::readAndUpgrade<BSPFace_v1>(stream, out);
		}
	});
}

std::vector<BSPGameLump> BSP::readGameLumps() const {
	std::vector<BSPGameLump> lumps;

	auto lumpsAggregate = this->readLump(BSPLump::GAME_LUMP, true);
	if (!lumpsAggregate.has_value())
		return lumps;

	BufferStreamReadOnly stream{lumpsAggregate.value()};

	uint32_t numGameLumps = stream.read<uint32_t>();
	for (int i = 0; i < numGameLumps; i++)
	{
		BSPGameLump lump;
		stream
			.read(lump.id)
			.read(lump.flags)
			.read(lump.version)
			.read(lump.fileoffset)
			.read(lump.length);
		// if (lump.id != 0)
			lumps.push_back(lump);
	}
	bool isCompressed = lumps.back().id == 0; // Game Lumps are compressed individually,
	//when they are there is an empty entry with only the offset to calculate compressed size

	for (int i = 0; i < numGameLumps; i++) {
		if (lumps[i].id == 0)
			break;
		if (!isCompressed) {
			lumps[i].data = stream.read_bytes(lumps[i].length);
		} else {
			//printf("%i: ", i);
			int32_t nextOffset = lumps[i+1].fileoffset;
			if (nextOffset == 0) {
				auto id = static_cast<std::underlying_type_t<BSPLump>>(BSPLump::GAME_LUMP);
				nextOffset = this->header.lumps[id].offset + this->header.lumps[id].length;
			}
			int32_t length = nextOffset - lumps[i].fileoffset;
			//printf("%i\n", length);
			auto data = stream.read_bytes(length);
			auto decompressed = decompressLumpData(data);

			if (!decompressed.has_value())
				lumps[i].data = data;
			else
				lumps[i].data = decompressed.value();
		}
	}
	if (lumps.back().id == 0)
		lumps.pop_back();

	return lumps;

}

std::optional<std::vector<std::byte>> BSP::compressLumpData(const std::span<const std::byte> data, uint8_t compressLevel){
	uint32_t bufsize = data.size_bytes() + sizeof(lzma_header_alone);
	std::byte *compressedData = (std::byte*)malloc(bufsize);
	lzma_stream stream = LZMA_STREAM_INIT;

	stream.next_in = (uint8_t*)data.data();
	stream.avail_in = data.size_bytes(); // + 32;
	stream.next_out = (uint8_t*)compressedData;
	stream.avail_out = bufsize;

	lzma_options_lzma options;
	if ( compressLevel > 9 )// Out of allowed range
		compressLevel = 6;
	lzma_lzma_preset(&options, compressLevel);
	lzma_ret initError = lzma_alone_encoder(&stream, &options);

	if (initError) {
		lzma_end(&stream);
		//printf("init %i", initError);
		free(compressedData);
		return std::nullopt;
	}

	lzma_ret compressError;
	while (true){//(compressError == LZMA_OK) {
		compressError = lzma_code(&stream, LZMA_RUN);
		// if (stream.total_in >= data.size()) {
		//     break;
		// }
		if (compressError != LZMA_OK)
		{
			break;
		}
	}
	// printf("%i\n", compressError);
	//if (compressError == LZMA_STREAM_END)
	//{
		compressError = lzma_code(&stream, LZMA_RUN);
		compressError = lzma_code(&stream, LZMA_FINISH);
		lzma_end(&stream);
	//}

	// if (compressError != LZMA_OK) {
	//     //printf("compress %i\n", compressError);
	//     lzma_end(&stream);
	//     return false;
	// }

	std::vector<std::byte> compressedLump;
	//newLump.insert(newLump.begin(), compressedData, compressedData + stream.total_out);

	// Transplant header with Valve's
	lzma_header_bsplump headerBSP;
	lzma_header_alone *headerAlone = (lzma_header_alone*)compressedData;

	headerBSP.id = BSP_LZMA_ID;
	headerBSP.actualSize = stream.total_in;
	headerBSP.lzmaSize = stream.total_out - sizeof(lzma_header_alone);

	//printf("actual: %i\nbsp: %i\n", headerAlone->actualSize, headerBSP.actualSize);

	std::copy(headerAlone->properties, headerAlone->properties + sizeof(headerAlone->properties), headerBSP.properties);

	//newLump.erase(newLump.begin(), newLump.begin() + sizeof(lzma_header_alone));
	//newLump.insert(newLump.begin(), (std::byte*)&headerBSP, ((std::byte*)&headerBSP) + sizeof(lzma_header_bsplump));
	compressedLump.insert(compressedLump.begin(), (std::byte*)&headerBSP, (std::byte*)&headerBSP + sizeof(headerBSP));
	compressedLump.insert(compressedLump.end(), compressedData + sizeof(lzma_header_alone), compressedData + stream.total_out);
	free(compressedData);
	return compressedLump;
}

std::optional<std::vector<std::byte>> BSP::decompressLumpData(const std::span<const std::byte> data) {
    // Transplant Valve header with a normal one
    lzma_header_bsplump *headerBSP = (lzma_header_bsplump*)(data.data());
    lzma_header_alone headerAlone;
    headerAlone.actualSize = headerBSP->actualSize;

    std::copy(headerBSP->properties, headerBSP->properties + sizeof(headerBSP->properties), headerAlone.properties);
    int32_t uncompressedSize = headerAlone.actualSize;
    //data.erase(data.begin(), data.begin() + sizeof(lzma_header_bsplump));
    //data.insert(data.begin(), (std::byte*)&headerAlone, ((std::byte*)&headerAlone) + sizeof(lzma_header_alone));
    std::span fixedHeaderData = data.subspan(sizeof(lzma_header_bsplump) - sizeof(lzma_header_alone), std::dynamic_extent);
    *((lzma_header_alone*)fixedHeaderData.data()) = headerAlone;

    std::byte *decodedLump = (std::byte*)malloc(uncompressedSize);

    uint64_t memlimit = 1074000000 ; // 1 GB should be more than enough
    lzma_stream stream = LZMA_STREAM_INIT;
    stream.next_in = (uint8_t*)fixedHeaderData.data();
    stream.avail_in = fixedHeaderData.size();
    stream.next_out = (uint8_t*)decodedLump;
    stream.avail_out = uncompressedSize;
    lzma_ret initError = lzma_alone_decoder(&stream, memlimit);

    if (initError) {
        //printf("init error %i\n", initError);
        lzma_end(&stream);
        free(decodedLump);
        return std::nullopt;
    }

    lzma_ret decompressError;
    decompressError = lzma_code(&stream, LZMA_RUN);
    decompressError = lzma_code(&stream, LZMA_FINISH);
    lzma_end(&stream);

    if (decompressError != LZMA_STREAM_END && decompressError != LZMA_OK) {
        return std::nullopt;
    }

    std::vector<std::byte> decodedLumpVec;
    decodedLumpVec.insert(decodedLumpVec.begin(), decodedLump, decodedLump + uncompressedSize);
    free(decodedLump);
    return decodedLumpVec;
}
