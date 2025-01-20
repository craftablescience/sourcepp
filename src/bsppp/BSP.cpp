#include <bsppp/BSP.h>

#include <algorithm>
#include <filesystem>
#include <type_traits>
#include <utility>

#include <FileStream.h>
#include <sourcepp/compression/LZMA.h>

using namespace bsppp;
using namespace sourcepp;

namespace {

template<BufferStreamPODType T>
[[nodiscard]] std::vector<T> parseLumpContents(const BSP& bsp, BSPLump lump, void(*callback)(const BSP&, BufferStreamReadOnly&, std::vector<T>&) = [](const BSP&, BufferStreamReadOnly& stream, std::vector<T>& out) {
	stream.read(out, stream.size() / sizeof(T));
}) {
	auto data = bsp.getLumpData(lump);
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
void parseAndUpgrade(BufferStreamReadOnly& stream, std::vector<New>& out) {
	std::vector<Old> old;
	stream.read(old, stream.size() / sizeof(Old));
	for (const auto& elem : old) {
		out.push_back(New::upgrade(elem));
	}
}

} // namespace

BSP::BSP(std::string path_, bool loadPatchFiles)
		: path(std::move(path_)) {
	if (!this->readHeader()) {
		this->path.clear();
		return;
	}

	this->stagedVersion = this->header.version;
	this->stagedGameLumps = this->parseGameLumps(false);
	this->stagedMapRevision = this->header.mapRevision;

	if (loadPatchFiles) {
		const auto fsPath = std::filesystem::path{this->path};
		const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string() + "_l_";

		for (int i = 0; ; i++) {
			auto patchFilePath = fsStem + std::to_string(i) + ".lmp";
			if (!std::filesystem::exists(patchFilePath)) {
				break;
			}
			this->setLumpFromPatchFile(patchFilePath);
		}
	}
}

BSP::operator bool() const {
	return !this->path.empty();
}

BSP BSP::create(std::string path, uint32_t version, uint32_t mapRevision) {
	{
		FileStream writer{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		writer << BSP_SIGNATURE << version;
		if (version == 27) {
			writer.write<uint32_t>(0);
		}
		for (int i = 0; i < BSP_LUMP_COUNT; i++) {
			writer
				.write<uint32_t>(0)
				.write<uint32_t>(0)
				.write<uint32_t>(0)
				.write<uint32_t>(0);
		}
		writer << mapRevision;
	}
	return BSP{std::move(path)};
}

uint32_t BSP::getVersion() const {
	return this->stagedVersion;
}

void BSP::setVersion(uint32_t version) {
	this->stagedVersion = version;
}

uint32_t BSP::getMapRevision() const {
	return this->stagedMapRevision;
}

void BSP::setMapRevision(uint32_t mapRevision) {
	this->stagedMapRevision = mapRevision;
}

bool BSP::hasLump(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return false;
	}
	const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	return (this->stagedLumps.contains(lump) && this->stagedLumps.at(lump).first.length != 0 && this->stagedLumps.at(lump).first.offset != 0)
		|| (this->header.lumps[lump].length != 0 && this->header.lumps[lump].offset != 0);
}

bool BSP::isLumpCompressed(BSPLump lumpIndex) const {
	if (this->hasLump(lumpIndex)) {
		const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
		return (this->stagedLumps.contains(lump) && this->stagedLumps.at(lump).first.uncompressedLength > 0)
			|| (this->header.lumps[lump].uncompressedLength > 0);
	}
	return false;
}

uint32_t BSP::getLumpVersion(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return 0;
	}
	const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	if (this->stagedLumps.contains(lump)) {
		return this->stagedLumps.at(lump).first.version;
	}
	return this->header.lumps[lump].version;
}

std::optional<std::vector<std::byte>> BSP::getLumpData(BSPLump lumpIndex, bool noDecompression) const {
	if (this->path.empty() || !this->hasLump(lumpIndex)) {
		return std::nullopt;
	}

	auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	std::vector<std::byte> lumpBytes;

	if (this->stagedLumps.contains(lump)) {
		lumpBytes = this->stagedLumps.at(lump).second;
	} else {
		FileStream reader{this->path};
		lumpBytes = reader
			.seek_in(this->header.lumps[lump].offset)
			.read_bytes(this->header.lumps[lump].length);
	}

	if (!noDecompression && this->isLumpCompressed(lumpIndex)) {
		return compression::decompressValveLZMA(lumpBytes);
	}
	return lumpBytes;
}

bool BSP::setLump(BSPLump lumpIndex, uint32_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	if (this->path.empty() || lumpIndex == BSPLump::UNKNOWN) {
		return false;
	}
	if (lumpIndex == BSPLump::GAME_LUMP) {
		// Game lump needs to be modified with other methods
		return false;
	}

	if (lumpIndex == BSPLump::PAKFILE) {
		// Paklump should use zip compression
		compressLevel = 0;
	}

	const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	if (compressLevel > 0) {
		auto compressedData = compression::compressValveLZMA(data, compressLevel);
		if (!compressedData) {
			return false;
		}
		this->stagedLumps[lump] = std::make_pair<Lump, std::vector<std::byte>>({
			.version = version, .uncompressedLength = static_cast<uint32_t>(data.size()),
		}, {compressedData->begin(), compressedData->end()});
	} else {
		this->stagedLumps[lump] = std::make_pair<Lump, std::vector<std::byte>>({
			.version = version, .uncompressedLength = 0,
		}, {data.begin(), data.end()});
	}
	return true;
}

bool BSP::setLump(uint32_t version, std::span<const BSPEntityKeyValues> data, uint8_t compressLevel) {
	if (version > 1) {
		return false;
	}
	std::string out;
	for (const auto& ent : data) {
		out += ent.bake(version == 1) + '\n';
	}
	if (out.empty()) {
		out += '\0';
	} else {
		out.back() = '\0';
	}
	return this->setLump(BSPLump::ENTITIES, version, {reinterpret_cast<const std::byte*>(out.data()), out.size()}, compressLevel);
}

bool BSP::isGameLumpCompressed(BSPGameLump::Signature signature) const {
	for (const auto& gameLump : this->stagedGameLumps) {
		if (gameLump.signature == signature) {
			return gameLump.isCompressed;
		}
	}
	return false;
}

uint16_t BSP::getGameLumpVersion(BSPGameLump::Signature signature) {
	for (const auto& gameLump : this->stagedGameLumps) {
		if (gameLump.signature == signature) {
			return gameLump.version;
		}
	}
	return 0;
}

std::optional<std::vector<std::byte>> BSP::getGameLumpData(BSPGameLump::Signature signature) const {
	for (const auto& gameLump : this->stagedGameLumps) {
		if (gameLump.signature == signature) {
			if (gameLump.isCompressed) {
				return compression::decompressValveLZMA(gameLump.data);
			}
			return gameLump.data;
		}
	}
	return std::nullopt;
}

bool BSP::setGameLump(BSPGameLump::Signature signature, uint16_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	BSPGameLump gameLump{
		.signature = signature,
		.isCompressed = compressLevel > 0,
		.version = version,
		.offset = 0,
		.uncompressedLength = static_cast<uint32_t>(data.size()),
	};

	if (compressLevel) {
		const auto compressedData = compression::compressValveLZMA(data, compressLevel);
		if (!compressedData) {
			return false;
		}
		gameLump.data = *compressedData;
	} else {
		gameLump.data = {data.begin(), data.end()};
	}

	this->stagedGameLumps.push_back(gameLump);
	return true;
}

void BSP::resetLump(BSPLump lumpIndex) {
	if (this->path.empty()) {
		return;
	}
	if (lumpIndex == BSPLump::UNKNOWN) {
		this->reset();
		return;
	}
	if (lumpIndex == BSPLump::GAME_LUMP) {
		this->stagedGameLumps = this->parseGameLumps(false);
		return;
	}
	const auto lump = static_cast<std::underlying_type_t<BSPLump>>(lumpIndex);
	if (this->stagedLumps.contains(lump)) {
		this->stagedLumps.erase(lump);
	}
}

void BSP::reset() {
	if (this->path.empty()) {
		return;
	}
	this->stagedVersion = this->header.version;
	this->stagedLumps.clear();
	this->stagedGameLumps = this->parseGameLumps(false);
	this->stagedMapRevision = this->header.mapRevision;
}

void BSP::createLumpPatchFile(BSPLump lumpIndex) const {
	if (this->path.empty()) {
		return;
	}

	auto lumpData = this->getLumpData(lumpIndex);
	if (!lumpData) {
		return;
	}

	const auto& [
		lumpOffset,
		lumpLength,
		lumpVersion,
		lumpUncompressedLength
	] = this->header.lumps.at(static_cast<std::underlying_type_t<BSPLump>>(lumpIndex));

	const auto fsPath = std::filesystem::path{this->path};
	const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string() + "_l_";
	int nonexistentNumber = 0;
	while (true) {
		if (!std::filesystem::exists(fsStem + std::to_string(nonexistentNumber) + ".lmp")) {
			break;
		}
		nonexistentNumber++;
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

bool BSP::setLumpFromPatchFile(const std::string& lumpFilePath) {
	if (this->path.empty()) {
		return false;
	}

	FileStream reader{lumpFilePath};
	if (!reader) {
		return false;
	}

	const auto offset = reader.read<uint32_t>();
	const auto index = reader.read<uint32_t>();
	const auto version = reader.read<uint32_t>();
	const auto length = reader.read<uint32_t>();
	if (index > BSP_LUMP_COUNT || offset == 0 || length == 0) {
		return false;
	}

	this->setLump(static_cast<BSPLump>(index), version, reader.seek_in(offset).read_bytes(length));
	return true;
}

bool BSP::bake(std::string_view outputPath) {
	if (this->path.empty()) {
		return false;
	}

	std::vector<std::byte> out;
	BufferStream stream{out};

	stream << BSP_SIGNATURE << this->stagedVersion;
	if (this->stagedVersion == 27) {
		// Contagion funny
		stream.write<uint32_t>(0);
	}

	const auto lumpsHeaderOffset = stream.tell();
	for (int i = 0; i < sizeof(Header::lumps); i++) {
		stream.write<uint8_t>(0);
	}

	stream << this->stagedMapRevision;

	for (auto i : BSP_LUMP_ORDER) {
		if (!this->hasLump(static_cast<BSPLump>(i))) {
			continue;
		}

		// Lumps are 4 byte aligned
		if (const auto padding = math::paddingForAlignment(4, stream.tell()); padding > 0) {
			for (int p = 0; p < padding; p++) {
				stream.write<uint8_t>(0);
			}
		}

		if (static_cast<BSPLump>(i) == BSPLump::GAME_LUMP && !this->stagedGameLumps.empty()) {
			const auto gameLumpOffset = stream.tell();

			bool oneOrMoreGameLumpCompressed = false;
			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.isCompressed) {
					oneOrMoreGameLumpCompressed = true;
					break;
				}
			}
			// NOLINTNEXTLINE(*-sizeof-container)
			auto gameLumpCurrentOffset = stream.tell() + sizeof(int32_t) + ((sizeof(BSPGameLump) - sizeof(BSPGameLump::data)) * (this->stagedGameLumps.size() + oneOrMoreGameLumpCompressed));
			stream.write<uint32_t>(this->stagedGameLumps.size() + oneOrMoreGameLumpCompressed);

			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.signature == 0) {
					break;
				}
				stream
					.write<uint32_t>(gameLump.signature)
					.write<uint16_t>(gameLump.isCompressed)
					.write<uint16_t>(gameLump.version)
					.write<uint32_t>(gameLumpCurrentOffset)
					.write<uint32_t>(gameLump.uncompressedLength);
				gameLumpCurrentOffset += gameLump.data.size();
			}
			if (oneOrMoreGameLumpCompressed) {
				stream
					.write<uint32_t>(0)
					.write<uint16_t>(0)
					.write<uint16_t>(0)
					.write<uint32_t>(0)
					.write<uint32_t>(0);
			}

			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.signature == 0) {
					break;
				}
				stream.write(gameLump.data);
			}

			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + (i * sizeof(Lump)));
			if (!this->isL4D2) {
				stream
					.write<uint32_t>(gameLumpOffset)
					.write<uint32_t>(curPos - gameLumpOffset)
					.write<uint32_t>(0);
			} else {
				stream
					.write<uint32_t>(0)
					.write<uint32_t>(gameLumpOffset)
					.write<uint32_t>(curPos - gameLumpOffset);
			}
			stream
				.write<uint32_t>(0)
				.seek_u(curPos);
			continue;
		}

		if (this->stagedLumps.contains(i)) {
			const auto& lumpPair = this->stagedLumps.at(i);
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + (i * sizeof(Lump)));
			if (!this->isL4D2) {
				stream
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpPair.second.size())
					.write<uint32_t>(lumpPair.first.version);
			} else {
				stream
					.write<uint32_t>(lumpPair.first.version)
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpPair.second.size());
			}
			stream
				.write<uint32_t>(lumpPair.first.uncompressedLength)
				.seek_u(curPos)
				.write(lumpPair.second);
			continue;
		}

		const auto data = this->getLumpData(static_cast<BSPLump>(i), true);
		if (data) {
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + (i * sizeof(Lump)));

			auto& lump = this->header.lumps[i];
			if (!this->isL4D2) {
				stream
					.write<uint32_t>(curPos)
					.write<uint32_t>(lump.length)
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(curPos)
					.write<uint32_t>(lump.length);
			}
			stream
				.write<uint32_t>(lump.uncompressedLength)
				.seek_u(curPos)
				.write(*data);
		} else {
			// We should never be here!
			SOURCEPP_DEBUG_BREAK;
		}
	}

	// Lumps are 4 byte aligned
	if (const auto padding = math::paddingForAlignment(4, stream.tell()); padding > 0) {
		for (int p = 0; p < padding; p++) {
			stream.write<uint8_t>(0);
		}
	}

	out.resize(stream.tell());
	if (!fs::writeFileBuffer(outputPath.empty() ? this->path : std::string{outputPath}, out)) {
		return false;
	}

	this->path = outputPath;
	this->stagedLumps.clear();
	return this->readHeader();
}

bool BSP::readHeader() {
	FileStream reader{this->path};
	if (!reader) {
		return false;
	}
	reader.seek_in(0);

	if (reader.read<uint32_t>() != BSP_SIGNATURE) {
		// File is not a BSP
		return false;
	}
	this->header.version = reader.read<uint32_t>();

	// Contagion funny
	if (this->header.version == 27) {
		reader.skip_in<uint32_t>();
	}

	for (auto& lump : this->header.lumps) {
		reader
			>> lump.offset
			>> lump.length
			>> lump.version
			>> lump.uncompressedLength;
	}

	// If no offsets are larger than 1024 (less than the size of the BSP header, but greater
	// than any lump version), it's probably a L4D2 BSP and those are lump versions!
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
	return true;
}

std::vector<BSPEntityKeyValues> BSP::parseEntities() const {
	using namespace sourcepp;

	const auto useEscapes = this->getLumpVersion(BSPLump::ENTITIES);
	if (useEscapes > 1) {
		return {};
	}

	auto data = this->getLumpData(BSPLump::ENTITIES);
	if (!data) {
		return {};
	}
	BufferStreamReadOnly stream{*data};

	std::vector<BSPEntityKeyValues> entities;

	try {
		while (true) {
			// Check for EOF - give 3 chars for extra breathing room
			if (stream.tell() >= stream.size() - 3) {
				return entities;
			}

			// Expect an opening brace
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() != '{') {
				break;
			}
			stream.skip<char>();

			auto& ent = entities.emplace_back();

			while (true) {
				// Check if the block is over
				parser::text::eatWhitespaceAndSingleLineComments(stream);
				if (stream.peek<char>() == '}') {
					stream.skip();
					break;
				}

				std::string key, value;

				// Read key
				{
					BufferStream keyStream{key};
					parser::text::readStringToBuffer(stream, keyStream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, useEscapes ? parser::text::DEFAULT_ESCAPE_SEQUENCES : parser::text::NO_ESCAPE_SEQUENCES);
					key.resize(keyStream.tell() - 1);
					parser::text::eatWhitespaceAndSingleLineComments(stream);
				}

				// Read value
				{
					BufferStream valueStream{value};
					parser::text::readStringToBuffer(stream, valueStream, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, useEscapes ? parser::text::DEFAULT_ESCAPE_SEQUENCES : parser::text::NO_ESCAPE_SEQUENCES);
					value.resize(valueStream.tell() - 1);
					parser::text::eatWhitespaceAndSingleLineComments(stream);
				}

				ent[key] = value;
			}
		}
	} catch (const std::overflow_error&) {
		return {};
	}
	return entities;
}

std::vector<BSPPlane> BSP::parsePlanes() const {
	return ::parseLumpContents<BSPPlane>(*this, BSPLump::PLANES);
}

std::vector<BSPTextureData> BSP::parseTextureData() const {
	return ::parseLumpContents<BSPTextureData>(*this, BSPLump::TEXDATA);
}

std::vector<BSPVertex> BSP::parseVertices() const {
	return ::parseLumpContents<BSPVertex>(*this, BSPLump::VERTEXES);
}

std::vector<BSPNode> BSP::parseNodes() const {
	return ::parseLumpContents<BSPNode>(*this, BSPLump::NODES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPNode>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(BSPLump::NODES); lumpVersion == 1) {
			stream.read(out, stream.size() / sizeof(BSPNode_v1));
		} else if (lumpVersion == 0) {
			::parseAndUpgrade<BSPNode_v0>(stream, out);
		}
	});
}

std::vector<BSPTextureInfo> BSP::parseTextureInfo() const {
	return ::parseLumpContents<BSPTextureInfo>(*this, BSPLump::TEXINFO);
}

std::vector<BSPFace> BSP::parseFaces() const {
	return ::parseLumpContents<BSPFace>(*this, BSPLump::FACES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPFace>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(BSPLump::FACES); lumpVersion == 2) {
			stream.read(out, stream.size() / sizeof(BSPFace_v2));
		} else if (lumpVersion == 1) {
			::parseAndUpgrade<BSPFace_v1>(stream, out);
		}
	});
}

std::vector<BSPEdge> BSP::parseEdges() const {
	return ::parseLumpContents<BSPEdge>(*this, BSPLump::EDGES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPEdge>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(BSPLump::EDGES); lumpVersion == 1) {
			stream.read(out, stream.size() / sizeof(BSPEdge_v1));
		} else if (lumpVersion == 0) {
			::parseAndUpgrade<BSPEdge_v0>(stream, out);
		}
	});
}

std::vector<BSPSurfEdge> BSP::parseSurfEdges() const {
	return ::parseLumpContents<BSPSurfEdge>(*this, BSPLump::SURFEDGES);
}

std::vector<BSPBrushModel> BSP::parseBrushModels() const {
	return ::parseLumpContents<BSPBrushModel>(*this, BSPLump::MODELS);
}

std::vector<BSPFace> BSP::parseOriginalFaces() const {
	return ::parseLumpContents<BSPFace>(*this, BSPLump::ORIGINALFACES, [](const BSP& bsp, BufferStreamReadOnly& stream, std::vector<BSPFace>& out) {
		// ORIGINALFACES lump version is always 0?
		if (const auto lumpVersion = bsp.getLumpVersion(BSPLump::FACES); lumpVersion == 2) {
			stream.read(out, stream.size() / sizeof(BSPFace_v2));
		} else if (lumpVersion == 1) {
			::parseAndUpgrade<BSPFace_v1>(stream, out);
		}
	});
}

std::vector<BSPGameLump> BSP::parseGameLumps(bool decompress) const {
	std::vector<BSPGameLump> lumps;

	auto gameLumpData = this->getLumpData(BSPLump::GAME_LUMP);
	if (!gameLumpData) {
		return lumps;
	}
	BufferStreamReadOnly stream{*gameLumpData};

	lumps.resize(stream.read<uint32_t>());
	for (auto& lump : lumps) {
		stream
			.read(lump.signature)
			.read(lump.isCompressed)
			.read(lump.version)
			.read(lump.offset)
			.read(lump.uncompressedLength);
	}

	// When one or more game lumps are compressed, the last entry
	// is empty except the offset field to calculate compressed
	// size. Game lumps are compressed individually
	for (uint32_t i = 0; i < lumps.size(); i++) {
		if (lumps[i].signature == 0) {
			break;
		}
		if (!lumps[i].isCompressed) {
			lumps[i].data = stream.read_bytes(lumps[i].uncompressedLength);
		} else {
			auto nextOffset = lumps[i + 1].offset;
			if (nextOffset == 0) {
				const auto id = static_cast<std::underlying_type_t<BSPLump>>(BSPLump::GAME_LUMP);
				nextOffset = this->header.lumps[id].offset + this->header.lumps[id].length;
			}
			if (!decompress) {
				lumps[i].data = stream.read_bytes(nextOffset - lumps[i].offset);
			} else {
				auto uncompressedData = compression::decompressValveLZMA(stream.read_bytes(nextOffset - lumps[i].offset));
				if (uncompressedData) {
					lumps[i].data = *uncompressedData;
				}
			}
		}
	}

	if (lumps.back().signature == 0) {
		lumps.pop_back();
	}
	return lumps;
}
