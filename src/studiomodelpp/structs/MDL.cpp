#include <studiomodelpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/detail/BufferUtils.h>

using namespace sourcepp;
using namespace sourcepp::detail;
using namespace studiomodelpp::MDL;

constexpr int MDL_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

bool MDL::open(const std::byte* data, std::size_t size) {
	BufferStream stream{data, size};

	int id = stream.read<int>();
	if (id != MDL_ID) {
		return false;
	}

	stream.read(this->version);
	if (this->version < 44 || this->version > 49) {
		return false;
	}

	stream.read(this->checksum);
    stream.read(this->name, 64);

	// dataLength
	stream.skip<int>();

	stream.read(this->eyePosition);
	stream.read(this->illuminationPosition);
	stream.read(this->hullMin);
	stream.read(this->hullMax);
	stream.read(this->viewBBoxMin);
	stream.read(this->viewBBoxMax);
	this->flags = static_cast<Flags>(stream.read<int>());

	int boneCount = stream.read<int>();
	int boneOffset = stream.read<int>();

	int boneControllerCount = stream.read<int>();
	int boneControllerOffset = stream.read<int>();

	int hitboxSetCount = stream.read<int>();
	int hitboxSetOffset = stream.read<int>();

	int animDescCount = stream.read<int>();
	int animDescOffset = stream.read<int>();

	//int sequenceDescCount = stream.read<int>();
	//int sequenceDescOffset = stream.read<int>();
	stream.skip<int>(2);

	stream.read(this->activityListVersion);
	stream.read(this->eventsIndexed);

	int materialCount = stream.read<int>();
	int materialOffset = stream.read<int>();

	int materialDirCount = stream.read<int>();
	int materialDirOffset = stream.read<int>();

    int skinReferenceCount = stream.read<int>();
    int skinReferenceFamilyCount = stream.read<int>();
    int skinReferenceOffset = stream.read<int>();

	int bodyPartCount = stream.read<int>();
	int bodyPartOffset = stream.read<int>();

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
		readStringAtOffset(stream, bone.surfacePropName, std::ios::cur, sizeof(int) * 12 + sizeof(Vector3) * 4 + sizeof(Quaternion) * 2 + sizeof(Matrix<3, 4>) + sizeof(Bone::Flags));
		stream.read(bone.contents);

		// _unused0
		stream.skip<int>(8);
	}

	stream.seek(boneControllerOffset);
	for (int i = 0; i < boneControllerCount; i++) {
		this->boneControllers.push_back(stream.read<BoneController>());

		// _unused0
		stream.skip<int>(8);
	}

	for (int i = 0; i < hitboxSetCount; i++) {
		auto hitboxSetPos = hitboxSetOffset + i * (sizeof(int) * 3);
		stream.seek(hitboxSetPos);

		auto& hitboxSet = this->hitboxSets.emplace_back();

		readStringAtOffset(stream, hitboxSet.name);
		int hitboxCount = stream.read<int>();
		int hitboxOffset = stream.read<int>();

		for (int j = 0; j < hitboxCount; j++) {
			auto hitboxPos = hitboxOffset + j * (sizeof(int) * 11 + sizeof(Vector3) * 2);
			stream.seek(hitboxSetPos + hitboxPos);

			auto& hitbox = hitboxSet.hitboxes.emplace_back();

			stream.read(hitbox.bone);
			stream.read(hitbox.group);
			stream.read(hitbox.bboxMin);
			stream.read(hitbox.bboxMax);

			// note: we don't know what model versions use absolute vs. relative offsets here
			//       and this is unimportant, so skip parsing the bbox name here
			//readStringAtOffset(stream, hitbox.name, std::ios::cur, sizeof(int) * 3 + sizeof(Vector3) * 2);
			stream.skip<int>();
			hitbox.name = "";

			// _unused0
			stream.skip<int>(8);
		}
	}


	stream.seek(animDescOffset);
	for (int i = 0; i < animDescCount; i++) {
        auto& animDesc = animationDescription.emplace_back();
        stream.skip<int>();
        readStringAtOffset(stream, animDesc.name, std::ios::cur,  sizeof(int) + 2);
        stream.read<float>(animDesc.fps);
        stream.read<AnimDesc::Flags>(animDesc.flags);
        stream.read<int>(animDesc.frameCount);

        int movementCount = stream.read<int>();
        int movementIndex = stream.read<int>();

        size_t current = stream.tell();
        stream.seek(movementIndex);
        for(int j = 0; j < movementCount; j++)
        {
            auto& movement = animDesc.movement.emplace_back();
            stream.read<int>(movement.endframe);
            stream.read<int>(movement.motionFlags);
            stream.read<float>(movement.startVelocity);
            stream.read<float>(movement.endVelocity);
            stream.read<float>(movement.angle);
            stream.read<sourcepp::Vector3>(movement.relativeMovementVector);
            stream.read<sourcepp::Vector3>(movement.position);
        }
        stream.seek(current);

        stream.skip<int>(7); // 6 unused 1 animBlock;


        int animBlockIndex = stream.read<int>();

        current = stream.tell();
        stream.seek(animBlockIndex);
        short nextIndex = 0;
        do{
            auto& anim = animDesc.animations.emplace_back();
            anim.bone = stream.read<std::byte>();
            anim.flags = stream.read<studioAnimFlags>();
            nextIndex = anim.nextOffset = stream.read<short>();

            if(anim.bone == std::byte(255))
                continue; //no animation data.

            if(anim.flags & ( studioAnimFlags::ANIM_RAWROT | studioAnimFlags::ANIM_RAWPOS | studioAnimFlags::ANIM_RAWROT2 ))
            {
                size_t offs = 4;
                if(anim.flags & studioAnimFlags::ANIM_RAWROT)
                {
                    readValueFromPointerAtOffset<Quaternion48>(stream, anim.quat48, std::ios::cur, offs);
                    offs+=4;
                }
                if(anim.flags & studioAnimFlags::ANIM_RAWROT2)
                {
                    readValueFromPointerAtOffset<Quaternion64>(stream, anim.quat64, std::ios::cur, offs);
                    offs+=4;
                }

                if( anim.flags & studioAnimFlags::ANIM_RAWPOS)
                {
                    readValueFromPointerAtOffset<Vector48>(stream, anim.pos, std::ios::cur, offs);
                }
            }
            else
            {

                struct threeShorts
                {
                    short one;
                    short two;
                    short three;

                    short operator[](int i) const
                    {
                        if(i == 0)
                        {
                            return one;
                        }

                        if(i == 1)
                        {
                            return one;
                        }

                        if(i == 2)
                        {
                            return one;
                        }
                        return 0;
                    }
                };

                threeShorts offs{};

                readValueFromPointerAtOffset<threeShorts>(stream, offs, std::ios::cur, 4);

                short offset1 = offs[0];
                short offset2 = offs[1];
                short offset3 = offs[2];;
                size_t telloff1 = stream.tell();
                if(offset1 > 0)
                {
                    stream.seek(telloff1 + offset1);
                    anim.rotVOffsets[0].isValid = true;
                    anim.rotVOffsets[0].val = stream.read<AnimationFrameValue>();
                }
                if(offset2 > 0)
                {
                    stream.seek(telloff1 + offset2);
                    anim.rotVOffsets[0].isValid = true;
                    anim.rotVOffsets[0].val = stream.read<AnimationFrameValue>();
                }
                if(offset3 > 0)
                {
                    stream.seek(telloff1 + offset3);
                    anim.rotVOffsets[0].isValid = true;
                    anim.rotVOffsets[0].val = stream.read<AnimationFrameValue>();
                }
                stream.seek(telloff1);
                if(anim.flags & studioAnimFlags::ANIM_ANIMROT)
                {
                    offset1 = stream.read<short>();
                    offset2 = stream.read<short>();
                    offset3 = stream.read<short>();
                    size_t telloff1 = stream.tell();
                    if(offset1 > 0)
                    {
                        stream.seek(telloff1 + offset1);
                        anim.posVOffsets[0].isValid = true;
                        anim.posVOffsets[0].val = stream.read<AnimationFrameValue>();
                    }
                    if(offset2 > 0)
                    {
                        stream.seek(telloff1 + offset2);
                        anim.posVOffsets[0].isValid = true;
                        anim.posVOffsets[0].val = stream.read<AnimationFrameValue>();
                    }
                    if(offset3 > 0)
                    {
                        stream.seek(telloff1 + offset3);
                        anim.posVOffsets[0].isValid = true;
                        anim.posVOffsets[0].val = stream.read<AnimationFrameValue>();
                    }
                }
                stream.seek(telloff1);

            }


//            short offsets[3];
//
//            offsets[0] = stream.read<short>();
//            offsets[1] = stream.read<short>();
//            offsets[2] = stream.read<short>();
//            size_t tempCurr = stream.tell();
//            size_t trueOffset = tempCurr + (sizeof(std::byte) * 2) + sizeof(short);
//            if(offsets[0] > 0)
//            {
//                stream.seek(trueOffset + offsets[0]);
//                anim.posVOffsets[0] = stream.read<AnimationFrameValue>();
//            }
//            if(offsets[1] > 0) {
//                stream.seek(trueOffset + offsets[1]);
//                anim.posVOffsets[1] = stream.read<AnimationFrameValue>();
//            }
//            if(offsets[2] > 0) {
//                stream.seek(trueOffset + offsets[2]);
//                anim.posVOffsets[2] = stream.read<AnimationFrameValue>();
//            }

//            if((flags & studioAnimFlags::ANIM_ANIMROT) != 0)
//            {
//
//            }


        } while (nextIndex != 0);
        stream.seek(current);

        stream.skip<int>(7);
        stream.skip<short>(2);
        stream.skip<int>();

        stream.skip<float>();

    }
    /*
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
		stream.skip<int>();
		// _unused0
		stream.skip<int>(13);
	}

	stream.seek(materialDirOffset);
	for (int i = 0; i < materialDirCount; i++) {
		auto& materialDir = this->materialDirectories.emplace_back();
		readStringAtOffset(stream, materialDir, std::ios::beg, 0);
	}

    stream.seek(skinReferenceOffset);
    for (int i = 0; i < skinReferenceFamilyCount; i++) {
        std::vector<short> skinFamily;
        for (int j = 0; j < skinReferenceCount; j++) {
            skinFamily.push_back(stream.read<short>());
        }
        this->skins.push_back(std::move(skinFamily));
    }

	for (int i = 0; i < bodyPartCount; i++) {
		auto bodyPartPos = bodyPartOffset + i * (sizeof(int) * 4);
		stream.seek(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		readStringAtOffset(stream, bodyPart.name);

		int modelsCount = stream.read<int>();
		// base
		stream.skip<int>();
		int modelsOffset = stream.read<int>();

		for (int j = 0; j < modelsCount; j++) {
			auto modelPos = modelsOffset + j * (64 + sizeof(float) + sizeof(int) * 20);
			stream.seek(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			stream.read(model.name, 64);
			stream.read(model.type);
			stream.read(model.boundingRadius);

			int meshesCount = stream.read<int>();
			int meshesOffset = stream.read<int>();

			stream.read(model.verticesCount);
			stream.read(model.verticesOffset);

			for (int k = 0; k < meshesCount; k++) {
				auto meshPos = meshesOffset + k * (sizeof(int) * (18 + MAX_LOD_COUNT) + sizeof(Vector3));
				stream.seek(bodyPartPos + modelPos + meshPos);

				auto& mesh = model.meshes.emplace_back();

				stream.read(mesh.material);
				stream.skip<int>();
				stream.read(mesh.verticesCount);
				stream.read(mesh.verticesOffset);
				stream.skip<int>(2);
				stream.read(mesh.materialType);
				stream.read(mesh.materialParam);
				stream.read(mesh.meshID);
				stream.read(mesh.center);
			}
		}
	}

	return true;
}
