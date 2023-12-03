#include <studiomodelpp/studiomodelpp.h>

using namespace studiomodelpp;

bool StudioModel::open(const std::byte* mdlData, std::size_t mdlSize,
					   const std::byte* vtxData, std::size_t vtxSize,
                       const std::byte* vvdData, std::size_t vvdSize) {
	if (this->opened || !mdlData || !vtxData || !vvdData || !mdlSize || !vtxSize || !vvdSize) {
		return false;
	}
	if ((!this->mdl.open(mdlData, mdlSize) ||
		!this->vtx.open(vtxData, vtxSize, this->mdl.version, this->mdl.checksum)) ||
		!this->vvd.open(vvdData, vvdSize, this->mdl.version, this->mdl.checksum)) {
		return false;
	}
	this->opened = true;
	return true;
}

bool StudioModel::open(const unsigned char* mdlData, std::size_t mdlSize,
					   const unsigned char* vtxData, std::size_t vtxSize,
                       const unsigned char* vvdData, std::size_t vvdSize) {
	return this->open(reinterpret_cast<const std::byte*>(mdlData), mdlSize,
					  reinterpret_cast<const std::byte*>(vtxData), vtxSize,
					  reinterpret_cast<const std::byte*>(vvdData), vvdSize);
}

bool StudioModel::open(const std::vector<std::byte>& mdlData,
					   const std::vector<std::byte>& vtxData,
                       const std::vector<std::byte>& vvdData) {
	return this->open(mdlData.data(), mdlData.size(),
					  vtxData.data(), vtxData.size(),
					  vvdData.data(), vvdData.size());
}

bool StudioModel::open(const std::vector<unsigned char>& mdlData,
					   const std::vector<unsigned char>& vtxData,
                       const std::vector<unsigned char>& vvdData) {
	return this->open(mdlData.data(), mdlData.size(),
	                  vtxData.data(), vtxData.size(),
	                  vvdData.data(), vvdData.size());
}

StudioModel::operator bool() const {
	return this->opened;
}
