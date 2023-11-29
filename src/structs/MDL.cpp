#include <studiomodelpp/structs/MDL.h>

#include <studiomodelpp/internal/BufferStream.h>
#include <studiomodelpp/internal/Helpers.h>

using namespace studiomodelpp::MDL;
using namespace studiomodelpp::internal;

constexpr int MDL_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

bool MDL::open(const std::byte* data, std::size_t size) {
	BufferStream stream{data, size};

	int id = stream.read<int>();
	if (id != MDL_ID) {
		return false;
	}

	stream.read(this->version);
	stream.read(this->checksum);

	auto nameBytes = stream.readBytes<64>();
	this->name = std::string{reinterpret_cast<const char* const>(nameBytes.data())};

	// dataLength
	stream.skip<int>();

	stream.read(this->eyePosition);
	stream.read(this->illuminationPosition);
	stream.read(this->hullMin);
	stream.read(this->hullMax);
	stream.read(this->viewBBoxMin);
	stream.read(this->viewBBoxMax);
	stream.read(this->flags);

	int boneCount = stream.read<int>();
	int boneOffset = stream.read<int>();

	int boneControllerCount = stream.read<int>();
	int boneControllerOffset = stream.read<int>();

	int hitboxSetCount = stream.read<int>();
	int hitboxSetOffset = stream.read<int>();

	int animDescCount = stream.read<int>();
	int animDescOffset = stream.read<int>();

	int sequenceDescCount = stream.read<int>();
	int sequenceDescOffset = stream.read<int>();

	stream.read(this->activityListVersion);
	stream.read(this->eventsIndexed);

	int materialCount = stream.read<int>();
	int materialOffset = stream.read<int>();

	int materialDirCount = stream.read<int>();
	int materialDirOffset = stream.read<int>();

	// Done reading sequentially, start seeking to offsets

	stream.seek(boneOffset);
	for (int i = 0; i < boneCount; i++) {
		auto& bone = this->bones.emplace_back();

		readStringAtOffset(stream, bone.name);
		stream.read(bone.parent);
		stream.read(bone.boneController);
		stream.read(bone.position);
		stream.read(bone.rotationQuat);
		stream.read(bone.rotationEuler);
		stream.read(bone.positionScale);
		stream.read(bone.rotationScale);
		stream.read(bone.poseToBose);
		stream.read(bone.alignment);
		stream.read(bone.flags);
		stream.read(bone.procType);
		stream.read(bone.procIndex);
		stream.read(bone.physicsBone);
		readStringAtOffset(stream, bone.surfacePropName);
		stream.read(bone.contents);

		// _unused0
		stream.skip<int, 8>();
	}

	stream.seek(boneControllerOffset);
	for (int i = 0; i < boneControllerCount; i++) {
		this->boneControllers.push_back(stream.read<BoneController>());

		// _unused0
		stream.skip<int, 8>();
	}

	stream.seek(hitboxSetOffset);
	for (int i = 0; i < hitboxSetCount; i++) {
		auto& hitboxSet = this->hitboxSets.emplace_back();

		readStringAtOffset(stream, hitboxSet.name);

		int hitboxCount = stream.read<int>();
		int hitboxOffset = stream.read<int>();

		stream.seek(hitboxOffset);
		for (int j = 0; j < hitboxCount; j++) {
			auto& hitbox = hitboxSet.hitboxes.emplace_back();

			stream.read(hitbox.bone);
			stream.read(hitbox.group);
			stream.read(hitbox.bboxMin);
			stream.read(hitbox.bboxMax);
			readStringAtOffset(stream, hitbox.name);

			// _unused0
			stream.skip<int, 8>();
		}
	}

	stream.seek(animDescOffset);
	for (int i = 0; i < animDescCount; i++) {
		// todo(wrapper)
	}

	stream.seek(sequenceDescOffset);
	for (int i = 0; i < sequenceDescCount; i++) {
		// todo(wrapper)
	}

	stream.seek(materialOffset);
	for (int i = 0; i < materialCount; i++) {
		auto& material = this->materials.emplace_back();

		int nameOffset = stream.read<int>();
		auto pos = stream.tell();
		stream.seek(nameOffset, std::ios::cur);
		stream.read(material.name);
		stream.seek(pos);

		stream.read(material.flags);

		// used
		// _unused0
		stream.skip<int, 14>();
	}

	stream.seek(materialDirOffset);
	for (int i = 0; i < materialDirCount; i++) {
		auto& materialDir = this->materialDirectories.emplace_back();

		int nameOffset = stream.read<int>();
		auto pos = stream.tell();
		stream.seek(nameOffset, std::ios::cur);
		stream.read(materialDir);
		stream.seek(pos);
	}

	return true;
}
