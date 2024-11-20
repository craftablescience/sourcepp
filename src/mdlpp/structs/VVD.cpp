#include <mdlpp/structs/VVD.h>

#include <BufferStream.h>

using namespace mdlpp::VVD;
using namespace sourcepp;

constexpr int32_t VVD_ID = 'I' + ('D' << 8) + ('S' << 16) + ('V' << 24);

bool VVD::open(const std::byte* data, std::size_t size, const MDL::MDL& mdl) {
	BufferStreamReadOnly stream{data, size};

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

	const auto fixupsCount = stream.read<int32_t>();
	const auto fixupsOffset = stream.read<int32_t>();
	const auto verticesOffset = stream.read<int32_t>();
	const auto tangentsOffset = stream.read<int32_t>();

	stream.seek(verticesOffset);
	for (int i = 0; i < this->numVerticesInLOD[0]; i++) {
		auto& [boneWeight, position, normal, uv, tangent] = this->vertices.emplace_back();

		stream.read(boneWeight.weight);

		std::array<int8_t, MAX_BONES_PER_VERTEX> bones{};
		stream.read(bones);
		const auto boneCount = stream.read<int8_t>();
		for (int8_t j = 0; j < boneCount && j < MAX_BONES_PER_VERTEX; j++) {
			boneWeight.bones.push_back(bones[j]);
		}

		stream
			.read(position)
			.read(normal)
			.read(uv);
		// tangents are assigned below
	}

	stream.seek(tangentsOffset);
	for (std::size_t i = 0; i < this->numVerticesInLOD[0]; i++) {
		this->vertices.at(i).tangent = stream.read<math::Vec4f>();
	}

	stream.seek(fixupsOffset);
	for (int i = 0; i < fixupsCount; i++) {
		auto& [LOD, sourceVertexID, vertexCount] = this->fixups.emplace_back();
		stream >> LOD >> sourceVertexID >> vertexCount;
	}

	return true;
}
