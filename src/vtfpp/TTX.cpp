#include <vtfpp/TTX.h>

#include <BufferStream.h>
#include <miniz.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vtfpp;

TTX::TTX(VTF&& vtf_)
		: vtf(std::move(vtf_))
		, aspectRatioType(3) {
	this->mipFlags.resize(this->vtf.getMipCount());

	this->opened = true;
}

TTX::TTX(std::span<const std::byte> tthData, std::span<const std::byte> ttzData) {
	BufferStreamReadOnly header{tthData.data(), tthData.size()};

	if (header.read<uint32_t>() != TTH_SIGNATURE) {
		return;
	}

	header >> this->majorVersion >> this->minorVersion;
	if (this->majorVersion != 1 || this->minorVersion != 0) {
		return;
	}

	const auto mipCount = header.read<uint8_t>();
	header >> this->aspectRatioType;
	const auto vtfChunkSize = header.read<uint32_t>();
	if (mipCount > 0) {
		header.read(this->mipFlags, mipCount);
	}
	mz_ulong ttzUncompressedSize = header.read<uint32_t>() - vtfChunkSize;
	const mz_ulong ttzCompressedSize = header.read<uint32_t>();

	std::vector<std::byte> vtfData;
	BufferStream vtfStream{vtfData};
	vtfStream << header.read_bytes(vtfChunkSize);
	if (ttzCompressedSize > 0) {
		if (ttzCompressedSize > ttzData.size()) {
			return;
		}
		std::vector<std::byte> additionalDataUncompressed(ttzUncompressedSize);
		if (mz_uncompress(reinterpret_cast<unsigned char*>(additionalDataUncompressed.data()), &ttzUncompressedSize, reinterpret_cast<const unsigned char*>(ttzData.data()), ttzCompressedSize) != MZ_OK) {
			return;
		}
		vtfStream << additionalDataUncompressed;
	}
	vtfData.resize(vtfStream.size());
	this->vtf = VTF{vtfData};
	if (!this->vtf) {
		return;
	}

	this->opened = true;
}

TTX::TTX(const std::string& tthPath, const std::string& ttzPath)
		: TTX(fs::readFileBuffer(tthPath), fs::readFileBuffer(ttzPath)) {}

TTX::operator bool() const {
	return this->opened;
}

uint8_t TTX::getMajorVersion() const {
	return this->majorVersion;
}

uint8_t TTX::getMinorVersion() const {
	return this->minorVersion;
}

void TTX::setVersion(uint8_t newMajorVersion, uint8_t newMinorVersion) {
	this->majorVersion = newMajorVersion;
	this->minorVersion = newMinorVersion;
}

void TTX::setMajorVersion(uint8_t newMajorVersion) {
	this->majorVersion = newMajorVersion;
}

void TTX::setMinorVersion(uint8_t newMinorVersion) {
	this->minorVersion = newMinorVersion;
}

uint8_t TTX::getAspectRatioType() const {
	return this->aspectRatioType;
}

void TTX::setAspectRatioType(uint8_t newAspectRatioType) {
	this->aspectRatioType = newAspectRatioType;
}

const std::vector<uint64_t>& TTX::getMipFlags() const {
	return this->mipFlags;
}

std::vector<uint64_t>& TTX::getMipFlags() {
	return this->mipFlags;
}

const VTF& TTX::getVTF() const {
	return this->vtf;
}

VTF& TTX::getVTF() {
	return this->vtf;
}

int16_t TTX::getCompressionLevel() const {
	return this->compressionLevel;
}

void TTX::setCompressionLevel(int16_t newCompressionLevel) {
	this->compressionLevel = newCompressionLevel;
}

std::pair<std::vector<std::byte>, std::vector<std::byte>> TTX::bake() const {
	std::pair<std::vector<std::byte>, std::vector<std::byte>> data;
	BufferStream streamTTH{data.first};

	// TTH
	streamTTH
		<< TTH_SIGNATURE
		<< this->majorVersion
		<< this->minorVersion
		<< static_cast<uint8_t>(this->mipFlags.size())
		<< this->aspectRatioType;

	// VTF data
	const auto vtfData = this->vtf.bake();
	if (vtfData.empty()) {
		return {};
	}
	uint32_t vtfChunkLength;
	{
		BufferStreamReadOnly streamVTF{vtfData.data(), vtfData.size()};
		vtfChunkLength = streamVTF.skip<uint32_t>(3).read<uint32_t>();
		if (this->vtf.hasThumbnailData()) {
			vtfChunkLength += this->vtf.getThumbnailDataRaw().size();
		}
	}
	const auto vtfTheRestLength = static_cast<uint32_t>(vtfData.size() - vtfChunkLength);
	streamTTH << vtfChunkLength << this->mipFlags << static_cast<uint32_t>(vtfData.size());
	const auto vtfTTZLengthPos = streamTTH.tell();
	streamTTH.write<uint32_t>(0).write(vtfData.data(), vtfChunkLength);

	// TTZ (copied from VTF ::compressData)
	mz_ulong compressedSize = mz_compressBound(vtfTheRestLength);
	data.second.resize(compressedSize);
	int status = MZ_OK;
	while ((status = mz_compress2(reinterpret_cast<unsigned char*>(data.second.data()), &compressedSize, reinterpret_cast<const unsigned char*>(vtfData.data() + vtfChunkLength), vtfTheRestLength, this->compressionLevel)) == MZ_BUF_ERROR) {
		compressedSize *= 2;
		data.second.resize(compressedSize);
	}
	if (status != MZ_OK) {
		return {};
	}
	data.second.resize(compressedSize);

	// Fix header
	streamTTH.seek_u(vtfTTZLengthPos).write<uint32_t>(data.second.size());
	data.first.resize(streamTTH.size());

	return data;
}

bool TTX::bake(const std::string& tthPath, const std::string& ttzPath) const {
	const auto [tthData, ttzData] = this->bake();
	const bool tth = fs::writeFileBuffer(tthPath, tthData);
	if (!ttzData.empty()) {
		return fs::writeFileBuffer(ttzPath, ttzData) && tth;
	}
	return tth;
}
