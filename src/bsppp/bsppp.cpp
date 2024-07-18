#include <bsppp/bsppp.h>

#include <filesystem>
#include <limits>
#include <utility>

#include <FileStream.h>

using namespace bsppp;

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
	reader
		.read(this->header.version)
		.read(this->header.lumps)
		.read(this->header.mapRevision);
}

BSP::operator bool() const {
	return !this->path.empty();
}

BSP BSP::create(std::string path, int32_t version, int32_t mapRevision) {
	FileStream writer{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	writer << BSP_SIGNATURE << BSP::Header{
		.version = version,
		.lumps = {},
		.mapRevision = mapRevision,
	};
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
	auto lump = static_cast<int32_t>(lumpIndex);
	return this->header.lumps[lump].length != 0 && this->header.lumps[lump].offset != 0;
}

std::optional<std::vector<std::byte>> BSP::readLump(BSPLump lump) const {
	if (this->path.empty() || !this->hasLump(lump)) {
		return std::nullopt;
	}
	FileStream reader{this->path};
	return reader
		.seek_in(this->header.lumps[static_cast<int32_t>(lump)].offset)
		.read_bytes(this->header.lumps[static_cast<int32_t>(lump)].length);
}

void BSP::writeLump(BSPLump lumpIndex, const std::vector<std::byte>& data) {
	if (this->path.empty()) {
		return;
	}

	this->moveLumpToWritableSpace(lumpIndex, static_cast<int>(data.size()));

	FileStream writer{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
	writer
		.seek_out(0)
		.write(BSP_SIGNATURE)
		.write(this->header.version)
		.write(this->header.lumps)
		.write(this->header.mapRevision)
		.seek_out(this->header.lumps[static_cast<uint32_t>(lumpIndex)].offset)
		.write(data);
}

void BSP::createLumpPatchFile(BSPLump lumpIndex) const {
	auto lumpData = this->readLump(lumpIndex);
	if (!lumpData) {
		return;
	}

	auto& lump = this->header.lumps.at(static_cast<int32_t>(lumpIndex));

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
	writer
		.seek_out(0)
		.write(BSP_SIGNATURE)
		.write(this->header.version)
		.write(this->header.lumps)
		.write(this->header.mapRevision);
}

void BSP::moveLumpToWritableSpace(BSPLump lumpIndex, int newSize) {
	auto lumpToMove = static_cast<uint32_t>(lumpIndex);
	this->header.lumps[lumpToMove].length = newSize;

	// If the moving lump is at the end of the file we just overwrite it, otherwise we have to shift some lumps over
	std::vector<int> lumpsAfterMovingLumpIndices;
	for (int i = 0; i < this->header.lumps.size(); i++) {
		if (this->header.lumps[i].offset > this->header.lumps[lumpToMove].offset) {
			lumpsAfterMovingLumpIndices.push_back(i);
		}
	}
	if (lumpsAfterMovingLumpIndices.empty()) {
		return;
	}

	// Get the exact area to move
	int moveOffsetStart = std::numeric_limits<int>::max(), moveOffsetEnd = 0;
	for (int lumpsAfterMovingLumpIndex : lumpsAfterMovingLumpIndices) {
		if (this->header.lumps[lumpsAfterMovingLumpIndex].offset < moveOffsetStart) {
			moveOffsetStart = this->header.lumps[lumpsAfterMovingLumpIndex].offset;
		}
		if (auto offsetAndLength = this->header.lumps[lumpsAfterMovingLumpIndex].offset + this->header.lumps[lumpsAfterMovingLumpIndex].length; offsetAndLength > moveOffsetEnd) {
			moveOffsetEnd = offsetAndLength;
		}
	}

	// Get where to move it
	int lastLumpBeforeMovingLumpOffset = 0, lastLumpBeforeMovingLumpLength = 0;
	for (const Lump& lump : this->header.lumps) {
		if (lump.offset < this->header.lumps[lumpToMove].offset && lump.offset > lastLumpBeforeMovingLumpOffset) {
			lastLumpBeforeMovingLumpOffset = lump.offset;
			lastLumpBeforeMovingLumpLength = lump.length;
		}
	}

	// Move all the lumps after paklump back
	{
		FileStream bsp{this->path, FileStream::OPT_READ | FileStream::OPT_WRITE};
		auto lumpsData = bsp.seek_in(moveOffsetStart).read_bytes(moveOffsetEnd - moveOffsetStart);
		bsp.seek_out(lastLumpBeforeMovingLumpOffset + lastLumpBeforeMovingLumpLength).write(lumpsData);

		// Fix the offsets
		for (int lumpsAfterMovingLumpIndex : lumpsAfterMovingLumpIndices) {
			this->header.lumps[lumpsAfterMovingLumpIndex].offset -= newSize;
		}
		this->header.lumps[lumpToMove].offset = lastLumpBeforeMovingLumpOffset + lastLumpBeforeMovingLumpLength + static_cast<int>(lumpsData.size());
	}

	// Write modified header
	this->writeHeader();
}
