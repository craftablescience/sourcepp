#include <studiomodelpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/detail/BufferUtils.h>

using namespace sourcepp;
using namespace sourcepp::detail;
using namespace studiomodelpp::MDL;

constexpr int32_t MDL_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

bool MDL::open(const std::byte* data, std::size_t size) {
	BufferStream stream{data, size};

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

	auto boneCount = stream.read<int32_t>();
	auto boneOffset = stream.read<int32_t>();

	auto boneControllerCount = stream.read<int32_t>();
	auto boneControllerOffset = stream.read<int32_t>();

	auto hitboxSetCount = stream.read<int32_t>();
	auto hitboxSetOffset = stream.read<int32_t>();

	//auto animDescCount = stream.read<int32_t>();
	//auto animDescOffset = stream.read<int32_t>();
	stream.skip<int32_t>(2);

	//auto sequenceDescCount = stream.read<int32_t>();
	//auto sequenceDescOffset = stream.read<int32_t>();
	stream.skip<int32_t>(2);

	stream
		.read(this->activityListVersion)
		.read(this->eventsIndexed);

	auto materialCount = stream.read<int32_t>();
	auto materialOffset = stream.read<int32_t>();

	auto materialDirCount = stream.read<int32_t>();
	auto materialDirOffset = stream.read<int32_t>();

    auto skinReferenceCount = stream.read<int32_t>();
    auto skinReferenceFamilyCount = stream.read<int32_t>();
    auto skinReferenceOffset = stream.read<int32_t>();

	auto bodyPartCount = stream.read<int32_t>();
	auto bodyPartOffset = stream.read<int32_t>();

	// Done reading sequentially, start seeking to offsets

	stream.seek(boneOffset);
	for (int i = 0; i < boneCount; i++) {
		auto& bone = this->bones.emplace_back();

		readStringAtOffset(stream, bone.name);
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
		readStringAtOffset(stream, bone.surfacePropName, std::ios::cur, sizeof(int32_t) * 12 + sizeof(Vec3f) * 4 + sizeof(Quat) * 2 + sizeof(Matrix<3,4>) + sizeof(Bone::Flags));
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
		auto hitboxSetPos = hitboxSetOffset + i * (sizeof(int32_t) * 3);
		stream.seek(hitboxSetPos);

		auto& hitboxSet = this->hitboxSets.emplace_back();

		readStringAtOffset(stream, hitboxSet.name);
		auto hitboxCount = stream.read<int32_t>();
		auto hitboxOffset = stream.read<int32_t>();

		for (int j = 0; j < hitboxCount; j++) {
			auto hitboxPos = hitboxOffset + j * (sizeof(int32_t) * 11 + sizeof(Vec3f) * 2);
			stream.seek(hitboxSetPos + hitboxPos);

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

		// Needs to be read from the base of the data structure
		readStringAtOffset(stream, material.name);

		stream.read(material.flags);

		// used
		stream.skip<int32_t>();
		// _unused0
		stream.skip<int32_t>(13);
	}

	stream.seek(materialDirOffset);
	for (int i = 0; i < materialDirCount; i++) {
		auto& materialDir = this->materialDirectories.emplace_back();
		readStringAtOffset(stream, materialDir, std::ios::beg, 0);
	}

    stream.seek(skinReferenceOffset);
    for (int i = 0; i < skinReferenceFamilyCount; i++) {
        std::vector<int16_t> skinFamily;
        for (int j = 0; j < skinReferenceCount; j++) {
            skinFamily.push_back(stream.read<int16_t>());
        }
        this->skins.push_back(std::move(skinFamily));
    }

	for (int i = 0; i < bodyPartCount; i++) {
		auto bodyPartPos = bodyPartOffset + i * (sizeof(int32_t) * 4);
		stream.seek(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		readStringAtOffset(stream, bodyPart.name);

		auto modelsCount = stream.read<int32_t>();
		// base
		stream.skip<int32_t>();
		auto modelsOffset = stream.read<int32_t>();

		for (int j = 0; j < modelsCount; j++) {
			auto modelPos = modelsOffset + j * (64 + sizeof(float) + sizeof(int32_t) * 20);
			stream.seek(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			stream
				.read(model.name, 64)
				.read(model.type)
				.read(model.boundingRadius);

			auto meshesCount = stream.read<int32_t>();
			auto meshesOffset = stream.read<int32_t>();

			stream
				.read(model.verticesCount)
				.read(model.verticesOffset);

			for (int k = 0; k < meshesCount; k++) {
				auto meshPos = meshesOffset + k * (sizeof(int32_t) * (18 + MAX_LOD_COUNT) + sizeof(Vec3f));
				stream.seek(bodyPartPos + modelPos + meshPos);

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
