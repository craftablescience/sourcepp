#include <studiomodelpp/structs/VVD.h>

#include <BufferStream.h>

using namespace sourcepp;
using namespace studiomodelpp::VVD;

constexpr int32_t VVD_ID = 'I' + ('D' << 8) + ('S' << 16) + ('V' << 24);

bool VVD::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStream stream{data, size};

	if (stream.read<int32_t>() != VVD_ID) {
		return false;
	}

	if (stream.read(this->version); this->version != 4) {
		return false;
	}

	if (stream.read<int32_t>() != mdl.checksum) {
		return false;
	}

	stream
		.read(this->numLODs)
		.read(this->numVerticesInLOD);

	auto fixupsCount = stream.read<int32_t>();
	auto fixupsOffset = stream.read<int32_t>();
	auto verticesOffset = stream.read<int32_t>();
	auto tangentsOffset = stream.read<int32_t>();

	stream.seek(verticesOffset);
	for (int i = 0; i < this->numVerticesInLOD[0]; i++) {
		auto& vertex = this->vertices.emplace_back();

		stream.read(vertex.boneWeight.weight);

		std::array<int8_t, MAX_BONES_PER_VERTEX> bones{};
		stream.read(bones);
		auto boneCount = stream.read<int8_t>();
		for (int8_t j = 0; j < boneCount && j < MAX_BONES_PER_VERTEX; j++) {
			vertex.boneWeight.bones.push_back(bones[j]);
		}

		stream
			.read(vertex.position)
			.read(vertex.normal)
			.read(vertex.uv);
		// tangents are assigned below
	}

	stream.seek(tangentsOffset);
	for (std::size_t i = 0; i < this->numVerticesInLOD[0]; i++) {
		this->vertices.at(i).tangent = stream.read<Vec4f>();
	}

	stream.seek(fixupsOffset);
	for (int i = 0; i < fixupsCount; i++) {
		auto& fixup = this->fixups.emplace_back();

		stream
			.read(fixup.LOD)
			.read(fixup.sourceVertexID)
			.read(fixup.vertexCount);
	}

	return true;
}
