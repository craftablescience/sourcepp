// ReSharper disable CppRedundantParentheses

#include <bsppp/format/VBSP.h>

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
[[nodiscard]] std::vector<T> parseLumpContents(const VBSP& bsp, VBSPLump lump, void(*callback)(const VBSP&, BufferStreamReadOnly&, std::vector<T>&) = [](const VBSP&, BufferStreamReadOnly& stream, std::vector<T>& out) {
	stream.read(out, stream.size() / sizeof(T));
}) {
	auto data = bsp.getLumpData(lump);
	if (!data) {
		return {};
	}

	BufferStreamReadOnly stream{*data};
	stream.set_big_endian(bsp.isConsole());
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

VBSP::VBSP(std::string path_, bool loadPatchFiles)
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
		const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string();

		for (int i = 0; ; i++) {
			auto patchFilePath = std::format("{}_l_{}.lmp", fsStem, i);
			if (!std::filesystem::exists(patchFilePath)) {
				break;
			}
			this->setLumpFromPatchFile(patchFilePath);
		}
	}
}

VBSP::operator bool() const {
	return !this->path.empty();
}

VBSP VBSP::create(std::string path, uint32_t version, uint32_t mapRevision) {
	{
		FileStream writer{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		writer << VBSP_SIGNATURE << version;
		if (version == 27) {
			writer.write<uint32_t>(0);
		}
		for (int i = 0; i < VBSP_LUMP_COUNT; i++) {
			writer
				.write<uint32_t>(0)
				.write<uint32_t>(0)
				.write<uint32_t>(0)
				.write<uint32_t>(0);
		}
		writer << mapRevision;
	}
	return VBSP{std::move(path)};
}

uint32_t VBSP::getVersion() const {
	return this->stagedVersion;
}

void VBSP::setVersion(uint32_t version) {
	this->stagedVersion = version;
}

uint32_t VBSP::getMapRevision() const {
	return this->stagedMapRevision;
}

void VBSP::setMapRevision(uint32_t mapRevision) {
	this->stagedMapRevision = mapRevision;
}

bool VBSP::isL4D2() const {
	return this->l4d2;
}

void VBSP::setL4D2(bool isL4D2) {
	this->l4d2 = isL4D2;
}

bool VBSP::isConsole() const {
	return this->console;
}

void VBSP::setConsole(bool isConsole) {
	this->console = isConsole;
}

bool VBSP::hasLump(VBSPLump lumpIndex) const {
	if (this->path.empty()) {
		return false;
	}
	const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
	return (this->stagedLumps.contains(lump) && this->stagedLumps.at(lump).first.length != 0 && this->stagedLumps.at(lump).first.offset != 0)
		|| (this->header.lumps[lump].length != 0 && this->header.lumps[lump].offset != 0);
}

bool VBSP::isLumpCompressed(VBSPLump lumpIndex) const {
	if (this->hasLump(lumpIndex)) {
		const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
		return (this->stagedLumps.contains(lump) && this->stagedLumps.at(lump).first.uncompressedLength > 0) || this->header.lumps[lump].uncompressedLength > 0;
	}
	return false;
}

uint32_t VBSP::getLumpVersion(VBSPLump lumpIndex) const {
	if (this->path.empty()) {
		return 0;
	}
	const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
	if (this->stagedLumps.contains(lump)) {
		return this->stagedLumps.at(lump).first.version;
	}
	return this->header.lumps[lump].version;
}

std::optional<std::vector<std::byte>> VBSP::getLumpData(VBSPLump lumpIndex, bool noDecompression) const {
	if (this->path.empty() || !this->hasLump(lumpIndex)) {
		return std::nullopt;
	}

	const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
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

bool VBSP::setLump(VBSPLump lumpIndex, uint32_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	if (this->path.empty() || lumpIndex == VBSPLump::UNKNOWN) {
		return false;
	}
	if (lumpIndex == VBSPLump::GAME_LUMP) {
		// Game lump needs to be modified with other methods
		return false;
	}

	if (lumpIndex == VBSPLump::PAKFILE) {
		// Paklump should use zip compression
		compressLevel = 0;
	}

	const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
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

bool VBSP::setLump(uint32_t version, std::span<const BSPEntityKeyValues> data, uint8_t compressLevel) {
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
	return this->setLump(VBSPLump::ENTITIES, version, {reinterpret_cast<const std::byte*>(out.data()), out.size()}, compressLevel);
}

bool VBSP::isGameLumpCompressed(VBSPGameLump::Signature signature) const {
	for (const auto& gameLump : this->stagedGameLumps) {
		if (gameLump.signature == signature) {
			return gameLump.isCompressed;
		}
	}
	return false;
}

uint16_t VBSP::getGameLumpVersion(VBSPGameLump::Signature signature) const {
	for (const auto& gameLump : this->stagedGameLumps) {
		if (gameLump.signature == signature) {
			return gameLump.version;
		}
	}
	return 0;
}

std::optional<std::vector<std::byte>> VBSP::getGameLumpData(VBSPGameLump::Signature signature) const {
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

bool VBSP::setGameLump(VBSPGameLump::Signature signature, uint16_t version, std::span<const std::byte> data, uint8_t compressLevel) {
	VBSPGameLump gameLump{
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

void VBSP::resetLump(VBSPLump lumpIndex) {
	if (this->path.empty()) {
		return;
	}
	if (lumpIndex == VBSPLump::UNKNOWN) {
		this->reset();
		return;
	}
	if (lumpIndex == VBSPLump::GAME_LUMP) {
		this->stagedGameLumps = this->parseGameLumps(false);
		return;
	}
	const auto lump = static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex);
	if (this->stagedLumps.contains(lump)) {
		this->stagedLumps.erase(lump);
	}
}

void VBSP::reset() {
	if (this->path.empty()) {
		return;
	}
	this->stagedVersion = this->header.version;
	this->stagedLumps.clear();
	this->stagedGameLumps = this->parseGameLumps(false);
	this->stagedMapRevision = this->header.mapRevision;
}

void VBSP::createLumpPatchFile(VBSPLump lumpIndex) const {
	if (this->path.empty()) {
		return;
	}

	const auto lumpData = this->getLumpData(lumpIndex);
	if (!lumpData) {
		return;
	}

	const auto& [
		lumpOffset,
		lumpLength,
		lumpVersion,
		lumpUncompressedLength
	] = this->header.lumps.at(static_cast<std::underlying_type_t<VBSPLump>>(lumpIndex));

	const auto fsPath = std::filesystem::path{this->path};
	const auto fsStem = (fsPath.parent_path() / fsPath.stem()).string();
	int nonexistentNumber = 0;
	while (true) {
		if (!std::filesystem::exists(std::format("{}_l_{}.lmp", fsStem, nonexistentNumber))) {
			break;
		}
		nonexistentNumber++;
	}

	FileStream writer{std::format("{}_l_{}.lmp", fsStem, nonexistentNumber), FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	writer
		.seek_out(0)
		.write<int32_t>(sizeof(int32_t) * 5)
		.write(lumpIndex)
		.write(lumpVersion)
		.write(lumpLength)
		.write(this->header.mapRevision)
		.write(*lumpData);
}

bool VBSP::setLumpFromPatchFile(const std::string& lumpFilePath) {
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
	if (index > VBSP_LUMP_COUNT || offset == 0 || length == 0) {
		return false;
	}

	this->setLump(static_cast<VBSPLump>(index), version, reader.seek_in(offset).read_bytes(length));
	return true;
}

bool VBSP::bake(std::string_view outputPath) {
	if (this->path.empty()) {
		return false;
	}

	std::vector<std::byte> out;
	BufferStream stream{out};
	stream.set_big_endian(this->console);

	stream << VBSP_SIGNATURE << this->stagedVersion;
	if (this->stagedVersion == 27) {
		// Contagion funny
		stream.write<uint32_t>(0);
	}

	const auto lumpsHeaderOffset = stream.tell();
	for (int i = 0; i < sizeof(Header::lumps); i++) {
		stream.write<uint8_t>(0);
	}

	stream << this->stagedMapRevision;

	for (auto i : VBSP_LUMP_ORDER) {
		if (!this->hasLump(static_cast<VBSPLump>(i))) {
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(Lump));

			const auto& lump = this->header.lumps[i];
			if (!this->l4d2) {
				stream
					.write<uint32_t>(0)
					.write<uint32_t>(0)
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(0)
					.write<uint32_t>(0);
			}
			stream
				.write<uint32_t>(0)
				.seek_u(curPos);
			continue;
		}

		// Lumps are 4 byte aligned
		if (const auto padding = math::paddingForAlignment(4, stream.tell()); padding > 0) {
			stream.pad(padding);
		}

		if (static_cast<VBSPLump>(i) == VBSPLump::GAME_LUMP && !this->stagedGameLumps.empty()) {
			const auto gameLumpOffset = stream.tell();

			bool oneOrMoreGameLumpCompressed = false;
			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.isCompressed) {
					oneOrMoreGameLumpCompressed = true;
					break;
				}
			}
			// NOLINTNEXTLINE(*-sizeof-container)
			auto gameLumpCurrentOffset = !this->console * stream.tell() + sizeof(int32_t) + ((sizeof(VBSPGameLump) - sizeof(VBSPGameLump::data)) * (this->stagedGameLumps.size() + oneOrMoreGameLumpCompressed));
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
					.write<uint32_t>(gameLumpCurrentOffset)
					.write<uint32_t>(0);
			}

			for (const auto& gameLump : this->stagedGameLumps) {
				if (gameLump.signature == 0) {
					break;
				}
				stream.write(gameLump.data);
			}

			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(Lump));
			if (!this->l4d2) {
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
			const auto&[lump, lumpData] = this->stagedLumps.at(i);
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(Lump));
			if (!this->l4d2) {
				stream
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpData.size())
					.write<uint32_t>(lump.version);
			} else {
				stream
					.write<uint32_t>(lump.version)
					.write<uint32_t>(curPos)
					.write<uint32_t>(lumpData.size());
			}
			stream
				.write<uint32_t>(lump.uncompressedLength)
				.seek_u(curPos)
				.write(lumpData);
			continue;
		}

		if (const auto data = this->getLumpData(static_cast<VBSPLump>(i), true)) {
			const auto curPos = stream.tell();
			stream.seek_u(lumpsHeaderOffset + i * sizeof(Lump));

			const auto& lump = this->header.lumps[i];
			if (!this->l4d2) {
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
		stream.pad(padding);
	}

	out.resize(stream.size());
	if (!fs::writeFileBuffer(outputPath.empty() ? this->path : std::string{outputPath}, out)) {
		return false;
	}

	this->path = outputPath;
	this->stagedLumps.clear();
	return this->readHeader();
}

bool VBSP::readHeader() {
	FileStream reader{this->path};
	if (!reader) {
		return false;
	}
	reader.seek_in(0);

	if (const auto signature = reader.read<uint32_t>(); signature != VBSP_SIGNATURE && signature != VBSP_CONSOLE_SIGNATURE) {
		// File is not a BSP
		return false;
	} else if (signature == VBSP_CONSOLE_SIGNATURE) {
		this->console = true;
		reader.set_big_endian(true);
	}
	this->header.version = reader.read<uint32_t>();

	// Contagion funny
	if (this->header.version == 27) {
		reader.skip_in<uint32_t>();
	}

	for (auto& [offset, length, version, uncompressedLength] : this->header.lumps) {
		reader
			>> offset
			>> length
			>> version
			>> uncompressedLength;
	}

	// If no offsets are larger than 1024 (less than the size of the BSP header, but greater
	// than any lump version), it's probably a L4D2 BSP and those are lump versions!
	if (this->header.version == 21) {
		int i;
		for (i = 0; i < VBSP_LUMP_COUNT; i++) {
			if (this->header.lumps[i].offset > 1024) {
				break;
			}
		}
		this->l4d2 = i == VBSP_LUMP_COUNT;
		if (this->l4d2) {
			// Swap fields around
			for (i = 0; i < VBSP_LUMP_COUNT; i++) {
				std::swap(this->header.lumps[i].offset, this->header.lumps[i].version);
				std::swap(this->header.lumps[i].offset, this->header.lumps[i].length);
			}
		}
	}

	reader >> this->header.mapRevision;
	return true;
}

std::vector<BSPEntityKeyValues> VBSP::parseEntities() const {
	using namespace sourcepp;

	const auto useEscapes = this->getLumpVersion(VBSPLump::ENTITIES);
	if (useEscapes > 1) {
		return {};
	}

	auto data = this->getLumpData(VBSPLump::ENTITIES);
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
					parser::text::readStringToBuffer(stream, keyStream, {parser::text::DEFAULT_QUOTES}, parser::text::getDefaultEscapeSequencesOrNone(useEscapes));
					key.resize(keyStream.tell() - 1);
					parser::text::eatWhitespaceAndSingleLineComments(stream);
				}

				// Read value
				{
					BufferStream valueStream{value};
					parser::text::readStringToBuffer(stream, valueStream, {parser::text::DEFAULT_QUOTES}, parser::text::getDefaultEscapeSequencesOrNone(useEscapes));
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

std::vector<VBSPPlane> VBSP::parsePlanes() const {
	return ::parseLumpContents<VBSPPlane>(*this, VBSPLump::PLANES);
}

std::vector<VBSPTextureData> VBSP::parseTextureData() const {
	return ::parseLumpContents<VBSPTextureData>(*this, VBSPLump::TEXDATA);
}

std::vector<VBSPVertex> VBSP::parseVertices() const {
	return ::parseLumpContents<VBSPVertex>(*this, VBSPLump::VERTEXES);
}

std::vector<VBSPNode> VBSP::parseNodes() const {
	return ::parseLumpContents<VBSPNode>(*this, VBSPLump::NODES, [](const VBSP& bsp, BufferStreamReadOnly& stream, std::vector<VBSPNode>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(VBSPLump::NODES); lumpVersion == 1) {
			stream.read(out, stream.size() / sizeof(VBSPNode_v1));
		} else if (lumpVersion == 0) {
			::parseAndUpgrade<VBSPNode_v0>(stream, out);
		}
	});
}

std::vector<VBSPTextureInfo> VBSP::parseTextureInfo() const {
	return ::parseLumpContents<VBSPTextureInfo>(*this, VBSPLump::TEXINFO);
}

std::vector<VBSPFace> VBSP::parseFaces() const {
	return ::parseLumpContents<VBSPFace>(*this, VBSPLump::FACES, [](const VBSP& bsp, BufferStreamReadOnly& stream, std::vector<VBSPFace>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(VBSPLump::FACES); lumpVersion == 2) {
			stream.read(out, stream.size() / sizeof(VBSPFace_v2));
		} else if (lumpVersion == 1) {
			::parseAndUpgrade<VBSPFace_v1>(stream, out);
		}
	});
}

std::vector<VBSPEdge> VBSP::parseEdges() const {
	return ::parseLumpContents<VBSPEdge>(*this, VBSPLump::EDGES, [](const VBSP& bsp, BufferStreamReadOnly& stream, std::vector<VBSPEdge>& out) {
		if (const auto lumpVersion = bsp.getLumpVersion(VBSPLump::EDGES); lumpVersion == 1) {
			stream.read(out, stream.size() / sizeof(VBSPEdge_v1));
		} else if (lumpVersion == 0) {
			::parseAndUpgrade<VBSPEdge_v0>(stream, out);
		}
	});
}

std::vector<VBSPSurfEdge> VBSP::parseSurfEdges() const {
	return ::parseLumpContents<VBSPSurfEdge>(*this, VBSPLump::SURFEDGES);
}

std::vector<VBSPBrushModel> VBSP::parseBrushModels() const {
	return ::parseLumpContents<VBSPBrushModel>(*this, VBSPLump::MODELS);
}

std::vector<VBSPFace> VBSP::parseOriginalFaces() const {
	return ::parseLumpContents<VBSPFace>(*this, VBSPLump::ORIGINALFACES, [](const VBSP& bsp, BufferStreamReadOnly& stream, std::vector<VBSPFace>& out) {
		// ORIGINALFACES lump version is always 0?
		if (const auto lumpVersion = bsp.getLumpVersion(VBSPLump::FACES); lumpVersion == 2) {
			stream.read(out, stream.size() / sizeof(VBSPFace_v2));
		} else if (lumpVersion == 1) {
			::parseAndUpgrade<VBSPFace_v1>(stream, out);
		}
	});
}

std::vector<VBSPGameLump> VBSP::parseGameLumps(bool decompress) const {
	std::vector<VBSPGameLump> lumps;

	auto gameLumpData = this->getLumpData(VBSPLump::GAME_LUMP);
	if (!gameLumpData) {
		return lumps;
	}
	BufferStreamReadOnly stream{*gameLumpData};
	stream.set_big_endian(this->console);

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
			uint32_t nextOffset = 0;
			if (i + 1 < lumps.size()) {
				nextOffset = lumps[i + 1].offset;
			}
			if (nextOffset == 0) {
				static constexpr auto gameLumpID = static_cast<std::underlying_type_t<VBSPLump>>(VBSPLump::GAME_LUMP);
				nextOffset = this->header.lumps[gameLumpID].offset + this->header.lumps[gameLumpID].length;
			}
			if (!decompress) {
				lumps[i].data = stream.read_bytes(nextOffset - lumps[i].offset);
			} else {
				if (auto uncompressedData = compression::decompressValveLZMA(stream.read_bytes(nextOffset - lumps[i].offset))) {
					lumps[i].data = *uncompressedData;
				}
			}
		}
	}

	if (!lumps.empty() && lumps.back().signature == 0) {
		lumps.pop_back();
	}
	return lumps;
}
