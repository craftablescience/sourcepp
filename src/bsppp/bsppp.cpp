#include <bsppp/bsppp.h>

#include <filesystem>
#include <type_traits>
#include <utility>
#include <algorithm>

#include <FileStream.h>

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

std::optional<std::vector<std::byte>> BSP::readLump(BSPLump lump) const {
	if (this->path.empty() || !this->hasLump(lump)) {
		return std::nullopt;
	}
	FileStream reader{this->path};
	return reader
		.seek_in(this->header.lumps[static_cast<std::underlying_type_t<BSPLump>>(lump)].offset)
		.read_bytes(this->header.lumps[static_cast<std::underlying_type_t<BSPLump>>(lump)].length);
}

void BSP::writeLump(BSPLump lumpIndex, std::span<const std::byte> data, bool condenseFile) {
	if (this->path.empty() || lumpIndex == BSPLump::UNKNOWN) {
		return;
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
	} else {
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
	{
		FileStream writer{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
		writer.seek_out(this->header.lumps[lumpToMove].offset).write(data);
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
