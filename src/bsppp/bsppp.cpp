#include <bsppp/bsppp.h>

#include <filesystem>
#include <type_traits>
#include <utility>
#include <algorithm>

#include <FileStream.h>

#include <third_party/liblzma/src/liblzma/api/lzma.h> //should maybe make this a submodule but minizip already downloads it for us
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
	this->header.version = version;
	this->writeHeader();
}

int32_t BSP::getMapRevision() const {
	return this->header.mapRevision;
}

void BSP::setMapRevision(int32_t mapRevision) {
	this->header.mapRevision = mapRevision;
	this->writeHeader();
}

bool BSP::hasLump(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return false;
	}
	auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	return this->header.lumps[lump].length != 0 && this->header.lumps[lump].offset != 0;
}

bool BSP::isLumpCompressed(BSPLump lumpIndex) const {
    if (this->hasLump(lumpIndex)){
        return this->header.lumps[static_cast<int32_t>(lumpIndex)].fourCC > 0;
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
	this->header.lumps[static_cast<std::underlying_type_t<BSPLump>>(lumpIndex)].version = version;
	this->writeHeader();
}

std::optional<std::vector<std::byte>> BSP::readLump(BSPLump lumpIndex, bool readRaw) const {
    if (this->path.empty() || !this->hasLump(lumpIndex)) {
		return std::nullopt;
	}
	FileStream reader{this->path};
    std::vector<std::byte> lumpBytes = reader
            .seek_in(this->header.lumps[static_cast<int32_t>(lumpIndex)].offset)
            .read_bytes(this->header.lumps[static_cast<int32_t>(lumpIndex)].length);

    if ( this->isLumpCompressed(lumpIndex) && !readRaw) {
        // Transplant Valve header with a normal one
        lzma_header_bsplump *headerBSP = (lzma_header_bsplump*)(lumpBytes.data());
        lzma_header_alone headerAlone;
        headerAlone.actualSize = headerBSP->actualSize;
        std::copy(headerBSP->properties, headerBSP->properties + sizeof(headerBSP->properties), headerAlone.properties);
        int32_t uncompressedSize = headerAlone.actualSize;

        lumpBytes.erase(lumpBytes.begin(), lumpBytes.begin() + sizeof(lzma_header_bsplump));
        lumpBytes.insert(lumpBytes.begin(), (std::byte*)&headerAlone, ((std::byte*)&headerAlone) + sizeof(lzma_header_alone));

        std::byte decodedLump[uncompressedSize];

        uint64_t memlimit = 1074000000; // 1 GB should be more than enough
        lzma_stream stream = LZMA_STREAM_INIT;
        stream.next_in = (uint8_t*)lumpBytes.data();
        stream.avail_in = lumpBytes.size();
        stream.next_out = (uint8_t*)decodedLump;
        stream.avail_out = uncompressedSize;
        lzma_ret initError = lzma_alone_decoder(&stream, memlimit);

        if (initError) {
            lzma_end(&stream);
            return std::nullopt;
        }

        printf("hucs: %i, lhucs: %i, len: %i\n", uncompressedSize, this->header.lumps[static_cast<int32_t>(lumpIndex)].fourCC, this->header.lumps[static_cast<int32_t>(lumpIndex)].length);
        lzma_ret decompressError = lzma_code(&stream, LZMA_RUN);

        lzma_end(&stream);
        if (decompressError != LZMA_STREAM_END && decompressError != LZMA_OK) {
            printf("decompressError: %i\n", decompressError);
            return std::nullopt;
        }

        std::vector<std::byte> decodedLumpVec;
        decodedLumpVec.insert(decodedLumpVec.begin(), decodedLump, decodedLump + uncompressedSize);
        return decodedLumpVec;
    }
    else {
        // Not Compressed and we dont need to do anything
        return lumpBytes;
    }

}

bool BSP::writeLump(BSPLump lumpIndex, const std::span<const std::byte> data, bool condenseFile, bool compress, uint32_t compressLevel) {

	if (this->path.empty() || lumpIndex == BSPLump::UNKNOWN) {
        return false;
	}

    auto lumpToMove = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);

    if (!this->hasLump(lumpIndex) || !condenseFile) {
        // Put the lump at the end of the file
        int32_t lastLumpOffset = 0, lastLumpLength = 0;
        for (const Lump& lump : this->header.lumps) {
            if (lastLumpOffset < this->header.lumps[lumpToMove].offset) {
                lastLumpOffset = lump.offset;
                lastLumpLength = lump.length;
            }
        }
        if (lastLumpOffset == 0) {
            // Whole file is full of empty lumps
            this->header.lumps[lumpToMove].offset = sizeof(Header);
        } else {
            this->header.lumps[lumpToMove].offset = lastLumpOffset + lastLumpLength;
        }
        this->header.lumps[lumpToMove].length = static_cast<int32_t>(data.size());
    }
    else {
        // Sort lumps by file position
        std::array<int, BSP_LUMP_COUNT> lumpIDs{};
        for (int i = 0; i < lumpIDs.size(); i++) {
            lumpIDs[i] = i;
        }
        std::sort(lumpIDs.begin(), lumpIDs.end(), [this](int lhs, int rhs) {
            return this->header.lumps[lhs].offset < this->header.lumps[rhs].offset;
        });

        // Condense the lumps in the order they appear in the file, and move the new lump to the end
        FileStream bsp{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
        int32_t currentOffset = 0;
        for (int i = 0; i < lumpIDs.size(); i++) {
            auto lumpID = lumpIDs[i];

            if (lumpID == lumpToMove) {
                continue;
            }
            if (!currentOffset) {
                currentOffset = this->header.lumps[lumpID].offset + this->header.lumps[lumpID].length;
                continue;
            }

            auto lumpsData = bsp.seek_in(this->header.lumps[lumpID].offset).read_bytes(this->header.lumps[lumpID].length);
            bsp.seek_out(currentOffset).write(lumpsData);

            this->header.lumps[lumpID].offset = currentOffset;
            currentOffset += this->header.lumps[lumpID].length;

            // If we have the space to add padding (we should), then do so
            // This should never fail for well-constructed BSP files
            auto padding = math::getPaddingForAlignment(4, currentOffset);
            if (padding && i < lumpIDs.size() - 1 && currentOffset + padding <= this->header.lumps[lumpIDs[i + 1]].offset) {
                currentOffset += padding;
            }
        }

        this->header.lumps[lumpToMove].offset = currentOffset;
        this->header.lumps[lumpToMove].length = static_cast<int32_t>(data.size());
    }

    // Write modified header and lump
    this->writeHeader();
    FileStream writer{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
    if (compress) {
        std::byte compressedData[data.size() + sizeof(lzma_header_alone)];// If we somehow get a 0% ratio
        lzma_stream stream = LZMA_STREAM_INIT;

        stream.next_in = (uint8_t*)data.data();
        stream.avail_in = data.size();
        stream.next_out = (uint8_t*)compressedData;
        stream.avail_out = sizeof(compressedData);


        lzma_options_lzma options;
        if ( 9 < compressLevel || compressLevel < 0 )// Out of allowed range
            compressLevel = 6;
        lzma_lzma_preset(&options, compressLevel);
        lzma_ret initError = lzma_alone_encoder(&stream, &options);

        if (initError) {
            lzma_end(&stream);
            printf("init %i", initError);
            return false;
        }

        lzma_ret compressError = LZMA_OK;
//        while (compressError == LZMA_OK) {
            compressError = lzma_code(&stream, LZMA_RUN);
        //     if (stream.total_in >= data.size()) {
        //         break;
        //     }
        // }
        if (compressError == LZMA_STREAM_END) {
            compressError = lzma_code(&stream, LZMA_FINISH);
            lzma_end(&stream);
        }

        if (compressError != LZMA_OK) {
            printf("compress %i\n", compressError);
            lzma_end(&stream);
            return false;
        }

        std::vector<std::byte> newLump;
        newLump.insert(newLump.begin(), compressedData, compressedData + stream.total_out);

        // Transplant header with Valve's
        lzma_header_bsplump headerBSP;
        lzma_header_alone *headerAlone = (lzma_header_alone*)compressedData;
        headerBSP.id = LZMA_ID;
        headerBSP.actualSize = data.size();
        printf("actual: %u\nbsp: %i\nvec: %i", headerAlone->actualSize, headerBSP.actualSize, data.size());
        headerBSP.lzmaSize = stream.total_out /*- sizeof(lzma_header_alone) + sizeof(lzma_header_bsplump)*/;
        std::copy(headerAlone->properties, headerAlone->properties + sizeof(headerAlone->properties), headerBSP.properties);

        newLump.erase(newLump.begin(), newLump.begin() + sizeof(lzma_header_alone));
        newLump.insert(newLump.begin(), (std::byte*)&headerBSP, ((std::byte*)&headerBSP) + sizeof(lzma_header_bsplump));


        this->header.lumps[static_cast<uint32_t>(lumpIndex)].fourCC = data.size();
        writer.seek_out(this->header.lumps[static_cast<uint32_t>(lumpIndex)].offset).write(newLump);
    }
    else {
        writer.seek_out(this->header.lumps[static_cast<uint32_t>(lumpIndex)].offset).write(data);
    }

    // Resize file if it shrank
    int32_t lastLumpOffset = 0, lastLumpLength = 0;
    for (const Lump& lump : this->header.lumps) {
        if (lastLumpOffset < this->header.lumps[lumpToMove].offset) {
            lastLumpOffset = lump.offset;
            lastLumpLength = lump.length;
        }
    }
    if (std::filesystem::file_size(this->path) > lastLumpOffset + lastLumpLength) {
        std::filesystem::resize_file(this->path, lastLumpOffset + lastLumpLength);
    }

    return true;
}

bool BSP::applyLumpPatchFile(const std::string& lumpFilePath) {
	if (this->path.empty()) {
		return false;
	}

	FileStream reader{lumpFilePath};
	if (!reader) {
		return false;
	}

	auto offset = reader.read<int32_t>();
	auto index = reader.read<int32_t>();
	auto version = reader.read<int32_t>();
	auto length = reader.read<int32_t>();
	if (index < 0 || index > BSP_LUMP_COUNT || offset <= 0 || length <= 0) {
		return false;
	}

	this->header.lumps[index].version = version;
	this->writeLump(static_cast<BSPLump>(index), reader.seek_in(offset).read_bytes(length));
	return true;
}

void BSP::createLumpPatchFile(BSPLump lumpIndex) const {
	auto lumpData = this->readLump(lumpIndex);
	if (!lumpData) {
		return;
	}

	auto& lump = this->header.lumps.at(static_cast<std::underlying_type_t<BSPLump>>(lumpIndex));

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
		.write(lump.version)
		.write(lump.length)
		.write(this->header.mapRevision)
		.write(*lumpData);
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
