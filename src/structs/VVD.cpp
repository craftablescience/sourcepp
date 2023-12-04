#include <studiomodelpp/structs/VVD.h>

#include <studiomodelpp/internal/BufferStream.h>

using namespace studiomodelpp::VVD;
using namespace studiomodelpp::internal;

constexpr int VVD_ID = 'I' + ('D' << 8) + ('S' << 16) + ('V' << 24);

bool VVD::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStream stream{data, size};

	int id = stream.read<int>();
	if (id != VVD_ID) {
		return false;
	}

	stream.read(this->version);
	if (this->version != 4) {
		return false;
	}

	int checksum = stream.read<int>();
	if (checksum != mdl.checksum) {
		return false;
	}

	stream.read(this->numLODs);
	stream.read(this->numVerticesInLOD);

	int fixupsCount = stream.read<int>();
	int fixupsOffset = stream.read<int>();
	int verticesOffset = stream.read<int>();
	int tangentsOffset = stream.read<int>();

	stream.seek(verticesOffset);
	for (int i = 0; i < this->numVerticesInLOD[0]; i++) {
		auto& vertex = this->vertices.emplace_back();

		stream.read(vertex.boneWeight.weight);

		std::array<char, MAX_BONES_PER_VERTEX> bones{};
		stream.read(bones);
		char boneCount = stream.read<char>();
		for (int j = 0; j < boneCount && j < MAX_BONES_PER_VERTEX; j++) {
			vertex.boneWeight.bones.push_back(bones[j]);
		}

		stream.read(vertex.position);
		stream.read(vertex.normal);
		stream.read(vertex.uv);
		// tangents are assigned below
	}

	stream.seek(tangentsOffset);
	for (std::size_t i = 0; i < this->numVerticesInLOD[0]; i++) {
		this->vertices.at(i).tangent = stream.read<Vector4>();
	}

	stream.seek(fixupsOffset);
	for (int i = 0; i < fixupsCount; i++) {
		auto& fixup = this->fixups.emplace_back();

		stream.read(fixup.LOD);
		stream.read(fixup.sourceVertexID);
		stream.read(fixup.vertexCount);
	}

	return true;
}
