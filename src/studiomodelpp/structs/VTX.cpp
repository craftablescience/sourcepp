#include <studiomodelpp/structs/VTX.h>

#include <BufferStream.h>

using namespace studiomodelpp::VTX;

bool VTX::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStream stream{data, size};

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

	// todo: read material replacement list
	stream.skip<int32_t>();

	auto bodyPartCount = stream.read<int32_t>();
	auto bodyPartOffset = stream.read<int32_t>();

	for (int i = 0; i < bodyPartCount; i++) {
		auto bodyPartPos = bodyPartOffset + i * ((sizeof(int32_t) * 2));
		stream.seek(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		auto modelCount = stream.read<int32_t>();
		auto modelOffset = stream.read<int32_t>();

		for (int j = 0; j < modelCount; j++) {
			auto modelPos = modelOffset + j * (sizeof(int32_t) * 2);
			stream.seek(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			auto modelLODCount = stream.read<int32_t>();
			auto modelLODOffset = stream.read<int32_t>();

			for (int k = 0; k < modelLODCount; k++) {
				auto modelLODPos = modelLODOffset + k * (sizeof(int32_t) * 2 + sizeof(float));
				stream.seek(bodyPartPos + modelPos + modelLODPos);

				auto& modelLOD = model.modelLODs.emplace_back();

				auto meshCount = stream.read<int32_t>();
				auto meshOffset = stream.read<int32_t>();

				stream.read(modelLOD.switchDistance);

				for (int l = 0; l < meshCount; l++) {
					auto meshPos = meshOffset + l * (sizeof(int32_t) * 2 + sizeof(Mesh::Flags));
					stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos);

					auto& mesh = modelLOD.meshes.emplace_back();

					auto stripGroupCount = stream.read<int32_t>();
					auto stripGroupOffset = stream.read<int32_t>();

					stream.read(mesh.flags);

					for (int m = 0; m < stripGroupCount; m++) {
						int stripGroupNumInts = 6;
						if (mdl.version >= 49) {
							stripGroupNumInts += 2;
						}
						auto stripGroupPos = stripGroupOffset + m * (sizeof(int32_t) * stripGroupNumInts + sizeof(StripGroup::Flags));
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos);

						auto& stripGroup = mesh.stripGroups.emplace_back();

						auto vertexCount = stream.read<int32_t>();
						auto vertexOffset = stream.read<int32_t>();

						auto stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + vertexOffset);
						for (int n = 0; n < vertexCount; n++) {
							auto& vertex = stripGroup.vertices.emplace_back();

							// todo: process bone data
							stream.skip<uint8_t>(4);

							stream.read(vertex.meshVertexID);

							// ditto
							stream.skip<int8_t>(3);
						}
						stream.seek(stripGroupCurrentPos);

						auto indexCount = stream.read<int32_t>();
						auto indexOffset = stream.read<int32_t>();

						stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + indexOffset);
						for (int n = 0; n < indexCount; n++) {
							auto& index = stripGroup.indices.emplace_back();
							stream.read(index);
						}
						stream.seek(stripGroupCurrentPos);

						auto stripCount = stream.read<int32_t>();
						auto stripOffset = stream.read<int32_t>();

						stream.read(stripGroup.flags);

						if (mdl.version >= 49) {
							// mesh topology
							stream.skip<int32_t>(2);
						}

						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + stripOffset);
						for (int n = 0; n < stripCount; n++) {
							auto& strip = stripGroup.strips.emplace_back();

							auto indicesCount = stream.read<int32_t>();
							stream.read(strip.indicesOffset);
							// todo: check if offset is in bytes
							strip.indices = std::span<unsigned short>(stripGroup.indices.begin() + strip.indicesOffset, indicesCount);

							auto verticesCount = stream.read<int32_t>();
							stream.read(strip.verticesOffset);
							// todo: check if offset is in bytes
							strip.vertices = std::span<Vertex>(stripGroup.vertices.begin() + strip.verticesOffset, verticesCount);

							stream
								.read(strip.boneCount)
								.read(strip.flags);

							// todo: bone stuff
							stream.skip<int32_t>(2);

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
