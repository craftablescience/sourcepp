#include <mdlpp/mdlpp.h>

#include <algorithm>
#include <span>
#include <utility>

using namespace mdlpp;
using namespace sourcepp;

bool StudioModel::open(const std::byte* mdlData, std::size_t mdlSize,
					   const std::byte* vtxData, std::size_t vtxSize,
                       const std::byte* vvdData, std::size_t vvdSize) {
	if (this->opened || !mdlData || !vtxData || !vvdData || !mdlSize || !vtxSize || !vvdSize) {
		return false;
	}
	if ((!this->mdl.open(mdlData, mdlSize) ||
		!this->vtx.open(vtxData, vtxSize, this->mdl)) ||
		!this->vvd.open(vvdData, vvdSize, this->mdl)) {
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

BakedModel StudioModel::processModelData(int currentLOD) const {
	BakedModel model;

	// According to my limited research, vertices stay constant (ignoring LOD fixups) but indices vary with LOD level
	static constexpr auto convertVertex = [](const VVD::Vertex& vertex) {
		return BakedModel::Vertex{vertex.position, vertex.normal, vertex.uv};
	};
	if (this->vvd.fixups.empty()) {
		std::transform(this->vvd.vertices.begin(), this->vvd.vertices.end(), std::back_inserter(model.vertices), convertVertex);
	} else {
		for (const auto& [LOD, sourceVertexID, vertexCount] : this->vvd.fixups) {
			if (LOD < currentLOD) {
				continue;
			}
			std::span fixupVertices{this->vvd.vertices.begin() + sourceVertexID, static_cast<std::span<const VVD::Vertex>::size_type>(vertexCount)};
			std::transform(fixupVertices.begin(), fixupVertices.end(), std::back_inserter(model.vertices), convertVertex);
		}
	}

	for (int bodyPartIndex = 0; bodyPartIndex < this->mdl.bodyParts.size(); bodyPartIndex++) {
		auto& mdlBodyPart = this->mdl.bodyParts.at(bodyPartIndex);
		auto& vtxBodyPart = this->vtx.bodyParts.at(bodyPartIndex);

		for (int modelIndex = 0; modelIndex < mdlBodyPart.models.size(); modelIndex++) {
			auto& mdlModel = mdlBodyPart.models.at(modelIndex);
			auto& vtxModel = vtxBodyPart.models.at(modelIndex);

			if (mdlModel.verticesCount == 0) {
				continue;
			}

			for (int meshIndex = 0; meshIndex < mdlModel.meshes.size(); meshIndex++) {
				auto& mdlMesh = mdlModel.meshes.at(meshIndex);
				auto& vtxMesh = vtxModel.modelLODs.at(currentLOD).meshes.at(meshIndex);

				std::vector<uint16_t> indices;
				for (const auto& stripGroup : vtxMesh.stripGroups) {
					for (const auto& strip : stripGroup.strips) {
						const auto addIndex = [&indices, mdlMesh, mdlModel, stripGroup](int index) {
							indices.push_back(stripGroup.vertices.at(index).meshVertexID + mdlMesh.verticesOffset + mdlModel.verticesOffset);
						};

						// Remember to flip the winding order
						if (strip.flags & VTX::Strip::FLAG_IS_TRILIST) {
							for (int i = 0; i < strip.indices.size(); i += 3) {
								addIndex(strip.indices[ i ]);
								addIndex(strip.indices[i+2]);
								addIndex(strip.indices[i+1]);
							}
						} else {
							for (auto i = strip.indices.size() - 1; i >= 2; i -= 3) {
								addIndex(strip.indices[ i ]);
								addIndex(strip.indices[i-2]);
								addIndex(strip.indices[i-1]);
							}
						}
					}
				}

				model.meshes.push_back({std::move(indices), mdlMesh.material});
			}
		}
	}

	return model;
}
