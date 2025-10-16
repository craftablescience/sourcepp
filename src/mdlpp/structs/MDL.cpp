#include <mdlpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/parser/Binary.h>

using namespace mdlpp::MDL;
using namespace sourcepp;
using mdlpp::AnimValue;

constexpr int32_t MDL_ID = parser::binary::makeFourCC("IDST");

namespace {

// TODO: Decompress RLE animation data into frame values
void readAnimValueRLE(BufferStreamReadOnly& stream, int frameCount, std::vector<AnimValue>& outData) {
	AnimValue val{};
	int framesCovered = 0;
	while (framesCovered < frameCount) {
		stream.read(val.value);
		outData.push_back(val);

		if (val.num.valid == 0) {
			framesCovered += val.num.total;
			stream.read(val.value);
			outData.push_back(val);
		} else {
			// val.num.valid values, covering val.num.total frames
			const int numValues = val.num.valid;
			const int totalFrames = val.num.total;
			for (int v = 0; v < numValues; v++) {
				stream.read(val.value);
				outData.push_back(val);
			}
			framesCovered += totalFrames;
		}
	}
}

} // anonymous namespace

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

	const auto animDescCount = stream.read<int32_t>();
	const auto animDescOffset = stream.read<int32_t>();

	const auto sequenceDescCount = stream.read<int32_t>();
	const auto sequenceDescOffset = stream.read<int32_t>();

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

	const auto attachmentCount = stream.read<int32_t>();
	const auto attachmentOffset = stream.read<int32_t>();

	const auto localNodeCount = stream.read<int32_t>();
	const auto localNodeIndex = stream.read<int32_t>();
	const auto localNodeNameIndex = stream.read<int32_t>();

	const auto flexDescCount = stream.read<int32_t>();
	const auto flexDescIndex = stream.read<int32_t>();
	const auto flexControllerCount = stream.read<int32_t>();
	const auto flexControllerIndex = stream.read<int32_t>();
	const auto flexRulesCount = stream.read<int32_t>();
	const auto flexRulesIndex = stream.read<int32_t>();
	const auto ikChainCount = stream.read<int32_t>();
	const auto ikChainIndex = stream.read<int32_t>();

	const auto mouthsCount = stream.read<int32_t>();
	const auto mouthsIndex = stream.read<int32_t>();

	const auto localPoseParamCount = stream.read<int32_t>();
	const auto localPoseParamIndex = stream.read<int32_t>();

	const auto surfacePropertyIndex = stream.read<int32_t>();
	const auto keyValueIndex = stream.read<int32_t>();
	const auto keyValueCount = stream.read<int32_t>();

	const auto localIKAutoplayLockCount = stream.read<int32_t>();
	const auto localIKAutoplayLockIndex = stream.read<int32_t>();

	stream
		.read(this->mass)
		.read(this->contentsFlags);

	const auto includeModelCount = stream.read<int32_t>();
	const auto includeModelIndex = stream.read<int32_t>();

	stream.read(this->virtualModel);

	// do we want to parse these here?
	const auto animationBlocksNameIndex = stream.read<int32_t>();
	const auto animationBlocksCount = stream.read<int32_t>();
	const auto animationBlocksIndex = stream.read<int32_t>();

	stream.read(this->animationBlockModel);

	const auto boneTableNameIndex = stream.read<int32_t>();

	stream
		.read(this->vertexBase)
		.read(this->offsetBase)
		.read(this->directionalDotProduct)
		.read(this->rootLOD)
		.read(this->numAllowedRootLODs)
		.skip<uint8_t>() // _unused0
		.skip<int32_t>(); // _unused1

	const auto flexControllerUICount = stream.read<int32_t>();
	const auto flexControllerUIIndex = stream.read<int32_t>();

	stream
		.read(this->vertAnimFixedPointScale)
		.skip<int32_t>() // _unused2
		.read(this->studiohdr2index)
		.skip<int32_t>(); // _unused3

	// header of the second kind perchance
	if (this->studiohdr2index != 0) {
		stream.seek_u(this->studiohdr2index);
		// TODO: Parse this
		stream
			.read(this->header2.srcBoneTransformCount)
			.read(this->header2.srcBoneTransformIndex)
			.read(this->header2.illumPositionAttachmentIndex)
			.read(this->header2.maxEyeDeflection)
			.read(this->header2.linearBoneIndex)
			.read(this->header2.nameIndex)
			.read(this->header2.boneFlexDriverCount)
			.read(this->header2.boneFlexDriverIndex);
		// skip the 56 reserved int32 fields
		stream.skip<int32_t>(56);
		this->hasHeader2 = true;
	}

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
		// this is awesome
		parser::binary::readStringAtOffset(stream, bone.surfacePropName, std::ios::cur, sizeof(int32_t) * 12 + sizeof(math::Vec3f) * 4 + sizeof(math::Quat) * 2 + sizeof(math::Mat3x4f) + sizeof(Bone::Flags));
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
			// and this is unimportant, so skip parsing the bbox name here
			// readStringAtOffset(stream, hitbox.name, std::ios::cur, sizeof(int32_t) * 3 + sizeof(Vec3f) * 2);
			stream.skip<int32_t>();
			hitbox.name = "";

			// _unused0
			stream.skip<int32_t>(8);
		}
	}

	stream.seek(animDescOffset);
	for (int i = 0; i < animDescCount; i++) {
		const auto animDescPos = animDescOffset + i * (sizeof(int32_t) * 22 + sizeof(int16_t) * 2 + sizeof(float) * 2);
		stream.seek_u(animDescPos);

		auto& animDesc = this->animations.emplace_back();

		stream.skip<int32_t>(); // basePointer
		// animation name offsets are relative to the struct base
		// after the offset int32, we are 8 bytes past the structure base (4 for basePointer + 4 for offset)
		// so we subtract 8 instead of the default 4
		parser::binary::readStringAtOffset(stream, animDesc.name, std::ios::cur, 8);
		stream
			.read(animDesc.fps)
			.read(animDesc.flags)
			.read(animDesc.frameCount);

		const auto movementCount = stream.read<int32_t>();
		const auto movementIndex = stream.read<int32_t>();
		stream.skip<int32_t>(6);  // unused

		stream
			.read(animDesc.animBlock)
			.read(animDesc.animIndex);

		const auto ikRuleCount = stream.read<int32_t>();
		const auto ikRuleIndex = stream.read<int32_t>();

		// TODO: Parse all this strange stuff
		stream
			.read(animDesc.animBlockIKRuleIndex)
			.read(animDesc.localHierarchyCount)
			.read(animDesc.localHierarchyIndex)
			.read(animDesc.sectionIndex)
			.read(animDesc.sectionFrames)
			.read(animDesc.zeroFrameSpan)
			.read(animDesc.zeroFrameCount)
			.read(animDesc.zeroFrameIndex)
			.read(animDesc.zeroFrameStallTime);

		// TODO: Load external animations if animBlock != 0
		if (animDesc.animIndex != 0 && animDesc.animBlock == 0) {
			const auto animDataPos = animDescPos + animDesc.animIndex;
			const auto savedPos = stream.tell();
			stream.seek_u(animDataPos);

			// LL of mstudioanim_t structures
			// bone, flags, nextoffset (2 bytes), then var data
			while (true) {
				const auto boneAnimPos = stream.tell();

				auto& boneAnim = animDesc.boneAnimations.emplace_back();
				stream
					.read(boneAnim.bone)
					.read(boneAnim.flags);

				if (boneAnim.bone == 255) {
					animDesc.boneAnimations.pop_back();
					break;
				}

				const auto nextOffset = stream.read<int16_t>();

				// flags
				if (boneAnim.flags & AnimBoneData::FLAG_RAW_ROT2) {
					boneAnim.staticRotation64 = stream.read<math::QuatCompressed64>();
				} else if (boneAnim.flags & AnimBoneData::FLAG_RAW_ROT) {
					boneAnim.staticRotation48 = stream.read<math::QuatCompressed48>();
				}

				if (boneAnim.flags & AnimBoneData::FLAG_RAW_POS) {
					boneAnim.staticPosition = stream.read<math::Vec3Compressed48>();
				}

				// rotation
				if (boneAnim.flags & AnimBoneData::FLAG_ANIM_ROT) {
					AnimValuePtr rotPtr{};
					stream.read(rotPtr);
					boneAnim.animRotationPtr = rotPtr;

					const auto ptrPos = stream.tell();

					// pitch/yaw/roll
					for (short comp : rotPtr) {
						if (comp > 0) {
							stream.seek_u(ptrPos - sizeof(AnimValuePtr) + comp);
							readAnimValueRLE(stream, animDesc.frameCount, boneAnim.animRotationData);
						}
					}

					stream.seek_u(ptrPos);
				}

				// position
				if (boneAnim.flags & AnimBoneData::FLAG_ANIM_POS) {
					AnimValuePtr posPtr{};
					stream.read(posPtr);
					boneAnim.animPositionPtr = posPtr;

					const auto ptrPos = stream.tell();

					// x/y/z
					for (short comp : posPtr) {
						if (comp > 0) {
							stream.seek_u(ptrPos - sizeof(AnimValuePtr) + comp);
							readAnimValueRLE(stream, animDesc.frameCount, boneAnim.animPositionData);
						}
					}

					stream.seek_u(ptrPos);
				}

				if (nextOffset == 0) {
					break;
				}
				stream.seek_u(boneAnimPos + nextOffset);
			}

			stream.seek_u(savedPos);
		}

		if (movementCount > 0 && movementIndex != 0) {
			const auto movementDataPos = animDescPos + movementIndex;
			stream.seek_u(movementDataPos);

			for (int j = 0; j < movementCount; j++) {
				auto& movement = animDesc.movements.emplace_back();
				stream.read(movement.endFrame);

				movement.flags = static_cast<Movement::Flags>(stream.read<int32_t>());

				stream
					.read(movement.velocityStart)
					.read(movement.velocityEnd)
					.read(movement.yawEnd)
					.read(movement.movement)
					.read(movement.relativePosition);
			}
		}

		if (ikRuleCount > 0 && ikRuleIndex != 0 && animDesc.animBlock == 0) {
			const auto ikRuleDataPos = animDescPos + ikRuleIndex;
			stream.seek_u(ikRuleDataPos);

			for (int j = 0; j < ikRuleCount; j++) {
				const auto ikRulePos = ikRuleDataPos + j * (sizeof(int32_t) * 20 + sizeof(float) * 11 + sizeof(math::Vec3f) + sizeof(math::Quat));
				stream.seek_u(ikRulePos);

				auto& ikRule = animDesc.ikRules.emplace_back();

				stream
					.read(ikRule.index)
					.read(ikRule.type)
					.read(ikRule.chain)
					.read(ikRule.bone)
					.read(ikRule.slot)
					.read(ikRule.height)
					.read(ikRule.radius)
					.read(ikRule.floor)
					.read(ikRule.pos)
					.read(ikRule.q);

				const auto compressedIKErrorIndex = stream.read<int32_t>();
				stream.skip<int32_t>();  // unused2

				stream
					.read(ikRule.iStart);

				const auto ikErrorIndex = stream.read<int32_t>();
				// note: afaik crowbar has a bug when decompiling 1 frame anims ??
				// [0.0, 0.0, 1.0, 1.0] should be 0 0 1 1 but crowbar gives 0 0 0 0 (for the model I tested: bot_heavy from TF2)
				stream
					.read(ikRule.start)
					.read(ikRule.peak)
					.read(ikRule.tail)
					.read(ikRule.end)
					.skip<float>()  // unused3
					.read(ikRule.contact)
					.read(ikRule.drop)
					.read(ikRule.top)
					.skip<int32_t>()  // unused6
					.skip<int32_t>()  // unused7
					.skip<int32_t>(); // unused8

				parser::binary::readStringAtOffset(stream, ikRule.attachment);

				stream.skip<int32_t>(7);  // unused[7]

				// compressed IK error ?
				if (compressedIKErrorIndex != 0) {
					const auto compErrorPos = ikRulePos + compressedIKErrorIndex;
					const auto savedPos = stream.tell();
					stream.seek_u(compErrorPos);

					ikRule.compressedIKError = CompressedIKError{};
					stream
						.read(ikRule.compressedIKError->scale)
						.read(ikRule.compressedIKError->offset);

					const auto compErrorDataPos = stream.tell();
					for (short k : ikRule.compressedIKError->offset) {
						if (k > 0) {
							stream.seek_u(compErrorDataPos + k);
							readAnimValueRLE(stream, animDesc.frameCount, ikRule.compressedIKError->animValues);
						}
					}

					stream.seek_u(savedPos);
				}

				// uncompressed IK error !
				if (ikErrorIndex != 0) {
					const auto ikErrorPos = ikRulePos + ikErrorIndex;
					const auto savedPos = stream.tell();
					stream.seek_u(ikErrorPos);

					const int errorFrameCount = animDesc.frameCount - ikRule.iStart;
					ikRule.ikErrors.reserve(errorFrameCount);

					for (int k = 0; k < errorFrameCount; k++) {
						auto& ikError = ikRule.ikErrors.emplace_back();
						stream
							.read(ikError.position)
							.read(ikError.rotation);
					}

					stream.seek_u(savedPos);
				}
			}
		}
	}

	stream.seek(sequenceDescOffset);
	for (int i = 0; i < sequenceDescCount; i++) {
		const auto sequenceDescPos = sequenceDescOffset + i * (sizeof(int32_t) * 38 + sizeof(float) * 9 + sizeof(math::Vec3f) * 2);
		stream.seek_u(sequenceDescPos);

		auto& sequenceDesc = this->sequences.emplace_back();

		stream.skip<int32_t>(); // basePointer
		parser::binary::readStringAtOffset(stream, sequenceDesc.label, std::ios::cur, 8);
		parser::binary::readStringAtOffset(stream, sequenceDesc.activityName, std::ios::cur, 12);
		stream
			.read(sequenceDesc.flags)
			.read(sequenceDesc.activity)
			.read(sequenceDesc.activityWeight);

		const auto eventCount = stream.read<int32_t>();
		const auto eventIndex = stream.read<int32_t>();

		// TODO: Parse animIndexIndex
		// I think its an index to an array that maps the 2d blend to animation indices
		// note: dont quote me on that
		stream
			.read(sequenceDesc.boundingBoxMin)
			.read(sequenceDesc.boundingBoxMax)
			.read(sequenceDesc.blendCount)
			.read(sequenceDesc.animIndexIndex)
			.read(sequenceDesc.movementIndex)
			.read(sequenceDesc.groupSize)
			.read(sequenceDesc.paramIndex)
			.read(sequenceDesc.paramStart)
			.read(sequenceDesc.paramEnd)
			.read(sequenceDesc.paramParent)
			.read(sequenceDesc.fadeInTime)
			.read(sequenceDesc.fadeOutTime)
			.read(sequenceDesc.localEntryNode)
			.read(sequenceDesc.localExitNode)
			.read(sequenceDesc.nodeFlags)
			.read(sequenceDesc.entryPhase)
			.read(sequenceDesc.exitPhase)
			.read(sequenceDesc.lastFrame)
			.read(sequenceDesc.nextSequence)
			.read(sequenceDesc.pose)
			.read(sequenceDesc.ikRuleCount);

		const auto autoLayerCount = stream.read<int32_t>();
		const auto autoLayerIndex = stream.read<int32_t>();

		// TODO: Parse weightListIndex & poseKeyIndex
		stream
			.read(sequenceDesc.weightListIndex)
			.read(sequenceDesc.poseKeyIndex);

		const auto ikLockCount = stream.read<int32_t>();
		const auto ikLockIndex = stream.read<int32_t>();

		const auto seqKeyValueIndex = stream.read<int32_t>();
		const auto seqKeyValueSize = stream.read<int32_t>();

		// TODO: Parse cyclePoseIndex
		stream
			.read(sequenceDesc.cyclePoseIndex);

		stream
			.skip<int32_t>(2) // activitymodifierindex, numactivitymodifiers
			.skip<int32_t>(5); // unused

		if (ikLockCount > 0 && ikLockIndex != 0) {
			const auto ikLockDataPos = sequenceDescPos + ikLockIndex;
			stream.seek_u(ikLockDataPos);

			for (int j = 0; j < ikLockCount; j++) {
				auto& ikLock = sequenceDesc.ikLocks.emplace_back();
				stream
					.read(ikLock.chain)
					.read(ikLock.posWeight)
					.read(ikLock.localQWeight)
					.read(ikLock.flags);
				stream.skip<int32_t>(4);  // unused[4]
			}
		}

		if (eventCount > 0 && eventIndex != 0) {
			const auto eventDataPos = sequenceDescPos + eventIndex;
			stream.seek_u(eventDataPos);

			for (int j = 0; j < eventCount; j++) {
				auto& event = sequenceDesc.events.emplace_back();

				stream
					.read(event.cycle)
					.read(event.event)
					.read(event.type)
					.read(event.options);
				
				parser::binary::readStringAtOffset(stream, event.eventName, std::ios::cur,
					sizeof(float) + sizeof(int32_t) * 2 + 64);
			}
		}

		if (autoLayerCount > 0 && autoLayerIndex != 0) {
			const auto autoLayerDataPos = sequenceDescPos + autoLayerIndex;
			stream.seek_u(autoLayerDataPos);

			for (int j = 0; j < autoLayerCount; j++) {
				auto& autoLayer = sequenceDesc.autoLayers.emplace_back();
				stream
					.read(autoLayer.sequence)
					.read(autoLayer.pose)
					.read(autoLayer.flags)
					.read(autoLayer.start)
					.read(autoLayer.peak)
					.read(autoLayer.tail)
					.read(autoLayer.end);
			}
		}

		if (seqKeyValueSize > 0 && seqKeyValueIndex != 0) {
			const auto seqKeyValueDataPos = sequenceDescPos + seqKeyValueIndex;
			stream.seek_u(seqKeyValueDataPos);
			stream.read(sequenceDesc.keyValues, seqKeyValueSize - 1, false); // seqKeyValueSize includes null terminator
		}
	}

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
		parser::binary::readStringAtOffset(stream, materialDir, std::ios::beg, 0);
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
				.read(model.verticesOffset)
				.read(model.tangentsOffset);

			const auto modelAttachmentsCount = stream.read<int32_t>();
			const auto modelAttachmentsOffset = stream.read<int32_t>();

			const auto eyeballsCount = stream.read<int32_t>();
			const auto eyeballsOffset = stream.read<int32_t>();

			for (int k = 0; k < meshesCount; k++) {
				const auto meshPos = meshesOffset + k * (sizeof(int32_t) * (18 + MAX_LOD_COUNT) + sizeof(math::Vec3f));
				stream.seek_u(bodyPartPos + modelPos + meshPos);

				auto& mesh = model.meshes.emplace_back();

				stream
					.read(mesh.material)
					.skip<int32_t>()
					.read(mesh.verticesCount)
					.read(mesh.verticesOffset);

				const auto meshFlexCount = stream.read<int32_t>();
				const auto meshFlexOffset = stream.read<int32_t>();

				stream
					.read(mesh.materialType)
					.read(mesh.materialParam)
					.read(mesh.meshID)
					.read(mesh.center)
					.read(mesh.modelVertexData)
					.read(mesh.numLODVertexes);

				stream.skip<int32_t>(8); // unused

				if (meshFlexCount > 0 && meshFlexOffset != 0) {

					for (int m = 0; m < meshFlexCount; m++) {
						const auto flexPos = meshFlexOffset + m * (sizeof(int32_t) * 10 + sizeof(float) * 4 + sizeof(uint8_t) * 4);
						stream.seek_u(bodyPartPos + modelPos + meshPos + flexPos);

						auto& flex = mesh.flexes.emplace_back();

						stream
							.read(flex.flexdesc)
							.read(flex.target0)
							.read(flex.target1)
							.read(flex.target2)
							.read(flex.target3);

						const auto numverts = stream.read<int32_t>();
						const auto vertindex = stream.read<int32_t>();

						stream
							.read(flex.flexpair)
							.read(flex.vertanimtype);

						if (numverts > 0 && vertindex != 0) {
							// vertindex is relative to the flex structure base
							const auto vertAnimPos = bodyPartPos + modelPos + meshPos + flexPos + vertindex;
							stream.seek_u(vertAnimPos);

							if (flex.vertanimtype == 0) {
								// normal vertex animations
								flex.vertAnims.reserve(numverts);
								for (int n = 0; n < numverts; n++) {
									auto& vertAnim = flex.vertAnims.emplace_back();
									stream
										.read(vertAnim.index)
										.read(vertAnim.speed)
										.read(vertAnim.side)
										.read(vertAnim.delta)
										.read(vertAnim.ndelta);
								}
							} else if (flex.vertanimtype == 1) {
								// wrinkle vertex animations
								flex.vertAnimsWrinkle.reserve(numverts);
								for (int n = 0; n < numverts; n++) {
									auto& vertAnim = flex.vertAnimsWrinkle.emplace_back();
									stream
										.read(vertAnim.index)
										.read(vertAnim.speed)
										.read(vertAnim.side)
										.read(vertAnim.delta)
										.read(vertAnim.ndelta)
										.read(vertAnim.wrinkledelta);
								}
							}
						}
					}
				}
			}

			if (eyeballsCount > 0 && eyeballsOffset != 0) {
				for (int k = 0; k < eyeballsCount; k++) {
					const auto eyeballPos = eyeballsOffset + k * (sizeof(int32_t) * 24 + sizeof(float) * 9 + sizeof(math::Vec3f) * 3 + sizeof(uint8_t) * 4);
					stream.seek_u(bodyPartPos + modelPos + eyeballPos);

					auto& eyeball = model.eyeballs.emplace_back();

					// note: eyeball names are not stored in compiled MDL files ?
					// sznameindex exists in the struct but is never populated by studiomdl
					stream.skip<int32_t>();  // sznameindex

					stream
						.read(eyeball.bone)
						.read(eyeball.org)
						.read(eyeball.zoffset)
						.read(eyeball.radius)
						.read(eyeball.up)
						.read(eyeball.forward)
						.read(eyeball.texture)
						.skip<int32_t>()  // unused1
						.read(eyeball.iris_scale)
						.skip<int32_t>()  // unused2
						.read(eyeball.upperflexdesc)
						.read(eyeball.lowerflexdesc)
						.read(eyeball.uppertarget)
						.read(eyeball.lowertarget)
						.read(eyeball.upperlidflexdesc)
						.read(eyeball.lowerlidflexdesc);

					stream.skip<int32_t>(4);  // unused[4]
					stream.read(eyeball.m_bNonFACS);
					stream.skip<uint8_t>(3);  // unused3[3]
					stream.skip<int32_t>(7);  // unused4[7]
				}
			}

			if (modelAttachmentsCount > 0 && modelAttachmentsOffset != 0) {
				for (int k = 0; k < modelAttachmentsCount; k++) {
					const auto attachmentPos = modelAttachmentsOffset + k * (sizeof(int32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(math::Mat3x4f) + sizeof(int32_t) * 8);
					stream.seek_u(bodyPartPos + modelPos + attachmentPos);

					auto& attachment = model.attachments.emplace_back();

					parser::binary::readStringAtOffset(stream, attachment.name);

					stream
						.skip<uint32_t>() // flags
						.read(attachment.bone)
						.read(attachment.localMatrix);

					// extract position from the last column of the matrix
					attachment.position[0] = attachment.localMatrix[0][3];
					attachment.position[1] = attachment.localMatrix[1][3];
					attachment.position[2] = attachment.localMatrix[2][3];

					stream.skip<int32_t>(8); // unused
				}
			}
		}
	}

	stream.seek(attachmentOffset);
	for (int i = 0; i < attachmentCount; i++) {
		auto& attachment = this->attachments.emplace_back();

		parser::binary::readStringAtOffset(stream, attachment.name);

		stream
			.skip<int32_t>() // flags
			.read(attachment.bone)
			.read(attachment.localMatrix);

		// ditto
		attachment.position[0] = attachment.localMatrix[0][3];
		attachment.position[1] = attachment.localMatrix[1][3];
		attachment.position[2] = attachment.localMatrix[2][3];


		stream.skip<int32_t>(8); // unused
	}

	if (localNodeCount > 0 && localNodeIndex != 0 && localNodeNameIndex != 0) {
		stream.seek(localNodeNameIndex);
		this->localNodeNames.reserve(localNodeCount);
		for (int i = 0; i < localNodeCount; i++) {
			auto& nodeName = this->localNodeNames.emplace_back();
			parser::binary::readStringAtOffset(stream, nodeName, std::ios::beg, 0);
		}

		// transition matrix (localNodeCount × localNodeCount bytes)
		stream.seek(localNodeIndex);
		const auto transitionMatrixSize = localNodeCount * localNodeCount;
		this->localNodeTransitions.resize(transitionMatrixSize);
		for (int i = 0; i < transitionMatrixSize; i++) {
			this->localNodeTransitions[i] = stream.read<uint8_t>();
		}
	}

	if (localPoseParamCount > 0 && localPoseParamIndex != 0) {
		stream.seek(localPoseParamIndex);

		for (int i = 0; i < localPoseParamCount; i++) {
			auto& poseParam = this->poseParameters.emplace_back();
			parser::binary::readStringAtOffset(stream, poseParam.name);
			stream
				.read(poseParam.flags)
				.read(poseParam.start)
				.read(poseParam.end)
				.read(poseParam.loop);
		}
	}

	if (includeModelCount > 0 && includeModelIndex != 0) {
		for (int i = 0; i < includeModelCount; i++) {
			const auto includeModelPos = includeModelIndex + i * (sizeof(int32_t) * 2);
			stream.seek_u(includeModelPos);

			auto& includeModel = this->includeModels.emplace_back();

			// lambda to read at relative offset
			auto readStringAtRelativeOffset = [&](std::string& target) {
				if (const auto offset = stream.read<int32_t>(); offset != 0) {
					const auto savedPos = stream.tell();
					stream.seek_u(includeModelPos + offset);
					stream.read(target);
					stream.seek_u(savedPos);
				}
			};

			readStringAtRelativeOffset(includeModel.label);
			readStringAtRelativeOffset(includeModel.name);
		}
	}

	if (flexDescCount > 0 && flexDescIndex != 0) {
		stream.seek(flexDescIndex);

		for (int i = 0; i < flexDescCount; i++) {
			auto& flexDescName = this->flexDescs.emplace_back();
			parser::binary::readStringAtOffset(stream, flexDescName);
		}
	}

	if (flexControllerCount > 0 && flexControllerIndex != 0) {
		for (int i = 0; i < flexControllerCount; i++) {
			const auto flexControllerPos = flexControllerIndex + i * (sizeof(int32_t) * 3 + sizeof(float) * 2);
			stream.seek_u(flexControllerPos);

			auto& flexController = this->flexControllers.emplace_back();

			// ditto
			auto readStringAtRelativeOffset = [&](std::string& target) {
				if (const auto offset = stream.read<int32_t>(); offset != 0) {
					const auto savedPos = stream.tell();
					stream.seek_u(flexControllerPos + offset);
					stream.read(target);
					stream.seek_u(savedPos);
				}
			};

			readStringAtRelativeOffset(flexController.type);
			readStringAtRelativeOffset(flexController.name);

			stream
				.read(flexController.localToGlobal)
				.read(flexController.min)
				.read(flexController.max);
		}
	}

	if (ikChainCount > 0 && ikChainIndex != 0) {
		for (int i = 0; i < ikChainCount; i++) {
			const auto ikChainPos = ikChainIndex + i * (sizeof(int32_t) * 4);
			stream.seek_u(ikChainPos);

			auto& ikChain = this->ikChains.emplace_back();
			parser::binary::readStringAtOffset(stream, ikChain.name);
			stream.read(ikChain.linkType);

			const auto linkCount = stream.read<int32_t>();

			if (const auto linkIndex = stream.read<int32_t>(); linkCount > 0 && linkIndex != 0) {
				const auto linkDataPos = ikChainPos + linkIndex;
				stream.seek_u(linkDataPos);

				for (int j = 0; j < linkCount; j++) {
					auto& link = ikChain.links.emplace_back();
					stream
						.read(link.bone)
						.read(link.kneeDir)
						.skip<math::Vec3f>();  // unused
				}
			}
		}
	}

	if (flexRulesCount > 0 && flexRulesIndex != 0) {
		for (int i = 0; i < flexRulesCount; i++) {
			const auto flexRulePos = flexRulesIndex + i * (sizeof(int32_t) * 3);
			stream.seek_u(flexRulePos);

			auto& flexRule = this->flexRules.emplace_back();
			stream.read(flexRule.flex);

			const auto opCount = stream.read<int32_t>();

			if (const auto opIndex = stream.read<int32_t>(); opCount > 0 && opIndex != 0) {
				const auto opDataPos = flexRulePos + opIndex;
				stream.seek_u(opDataPos);

				for (int j = 0; j < opCount; j++) {
					auto& op = flexRule.ops.emplace_back();
					stream
						.read(op.op)
						.read(op.d.index);
				}
			}
		}
	}

	if (mouthsCount > 0 && mouthsIndex != 0) {
		stream.seek(mouthsIndex);

		for (int i = 0; i < mouthsCount; i++) {
			auto& mouth = this->mouths.emplace_back();
			stream
				.read(mouth.bone)
				.read(mouth.forward)
				.read(mouth.flexdesc);
		}
	}

	if (keyValueCount > 0 && keyValueIndex != 0) {
		stream.seek(keyValueIndex);
		stream.read(this->keyValues, keyValueCount - 1, false); // keyValueCount includes null terminator
	}

	if (surfacePropertyIndex != 0) {
		stream.seek(surfacePropertyIndex);
		stream.read(this->surfaceProperty);
	}

	if (localIKAutoplayLockCount > 0 && localIKAutoplayLockIndex != 0) {
		stream.seek(localIKAutoplayLockIndex);

		for (int i = 0; i < localIKAutoplayLockCount; i++) {
			auto& ikLock = this->ikAutoplayLocks.emplace_back();
			stream
				.read(ikLock.chain)
				.read(ikLock.posWeight)
				.read(ikLock.localQWeight)
				.read(ikLock.flags);
			stream.skip<int32_t>(4);  // unused[4]
		}
	}

	if (boneTableNameIndex != 0 && boneCount > 0) {
		stream.seek(boneTableNameIndex);
		this->boneTableByName.reserve(boneCount);
		for (int i = 0; i < boneCount; i++) {
			this->boneTableByName.push_back(stream.read<uint8_t>());
		}
	}

	if (animationBlocksNameIndex != 0) {
		stream.seek(animationBlocksNameIndex);
		stream.read(this->animationBlocksName);
	}

	if (animationBlocksCount > 0 && animationBlocksIndex != 0) {
		stream.seek(animationBlocksIndex);

		for (int i = 0; i < animationBlocksCount; i++) {
			auto& animBlock = this->animationBlocks.emplace_back();
			stream
				.read(animBlock.dataStart)
				.read(animBlock.dataEnd);
		}
	}

	if (flexControllerUICount > 0 && flexControllerUIIndex != 0) {
		for (int i = 0; i < flexControllerUICount; i++) {
			const auto flexControllerUIPos = flexControllerUIIndex + i * (sizeof(int32_t) * 4 + sizeof(uint8_t) * 4);
			stream.seek_u(flexControllerUIPos);

			auto& flexControllerUI = this->flexControllerUIs.emplace_back();

			parser::binary::readStringAtOffset(stream, flexControllerUI.name);

			const auto szindex0 = stream.read<int32_t>();
			const auto szindex1 = stream.read<int32_t>();
			const auto szindex2 = stream.read<int32_t>();

			stream
				.read(flexControllerUI.remaptype)
				.read(flexControllerUI.stereo);

			stream.skip<uint8_t>(2); // unused

			// lambda to resolve controller name from szindex offset
			auto resolveControllerName = [&](int32_t szindex, std::string& targetName) {
				if (szindex != 0) {
					const auto savedPos = stream.tell();
					stream.seek_u(flexControllerUIPos + szindex);
					stream.skip<int32_t>();
					if (const auto controllerNameOffset = stream.read<int32_t>(); controllerNameOffset != 0) {
						stream.seek_u(flexControllerUIPos + szindex + controllerNameOffset);
						stream.read(targetName);
					}
					stream.seek_u(savedPos);
				}
			};

			resolveControllerName(szindex0, flexControllerUI.controllerName0);
			resolveControllerName(szindex1, flexControllerUI.controllerName1);
			resolveControllerName(szindex2, flexControllerUI.controllerName2);
		}
	}

	return true;
}
