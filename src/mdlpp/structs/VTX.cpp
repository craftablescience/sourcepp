#include <mdlpp/structs/VTX.h>

#include <BufferStream.h>
#include <sourcepp/parser/Binary.h>

using namespace mdlpp::VTX;
using namespace sourcepp;

bool VTX::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStreamReadOnly stream{data, size};

	if (stream.read(this->version); this->version != 7) {
		return false;
	}

	stream
		.read(this->vertexCacheSize)
		.read(this->maxBonesPerStrip)
		.read(this->maxBonesPerTriangle)
		.read(this->maxBonesPerVertex);

	if (stream.read<int32_t>() != mdl.checksum) {
		return false;
	}

	stream.read(this->numLODs);

	const auto materialReplacementListOffset = stream.read<int32_t>();

	const auto bodyPartCount = stream.read<int32_t>();
	const auto bodyPartOffset = stream.read<int32_t>();

	if (materialReplacementListOffset > 0) {
		stream.seek(materialReplacementListOffset);

		for (int i = 0; i < this->numLODs; i++) {
			const auto replacementListPos = materialReplacementListOffset + i * (sizeof(int32_t) * 2);
			stream.seek_u(replacementListPos);

			auto& replacementList = this->materialReplacementLists.emplace_back();

			const auto numReplacements = stream.read<int32_t>();
			const auto replacementOffset = stream.read<int32_t>();

			if (numReplacements > 0 && replacementOffset > 0) {
				for (int j = 0; j < numReplacements; j++) {
					const auto replacementPos = replacementOffset + j * (sizeof(int16_t) + sizeof(int32_t));
					stream.seek_u(replacementListPos + replacementPos);

					auto& replacement = replacementList.replacements.emplace_back();
					stream.read(replacement.materialID);

					parser::binary::readStringAtOffset(stream, replacement.replacementMaterialName, std::ios::cur, 6);
				}
			}
		}
	}

	for (int i = 0; i < bodyPartCount; i++) {
		const auto bodyPartPos = bodyPartOffset + i * ((sizeof(int32_t) * 2));
		stream.seek_u(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		const auto modelCount = stream.read<int32_t>();
		const auto modelOffset = stream.read<int32_t>();

		for (int j = 0; j < modelCount; j++) {
			const auto modelPos = modelOffset + j * (sizeof(int32_t) * 2);
			stream.seek_u(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			const auto modelLODCount = stream.read<int32_t>();
			const auto modelLODOffset = stream.read<int32_t>();

			for (int k = 0; k < modelLODCount; k++) {
				const auto modelLODPos = modelLODOffset + k * (sizeof(int32_t) * 2 + sizeof(float));
				stream.seek_u(bodyPartPos + modelPos + modelLODPos);

				auto& modelLOD = model.modelLODs.emplace_back();

				const auto meshCount = stream.read<int32_t>();
				const auto meshOffset = stream.read<int32_t>();

				stream.read(modelLOD.switchDistance);

				for (int l = 0; l < meshCount; l++) {
					const auto meshPos = meshOffset + l * (sizeof(int32_t) * 2 + sizeof(Mesh::Flags));
					stream.seek_u(bodyPartPos + modelPos + modelLODPos + meshPos);

					auto& mesh = modelLOD.meshes.emplace_back();

					const auto stripGroupCount = stream.read<int32_t>();
					const auto stripGroupOffset = stream.read<int32_t>();

					stream.read(mesh.flags);

					for (int m = 0; m < stripGroupCount; m++) {
						int stripGroupNumInts = 6;
						if (mdl.version >= 49) {
							stripGroupNumInts += 2;
						}
						const auto stripGroupPos = stripGroupOffset + m * (sizeof(int32_t) * stripGroupNumInts + sizeof(StripGroup::Flags));
						stream.seek_u(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos);

						auto& stripGroup = mesh.stripGroups.emplace_back();

						const auto vertexCount = stream.read<int32_t>();
						const auto vertexOffset = stream.read<int32_t>();

						auto stripGroupCurrentPos = stream.tell();
						stream.seek_u(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + vertexOffset);
						for (int n = 0; n < vertexCount; n++) {
							auto& vertex = stripGroup.vertices.emplace_back();

							stream
								.read(vertex.boneWeightIndex)
								.read(vertex.boneCount)
								.read(vertex.meshVertexID)
								.read(vertex.boneID);
						}
						stream.seek_u(stripGroupCurrentPos);

						const auto indexCount = stream.read<int32_t>();
						const auto indexOffset = stream.read<int32_t>();

						stripGroupCurrentPos = stream.tell();
						stream.seek_u(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + indexOffset);
						for (int n = 0; n < indexCount; n++) {
							auto& index = stripGroup.indices.emplace_back();
							stream.read(index);
						}
						stream.seek_u(stripGroupCurrentPos);

						const auto stripCount = stream.read<int32_t>();
						const auto stripOffset = stream.read<int32_t>();

						stream.read(stripGroup.flags);

						if (mdl.version >= 49) {
							// mesh topology
							stream.skip<int32_t>(2);
						}

						stream.seek_u(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + stripOffset);
						for (int n = 0; n < stripCount; n++) {
							auto& strip = stripGroup.strips.emplace_back();

							const auto indicesCount = stream.read<int32_t>();
							stream.read(strip.indicesOffset);
							// Note: offset is in elements, not bytes
							strip.indices = std::span(stripGroup.indices.begin() + strip.indicesOffset, indicesCount);

							const auto verticesCount = stream.read<int32_t>();
							stream.read(strip.verticesOffset);
							// Note: offset is in elements, not bytes
							strip.vertices = std::span(stripGroup.vertices.begin() + strip.verticesOffset, verticesCount);

							stream
								.read(strip.boneCount)
								.read(strip.flags);

							// do: bone stuff
							const auto boneStateChangeCount = stream.read<int32_t>();
							const auto boneStateChangeOffset = stream.read<int32_t>();

							if (boneStateChangeCount > 0 && boneStateChangeOffset > 0) {
								const auto savedPos = stream.tell();
								constexpr auto stripHeaderSize = sizeof(int32_t) * 6 + sizeof(int16_t) + sizeof(Strip::Flags);
								const auto stripBasePos = bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + stripOffset + (n * stripHeaderSize);
								stream.seek_u(stripBasePos + boneStateChangeOffset);

								for (int p = 0; p < boneStateChangeCount; p++) {
									auto& boneStateChange = strip.boneStateChanges.emplace_back();
									stream
										.read(boneStateChange.hardwareID)
										.read(boneStateChange.newBoneID);
								}

								stream.seek_u(savedPos);
							}

							if (mdl.version >= 49) {
								// mesh topology
								stream.skip<int32_t>(2);
							}
						}
					}
				}
			}
		}
	}

	return true;
}
