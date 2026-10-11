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
	this->opened = this->mdl.open(mdlData, mdlSize) && this->vtx.open(vtxData, vtxSize, this->mdl) && this->vvd.open(vvdData, vvdSize, this->mdl);
	return this->opened;
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
	if (this->vvd.fixups.empty()) {
		std::ranges::copy(this->vvd.vertices, std::back_inserter(model.vertices));
	} else {
		for (const auto& [LOD, sourceVertexID, vertexCount] : this->vvd.fixups) {
			if (LOD < currentLOD) {
				continue;
			}
			std::span fixupVertices{this->vvd.vertices.begin() + sourceVertexID, static_cast<std::span<const VVD::Vertex>::size_type>(vertexCount)};
			std::ranges::copy(fixupVertices, std::back_inserter(model.vertices));
		}
	}

	for (int bodyPartIndex = 0; bodyPartIndex < this->mdl.bodyParts.size(); bodyPartIndex++) {
		auto& mdlBodyPart = this->mdl.bodyParts.at(bodyPartIndex);
		const auto& [vtxModels] = this->vtx.bodyParts.at(bodyPartIndex);

		for (int modelIndex = 0; modelIndex < mdlBodyPart.models.size(); modelIndex++) {
			auto& mdlModel = mdlBodyPart.models.at(modelIndex);
			const auto& [vtxModelLODs] = vtxModels.at(modelIndex);

			if (mdlModel.verticesCount == 0) {
				continue;
			}

			for (int meshIndex = 0; meshIndex < mdlModel.meshes.size(); meshIndex++) {
				auto& mdlMesh = mdlModel.meshes.at(meshIndex);
				const auto& [vtxStripGroups, vtxMeshFlags] = vtxModelLODs.at(currentLOD).meshes.at(meshIndex);

				std::vector<uint16_t> indices;
				for (const auto& stripGroup : vtxStripGroups) {
					for (const auto& strip : stripGroup.strips) {
						const auto addIndex = [&indices, mdlMesh, mdlModel, stripGroup](int index) {
							indices.push_back(stripGroup.vertices.at(index).meshVertexID + mdlMesh.verticesOffset + mdlModel.verticesOffset);
						};

						// Remember to flip the winding order
						if (strip.flags & VTX::Strip::FLAG_IS_TRILIST) {
							for (int i = 0; i < strip.indices.size(); i += 3) {
								addIndex(strip.indices[i+0]);
								addIndex(strip.indices[i+2]);
								addIndex(strip.indices[i+1]);
							}
						} else /* if (strip.flags & VTX::Strip::FLAG_IS_TRISTRIP) */ {
							for (int i = 0; i < strip.indices.size() - 2; i++) {
								if (i & 1) {
									addIndex(strip.indices[i+0]);
									addIndex(strip.indices[i+2]);
									addIndex(strip.indices[i+1]);
								} else {
									addIndex(strip.indices[i+1]);
									addIndex(strip.indices[i+2]);
									addIndex(strip.indices[i+0]);
								}
							}
						}
					}
				}

				model.meshes.push_back({
					.indices = std::move(indices),
					.materialIndex = mdlMesh.material,
				});
			}
		}
	}

	return model;
}
