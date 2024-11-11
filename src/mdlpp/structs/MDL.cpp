#include <mdlpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/parser/Binary.h>

using namespace mdlpp::MDL;
using namespace sourcepp;

constexpr int32_t MDL_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

bool MDL::open(const std::byte* data, std::size_t size) {
	BufferStreamReadOnly stream{data, size};

	if (stream.read<int32_t>() != MDL_ID) {
		return false;
	}

	if (stream.read(this->version); this->version < 44 || this->version > 49) {
		return false;
	}

	stream
		.read(this->checksum)
		.read(this->name, 64)
		.skip<int32_t>() // dataLength
		.read(this->eyePosition)
		.read(this->illuminationPosition)
		.read(this->hullMin)
		.read(this->hullMax)
		.read(this->viewBBoxMin)
		.read(this->viewBBoxMax);

	this->flags = static_cast<Flags>(stream.read<int32_t>());

	const auto boneCount = stream.read<int32_t>();
	const auto boneOffset = stream.read<int32_t>();

	const auto boneControllerCount = stream.read<int32_t>();
	const auto boneControllerOffset = stream.read<int32_t>();

	const auto hitboxSetCount = stream.read<int32_t>();
	const auto hitboxSetOffset = stream.read<int32_t>();

	//auto animDescCount = stream.read<int32_t>();
	//auto animDescOffset = stream.read<int32_t>();
	stream.skip<int32_t>(2);

	//auto sequenceDescCount = stream.read<int32_t>();
	//auto sequenceDescOffset = stream.read<int32_t>();
	stream.skip<int32_t>(2);

	stream
		.read(this->activityListVersion)
		.read(this->eventsIndexed);

	const auto materialCount = stream.read<int32_t>();
	const auto materialOffset = stream.read<int32_t>();

	const auto materialDirCount = stream.read<int32_t>();
	const auto materialDirOffset = stream.read<int32_t>();

	const auto skinReferenceCount = stream.read<int32_t>();
	const auto skinReferenceFamilyCount = stream.read<int32_t>();
	const auto skinReferenceOffset = stream.read<int32_t>();

	const auto bodyPartCount = stream.read<int32_t>();
	const auto bodyPartOffset = stream.read<int32_t>();

	// Done reading sequentially, start seeking to offsets

	stream.seek(boneOffset);
	for (int i = 0; i < boneCount; i++) {
		auto& bone = this->bones.emplace_back();

		parser::binary::readStringAtOffset(stream, bone.name);
		stream
			.read(bone.parent)
			.read(bone.boneController)
			.read(bone.position)
			.read(bone.rotationQuat)
			.read(bone.rotationEuler)
			.read(bone.positionScale)
			.read(bone.rotationScale)
			.read(bone.poseToBose)
			.read(bone.alignment)
			.read(bone.flags)
			.read(bone.procType)
			.read(bone.procIndex)
			.read(bone.physicsBone);
		parser::binary::readStringAtOffset(stream, bone.surfacePropName, BufferStream::SEEKDIR_CUR, sizeof(int32_t) * 12 + sizeof(math::Vec3f) * 4 + sizeof(math::Quat) * 2 + sizeof(math::Mat3x4f) + sizeof(Bone::Flags));
		stream.read(bone.contents);

		// _unused0
		stream.skip<int32_t>(8);
	}

	stream.seek(boneControllerOffset);
	for (int i = 0; i < boneControllerCount; i++) {
		this->boneControllers.push_back(stream.read<BoneController>());

		// _unused0
		stream.skip<int32_t>(8);
	}

	for (int i = 0; i < hitboxSetCount; i++) {
		const auto hitboxSetPos = hitboxSetOffset + i * (sizeof(int32_t) * 3);
		stream.seek_u(hitboxSetPos);

		auto& hitboxSet = this->hitboxSets.emplace_back();

		parser::binary::readStringAtOffset(stream, hitboxSet.name);
		const auto hitboxCount = stream.read<int32_t>();
		const auto hitboxOffset = stream.read<int32_t>();

		for (int j = 0; j < hitboxCount; j++) {
			const auto hitboxPos = hitboxOffset + j * (sizeof(int32_t) * 11 + sizeof(math::Vec3f) * 2);
			stream.seek_u(hitboxSetPos + hitboxPos);

			auto& hitbox = hitboxSet.hitboxes.emplace_back();

			stream
				.read(hitbox.bone)
				.read(hitbox.group)
				.read(hitbox.bboxMin)
				.read(hitbox.bboxMax);

			// note: we don't know what model versions use absolute vs. relative offsets here
			//       and this is unimportant, so skip parsing the bbox name here
			//readStringAtOffset(stream, hitbox.name, std::ios::cur, sizeof(int32_t) * 3 + sizeof(Vec3f) * 2);
			stream.skip<int32_t>();
			hitbox.name = "";

			// _unused0
			stream.skip<int32_t>(8);
		}
	}

	/*
	stream.seek(animDescOffset);
	for (int i = 0; i < animDescCount; i++) {
		// todo(wrapper)
	}

	stream.seek(sequenceDescOffset);
	for (int i = 0; i < sequenceDescCount; i++) {
		// todo(wrapper)
	}
	*/

	stream.seek(materialOffset);
	for (int i = 0; i < materialCount; i++) {
		auto& material = this->materials.emplace_back();

		parser::binary::readStringAtOffset(stream, material.name);
		stream.read(material.flags);

		// used
		stream.skip<int32_t>();
		// _unused0
		stream.skip<int32_t>(13);
	}

	stream.seek(materialDirOffset);
	for (int i = 0; i < materialDirCount; i++) {
		auto& materialDir = this->materialDirectories.emplace_back();

		parser::binary::readStringAtOffset(stream, materialDir, BufferStream::SEEKDIR_BEG, 0);
	}

    stream.seek(skinReferenceOffset);
    for (int i = 0; i < skinReferenceFamilyCount; i++) {
        std::vector<int16_t> skinFamily;
        skinFamily.reserve(skinReferenceCount);
		for (int j = 0; j < skinReferenceCount; j++) {
            skinFamily.push_back(stream.read<int16_t>());
        }
        this->skins.push_back(std::move(skinFamily));
    }

	for (int i = 0; i < bodyPartCount; i++) {
		const auto bodyPartPos = bodyPartOffset + i * (sizeof(int32_t) * 4);
		stream.seek_u(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		parser::binary::readStringAtOffset(stream, bodyPart.name);

		const auto modelsCount = stream.read<int32_t>();
		stream.skip<int32_t>(); // base
		const auto modelsOffset = stream.read<int32_t>();

		for (int j = 0; j < modelsCount; j++) {
			auto modelPos = modelsOffset + j * (64 + sizeof(float) + sizeof(int32_t) * 20);
			stream.seek_u(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			stream
				.read(model.name, 64)
				.read(model.type)
				.read(model.boundingRadius);

			const auto meshesCount = stream.read<int32_t>();
			const auto meshesOffset = stream.read<int32_t>();

			stream
				.read(model.verticesCount)
				.read(model.verticesOffset);

			for (int k = 0; k < meshesCount; k++) {
				const auto meshPos = meshesOffset + k * (sizeof(int32_t) * (18 + MAX_LOD_COUNT) + sizeof(math::Vec3f));
				stream.seek_u(bodyPartPos + modelPos + meshPos);

				auto& mesh = model.meshes.emplace_back();

				stream
					.read(mesh.material)
					.skip<int32_t>()
					.read(mesh.verticesCount)
					.read(mesh.verticesOffset)
					.skip<int32_t>(2)
					.read(mesh.materialType)
					.read(mesh.materialParam)
					.read(mesh.meshID)
					.read(mesh.center);
			}
		}
	}

	return true;
}
