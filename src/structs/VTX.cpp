#include <studiomodelpp/structs/VTX.h>

#include <studiomodelpp/internal/BufferStream.h>

using namespace studiomodelpp::VTX;
using namespace studiomodelpp::internal;

bool VTX::open(const std::byte* data, std::size_t size, int mdlVersion, int mdlChecksum) {
	BufferStream stream{data, size};

	stream.read(this->version);
	stream.read(this->vertexCacheSize);
	stream.read(this->maxBonesPerStrip);
	stream.read(this->maxBonesPerTriangle);
	stream.read(this->maxBonesPerVertex);

	int checksum = stream.read<int>();
	if (checksum != mdlChecksum) {
		return false;
	}

	stream.read(this->numLODs);

	// todo: read material replacement list
	stream.skip<int>();

	int bodyPartCount = stream.read<int>();
	int bodyPartOffset = stream.read<int>();

	for (int i = 0; i < bodyPartCount; i++) {
		stream.seek(bodyPartOffset + i * ((sizeof(int) * 2)));

		auto& bodyPart = this->bodyParts.emplace_back();

		int modelCount = stream.read<int>();
		int modelOffset = stream.read<int>();

		for (int j = 0; j < modelCount; j++) {
			stream.seek(bodyPartOffset + modelOffset + j * (sizeof(int) * 2));

			auto& model = bodyPart.models.emplace_back();

			int modelLODCount = stream.read<int>();
			int modelLODOffset = stream.read<int>();

			for (int k = 0; k < modelLODCount; k++) {
				stream.seek(bodyPartOffset + modelOffset + modelLODOffset + k * (sizeof(int) * 3));

				auto& modelLOD = model.modelLODs.emplace_back();

				int meshCount = stream.read<int>();
				int meshOffset = stream.read<int>();

				stream.read(modelLOD.switchDistance);

				for (int l = 0; l < meshCount; l++) {
					stream.seek(bodyPartOffset + modelOffset + modelLODOffset + meshOffset + l * (sizeof(int) * 2) + sizeof(Mesh::Flags));

					auto& mesh = modelLOD.meshes.emplace_back();

					int stripGroupCount = stream.read<int>();
					int stripGroupOffset = stream.read<int>();

					stream.read(mesh.flags);

					for (int m = 0; m < stripGroupCount; m++) {
						int stripGroupNumInts = 6;
						if (mdlVersion >= 49) {
							stripGroupNumInts += 2;
						}
						stream.seek(bodyPartOffset + modelOffset + modelLODOffset + meshOffset + stripGroupOffset + m * (sizeof(int) * stripGroupNumInts + sizeof(StripGroup::Flags)));

						auto& stripGroup = mesh.stripGroups.emplace_back();

						int vertexCount = stream.read<int>();
						int vertexOffset = stream.read<int>();

						auto stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartOffset + modelOffset + modelLODOffset + meshOffset + stripGroupOffset + vertexOffset);
						for (int n = 0; n < vertexCount; n++) {
							auto& vertex = stripGroup.vertices.emplace_back();

							// todo: process bone data
							stream.skip<unsigned char, 4>();

							stream.read(vertex.meshVertexID);

							// ditto
							stream.skip<char, 3>();
						}
						stream.seek(stripGroupCurrentPos);

						int indexCount = stream.read<int>();
						int indexOffset = stream.read<int>();

						stripGroupCurrentPos = stream.tell();
						stream.seek(bodyPartOffset + modelOffset + modelLODOffset + meshOffset + stripGroupOffset + indexOffset);
						for (int n = 0; n < indexCount; n++) {
							auto& index = stripGroup.indices.emplace_back();
							stream.read(index);
						}
						stream.seek(stripGroupCurrentPos);

						int stripCount = stream.read<int>();
						int stripOffset = stream.read<int>();

						stream.read(stripGroup.flags);

						if (mdlVersion >= 49) {
							// todo: mesh topology?
							stream.skip<int, 2>();
						}

						stream.seek(bodyPartOffset + modelOffset + modelLODOffset + meshOffset + stripGroupOffset + stripOffset);
						for (int n = 0; n < stripCount; n++) {
							auto& strip = stripGroup.strips.emplace_back();

							// todo: maybe get a std::span of parent array?
							stream.skip<int, 4>();

							stream.read(strip.boneCount);
							stream.read(strip.flags);

							// todo: bone stuff
							stream.skip<int, 2>();

							if (mdlVersion >= 49) {
								// mesh topology
								stream.skip<int, 2>();
							}
						}
					}
				}
			}
		}
	}

	return true;
}
