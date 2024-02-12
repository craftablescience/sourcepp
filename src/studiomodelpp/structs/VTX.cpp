#include <studiomodelpp/structs/VTX.h>

#include <BufferStream.h>

using namespace studiomodelpp::VTX;

bool VTX::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStream stream{data, size};

	stream.read(this->version);
	if (this->version != 7) {
		return false;
	}

	stream.read(this->vertexCacheSize);
	stream.read(this->maxBonesPerStrip);
	stream.read(this->maxBonesPerTriangle);
	stream.read(this->maxBonesPerVertex);

	int checksum = stream.read<int>();
	if (checksum != mdl.checksum) {
		return false;
	}

	stream.read(this->numLODs);

	// todo: read material replacement list
	stream.skip<int>();

	int bodyPartCount = stream.read<int>();
	int bodyPartOffset = stream.read<int>();

	for (int i = 0; i < bodyPartCount; i++) {
		auto bodyPartPos = bodyPartOffset + i * ((sizeof(int) * 2));
		stream.seek(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		int modelCount = stream.read<int>();
		int modelOffset = stream.read<int>();

		for (int j = 0; j < modelCount; j++) {
			auto modelPos = modelOffset + j * (sizeof(int) * 2);
			stream.seek(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			int modelLODCount = stream.read<int>();
			int modelLODOffset = stream.read<int>();

			for (int k = 0; k < modelLODCount; k++) {
				auto modelLODPos = modelLODOffset + k * (sizeof(int) * 2 + sizeof(float));
				stream.seek(bodyPartPos + modelPos + modelLODPos);

				auto& modelLOD = model.modelLODs.emplace_back();

				int meshCount = stream.read<int>();
				int meshOffset = stream.read<int>();

				stream.read(modelLOD.switchDistance);

				for (int l = 0; l < meshCount; l++) {
					auto meshPos = meshOffset + l * (sizeof(int) * 2 + sizeof(Mesh::Flags));
					stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos);

					auto& mesh = modelLOD.meshes.emplace_back();

					int stripGroupCount = stream.read<int>();
					int stripGroupOffset = stream.read<int>();

					stream.read(mesh.flags);

					for (int m = 0; m < stripGroupCount; m++) {
						int stripGroupNumInts = 6;
						if (mdl.version >= 49) {
							stripGroupNumInts += 2;
						}
						auto stripGroupPos = stripGroupOffset + m * (sizeof(int) * stripGroupNumInts + sizeof(StripGroup::Flags));
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos);

						auto& stripGroup = mesh.stripGroups.emplace_back();

						int vertexCount = stream.read<int>();
						int vertexOffset = stream.read<int>();

						auto stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + vertexOffset);
						for (int n = 0; n < vertexCount; n++) {
							auto& vertex = stripGroup.vertices.emplace_back();

							// todo: process bone data
							stream.skip<unsigned char>(4);

							stream.read(vertex.meshVertexID);

							// ditto
							stream.skip<char>(3);
						}
						stream.seek(stripGroupCurrentPos);

						int indexCount = stream.read<int>();
						int indexOffset = stream.read<int>();

						stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + indexOffset);
						for (int n = 0; n < indexCount; n++) {
							auto& index = stripGroup.indices.emplace_back();
							stream.read(index);
						}
						stream.seek(stripGroupCurrentPos);

						int stripCount = stream.read<int>();
						int stripOffset = stream.read<int>();

						stream.read(stripGroup.flags);

						if (mdl.version >= 49) {
							// mesh topology
							stream.skip<int>(2);
						}

						stream.seek(bodyPartPos + modelPos + modelLODPos + meshPos + stripGroupPos + stripOffset);
						for (int n = 0; n < stripCount; n++) {
							auto& strip = stripGroup.strips.emplace_back();

							int indicesCount = stream.read<int>();
							stream.read(strip.indicesOffset);
							// todo: check if offset is in bytes
							strip.indices = std::span<unsigned short>(stripGroup.indices.begin() + strip.indicesOffset, indicesCount);

							int verticesCount = stream.read<int>();
							stream.read(strip.verticesOffset);
							// todo: check if offset is in bytes
							strip.vertices = std::span<Vertex>(stripGroup.vertices.begin() + strip.verticesOffset, verticesCount);

							stream.read(strip.boneCount);
							stream.read(strip.flags);

							// todo: bone stuff
							stream.skip<int>(2);

							if (mdl.version >= 49) {
								// mesh topology
								stream.skip<int>(2);
							}
						}
					}
				}
			}
		}
	}

	return true;
}
