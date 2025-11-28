#include <mdlpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/parser/Binary.h>

using namespace mdlpp::MDL;
using namespace sourcepp;
using mdlpp::AnimValue;

constexpr int32_t MDL_ID = parser::binary::makeFourCC("IDST");

namespace {

// boundary check before seeking
bool seekAndValidate(BufferStreamReadOnly& stream, const uint64_t offset) {
	if (offset >= stream.size()) {
		return false;
	}
	stream.seek_u(offset);
	return true;
}

// TODO: Decompress RLE animation data into frame values
// TODO: Make lambda return bool for the boundary check?
void readAnimValueRLE(BufferStreamReadOnly& stream, const int frameCount, std::vector<AnimValue>& outData) {
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

} // namespace

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
	stream.skip<int32_t>(); // virtualModel

	// do we want to parse these here?
	const auto animationBlocksNameIndex = stream.read<int32_t>();
	const auto animationBlocksCount = stream.read<int32_t>();
	const auto animationBlocksIndex = stream.read<int32_t>();
	stream.skip<int32_t>(); // animationBlockModel

	const auto boneTableNameIndex = stream.read<int32_t>();

	stream
		.skip<int32_t>() // vertexBase
		.skip<int32_t>() // offsetBase
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
		.read(this->studioHdr2Index)
		.skip<int32_t>(); // _unused3

	// header 2
	// Note: some models have weird (dynamic??) header2 data with offsets (and size) pointing outside the file.
	// Unlike the rest of this parser, I validate offset AND size, then skip invalid sections
	// rather than failing the whole file. (Might want to change this). - cueki Nov 27, 2025
	if (this->studioHdr2Index != 0) {
		stream.seek_u(this->studioHdr2Index)
			.read(this->header2.srcBoneTransformCount)
			.read(this->header2.srcBoneTransformIndex)
			.read(this->header2.illumPositionAttachmentIndex)
			.read(this->header2.maxEyeDeflection)
			.read(this->header2.linearBoneIndex)
			.read(this->header2.nameIndex)
			.read(this->header2.boneFlexDriverCount)
			.read(this->header2.boneFlexDriverIndex);
		stream.skip<int32_t>(56); // reserved
		this->hasHeader2 = true;

		for (int i = 0; i < this->header2.srcBoneTransformCount; i++) {
			const auto srcBoneTransformPos = this->studioHdr2Index + this->header2.srcBoneTransformIndex + i * (sizeof(int32_t) + sizeof(math::Mat3x4f) * 2);
			if (srcBoneTransformPos + (sizeof(int32_t) + sizeof(math::Mat3x4f) * 2) > stream.size()) {
				break;
			}
			auto& srcBoneTransform = this->srcBoneTransforms.emplace_back();

			stream.seek_u(srcBoneTransformPos);
			const auto nameOffset = stream.read<int32_t>();
			const auto nameAbsOffset = srcBoneTransformPos + nameOffset;
			if (nameOffset != 0 && nameAbsOffset > 0 && nameAbsOffset < stream.size()) {
				stream.seek_u(nameAbsOffset)
					.read(srcBoneTransform.name)
					.seek_u(srcBoneTransformPos + sizeof(int32_t));
			}

			stream
				.read(srcBoneTransform.pretransform)
				.read(srcBoneTransform.posttransform);
		}

		if (this->header2.linearBoneIndex != 0) {
			const auto linearBoneOffset = this->studioHdr2Index + this->header2.linearBoneIndex;
			if (linearBoneOffset + sizeof(int32_t) * 16 <= stream.size()) {
				this->linearBone = LinearBone{};
				auto& lb = *this->linearBone;

				stream.seek_u(linearBoneOffset)
					.read(lb.boneCount);

				const auto flagsIndex = stream.read<int32_t>();
				const auto parentIndex = stream.read<int32_t>();
				const auto posIndex = stream.read<int32_t>();
				const auto quatIndex = stream.read<int32_t>();
				const auto rotIndex = stream.read<int32_t>();
				const auto poseToBoneIndex = stream.read<int32_t>();
				const auto posScaleIndex = stream.read<int32_t>();
				const auto rotScaleIndex = stream.read<int32_t>();
				const auto qAlignmentIndex = stream.read<int32_t>();
				stream.skip<int32_t>(6); // unused[6]

				// we are reading from a struct of arrays instead of an array of structs
				stream.seek_u(linearBoneOffset + flagsIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.flags.push_back(stream.read<int32_t>());
				}

				stream.seek_u(linearBoneOffset + parentIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.parent.push_back(stream.read<int32_t>());
				}

				stream.seek_u(linearBoneOffset + posIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.position.push_back(stream.read<math::Vec3f>());
				}

				stream.seek_u(linearBoneOffset + quatIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.quaternion.push_back(stream.read<math::Quat>());
				}

				stream.seek_u(linearBoneOffset + rotIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.rotation.push_back(stream.read<math::Vec3f>());
				}

				stream.seek_u(linearBoneOffset + poseToBoneIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.poseToBone.push_back(stream.read<math::Mat3x4f>());
				}

				stream.seek_u(linearBoneOffset + posScaleIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.positionScale.push_back(stream.read<math::Vec3f>());
				}

				stream.seek_u(linearBoneOffset + rotScaleIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.rotationScale.push_back(stream.read<math::Vec3f>());
				}

				stream.seek_u(linearBoneOffset + qAlignmentIndex);
				for (int i = 0; i < lb.boneCount; i++) {
					lb.quaternionAlignment.push_back(stream.read<math::Quat>());
				}
			}
		}

		for (int i = 0; i < this->header2.boneFlexDriverCount; i++) {
			const auto boneFlexDriverPos = this->studioHdr2Index + this->header2.boneFlexDriverIndex + i * (sizeof(int32_t) * 6);
			if (boneFlexDriverPos + (sizeof(int32_t) * 6) > stream.size()) {
				break;
			}
			auto& boneFlexDriver = this->boneFlexDrivers.emplace_back();
			stream.seek_u(boneFlexDriverPos)
				.read(boneFlexDriver.boneIndex);

			const auto controlCount = stream.read<int32_t>();
			const auto controlIndex = stream.read<int32_t>();
			stream.skip<int32_t>(3); // unused[3]

			for (int j = 0; j < controlCount; j++) {
				const auto controlPos = boneFlexDriverPos + controlIndex + j * (sizeof(int32_t) * 2 + sizeof(float) * 2);
				if (controlPos + (sizeof(int32_t) * 2 + sizeof(float) * 2) > stream.size()) {
					break;
				}
				auto& control = boneFlexDriver.controls.emplace_back();
				stream.seek_u(controlPos)
					.read(control.boneComponent)
					.read(control.flexControllerIndex)
					.read(control.min)
					.read(control.max);
			}
		}
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

		// TODO: Parse procedural bone data at procIndex
		// - STUDIO_PROC_AXISINTERP (1): mstudioaxisinterpbone_t - 152 bytes
		// - STUDIO_PROC_QUATINTERP (2): mstudioquatinterpbone_t - 12 bytes + trigger array
		// - STUDIO_PROC_AIMATBONE (3): mstudioaimatbone_t - 44 bytes
		// - STUDIO_PROC_AIMATATTACH (4): mstudioaimatbone_t - 44 bytes
		// - STUDIO_PROC_JIGGLE (5): mstudiojigglebone_t - 140 bytes (lol)

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
			if (!seekAndValidate(stream, hitboxSetPos + hitboxPos)) {
				return false;
			}

			auto& hitbox = hitboxSet.hitboxes.emplace_back();

			stream
				.read(hitbox.bone)
				.read(hitbox.group)
				.read(hitbox.bboxMin)
				.read(hitbox.bboxMax);

			// note: we don't know what model versions use absolute vs. relative offsets here
			// and this is unimportant, so skip parsing the bbox name here
			//readStringAtOffset(stream, hitbox.name, std::ios::cur, sizeof(int32_t) * 3 + sizeof(Vec3f) * 2);
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

		stream.read(animDesc.animBlockIKRuleIndex);

		const auto localHierarchyCount = stream.read<int32_t>();
		const auto localHierarchyIndex = stream.read<int32_t>();

		const auto sectionIndex = stream.read<int32_t>();
		stream.read(animDesc.sectionFrames);

		// TODO: Parse zeroframe bone data at zeroFrameIndex when animBlock != 0
		// For each bone with BONE_HAS_SAVEFRAME_POS: read zeroFrameCount * Vector48
		// For each bone with BONE_HAS_SAVEFRAME_ROT: read zeroFrameCount * Quaternion64
		stream.read(animDesc.zeroFrameSpan)
			.read(animDesc.zeroFrameCount)
			.read(animDesc.zeroFrameIndex)
			.read(animDesc.zeroFrameStallTime);

		// TODO: Load external animations from external file when animBlock != 0
		// Animation data, IK rules (animBlockIKRuleIndex), and local hierarchy are in external blocks
		if (animDesc.animIndex != 0 && animDesc.animBlock == 0) {
			const auto animDataPos = animDescPos + animDesc.animIndex;
			if (!seekAndValidate(stream, animDataPos)) {
				return false;
			}
			const auto savedPos = stream.tell();

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
					boneAnim.staticRotation = stream.read<math::QuatCompressed64>();
				} else if (boneAnim.flags & AnimBoneData::FLAG_RAW_ROT) {
					boneAnim.staticRotation = stream.read<math::QuatCompressed48>();
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
					for (uint8_t idx = 0; idx < rotPtr.size(); idx++) {
						const auto comp = rotPtr[idx];
						if (comp > 0) {
							if (!seekAndValidate(stream, ptrPos - sizeof(AnimValuePtr) + comp)) {
								return false;
							}
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
					for (uint8_t idx = 0; idx < posPtr.size(); idx++) {
						const auto comp = posPtr[idx];
						if (comp > 0) {
							if (!seekAndValidate(stream, ptrPos - sizeof(AnimValuePtr) + comp)) {
								return false;
							}
							readAnimValueRLE(stream, animDesc.frameCount, boneAnim.animPositionData);
						}
					}

					stream.seek_u(ptrPos);
				}

				if (nextOffset == 0) {
					break;
				}
				if (!seekAndValidate(stream, boneAnimPos + nextOffset)) {
					return false;
				}
			}

			stream.seek_u(savedPos);
		}

		const auto movementDataPos = animDescPos + movementIndex;
		if (!seekAndValidate(stream, movementDataPos)) {
			return false;
		}

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

		if (ikRuleIndex != 0 && animDesc.animBlock == 0) {
			const auto ikRuleDataPos = animDescPos + ikRuleIndex;
			if (!seekAndValidate(stream, ikRuleDataPos)) {
				return false;
			}

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

				// compressed IK error
				if (compressedIKErrorIndex != 0) {
					const auto compErrorPos = ikRulePos + compressedIKErrorIndex;
					const auto savedPos = stream.tell();
					stream.seek_u(compErrorPos);

					ikRule.compressedIKError = CompressedIKError{};
					stream
						.read(ikRule.compressedIKError->scale)
						.read(ikRule.compressedIKError->offset);

					const auto compErrorDataPos = stream.tell();
					for (uint8_t idx = 0; idx < ikRule.compressedIKError->offset.size(); idx++) {
						const auto k = ikRule.compressedIKError->offset[idx];
						if (k > 0) {
							if (!seekAndValidate(stream, compErrorDataPos + k)) {
								return false;
							}
							readAnimValueRLE(stream, animDesc.frameCount, ikRule.compressedIKError->animValues);
						}
					}

					stream.seek_u(savedPos);
				}

				// uncompressed IK error
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

		if (localHierarchyIndex != 0 && animDesc.animBlock == 0) {
			for (int j = 0; j < localHierarchyCount; j++) {
				const auto localHierarchyPos = animDescPos + localHierarchyIndex + j * (sizeof(int32_t) * 2 + sizeof(float) * 4 + sizeof(int32_t) * 6);
				stream.seek_u(localHierarchyPos);

				auto& localHierarchy = animDesc.localHierarchies.emplace_back();
				stream
					.read(localHierarchy.bone)
					.read(localHierarchy.newParent)
					.read(localHierarchy.start)
					.read(localHierarchy.peak)
					.read(localHierarchy.tail)
					.read(localHierarchy.end)
					.read(localHierarchy.startFrame);

				const auto localAnimIndex = stream.read<int32_t>();
				stream.skip<int32_t>(4); // unused[4]

				if (localAnimIndex != 0) {
					const auto compErrorPos = localHierarchyPos + localAnimIndex;
					stream.seek_u(compErrorPos);

					localHierarchy.compressedIKError = CompressedIKError{};
					stream
						.read(localHierarchy.compressedIKError->scale)
						.read(localHierarchy.compressedIKError->offset);

					const auto compErrorDataPos = stream.tell();
					for (uint8_t idx = 0; idx < localHierarchy.compressedIKError->offset.size(); idx++) {
						const auto k = localHierarchy.compressedIKError->offset[idx];
						if (k > 0) {
							if (!seekAndValidate(stream, compErrorDataPos + k)) {
								return false;
							}
							readAnimValueRLE(stream, animDesc.frameCount, localHierarchy.compressedIKError->animValues);
						}
					}
				}
			}
		}

		if (sectionIndex != 0 && animDesc.sectionFrames > 0) {
			// NOTE: numsections is not stored in the file, only in studiomdl's internal s_animation_t.
			// So I cannot guarantee this formula is correct - cueki
			const int sectionCount = animDesc.frameCount / animDesc.sectionFrames + 2;
			for (int j = 0; j < sectionCount; j++) {
				const auto sectionPos = animDescPos + sectionIndex + j * sizeof(AnimSection);
				stream.seek_u(sectionPos);

				auto& section = animDesc.sections.emplace_back();
				stream
					.read(section.animBlock)
					.read(section.animIndex);
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

		stream
			.read(sequenceDesc.boundingBoxMin)
			.read(sequenceDesc.boundingBoxMax)
			.read(sequenceDesc.blendCount);

		const auto animIndexIndex = stream.read<int32_t>();

		stream
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

		const auto weightListIndex = stream.read<int32_t>();
		const auto poseKeyIndex = stream.read<int32_t>();

		const auto ikLockCount = stream.read<int32_t>();
		const auto ikLockIndex = stream.read<int32_t>();

		const auto seqKeyValueIndex = stream.read<int32_t>();
		const auto seqKeyValueSize = stream.read<int32_t>();

		stream.read(sequenceDesc.cyclePoseIndex);

		const auto activityModifierIndex = stream.read<int32_t>();
		const auto activityModifierCount = stream.read<int32_t>();

		stream.skip<int32_t>(5); // unused[5]

		if (animIndexIndex != 0 && sequenceDesc.groupSize[0] > 0 && sequenceDesc.groupSize[1] > 0) {
			stream.seek_u(sequenceDescPos + animIndexIndex);
			const int blendCount = sequenceDesc.groupSize[0] * sequenceDesc.groupSize[1];
			for (int j = 0; j < blendCount; j++) {
				sequenceDesc.animIndices.push_back(stream.read<int16_t>());
			}
		}

		// if boneCount is not set this will be garbage data so we check for it
		if (weightListIndex != 0 && boneCount > 0) {
			stream.seek_u(sequenceDescPos + weightListIndex);
			for (int j = 0; j < boneCount; j++) {
				sequenceDesc.boneWeights.push_back(stream.read<float>());
			}
		}

		if (poseKeyIndex != 0) {
			stream.seek_u(sequenceDescPos + poseKeyIndex);
			// pPoseKey(iParam, iAnim) indexes as iParam * groupsize[0] + iAnim
			// iParam is 0 or 1 (2 params), iAnim is 0 to groupsize[0]-1
			// So total keys = 2 * groupSize[0]
			const int poseKeyCount = 2 * sequenceDesc.groupSize[0];
			for (int j = 0; j < poseKeyCount; j++) {
				sequenceDesc.poseKeys.push_back(stream.read<float>());
			}
		}

		if (activityModifierIndex != 0) {
			for (int j = 0; j < activityModifierCount; j++) {
				const auto modifierPos = sequenceDescPos + activityModifierIndex + j * sizeof(int32_t);
				stream.seek_u(modifierPos);
				auto& modifier = sequenceDesc.activityModifiers.emplace_back();
				parser::binary::readStringAtOffset(stream, modifier);
			}
		}

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

		const auto eventDataPos = sequenceDescPos + eventIndex;
		stream.seek_u(eventDataPos);

		for (int j = 0; j < eventCount; j++) {
			auto& event = sequenceDesc.events.emplace_back();

			stream
				.read(event.cycle)
				.read(event.event)
				.read(event.type)
				.read(event.options, 64);

			parser::binary::readStringAtOffset(stream, event.eventName, std::ios::cur,
				sizeof(float) + sizeof(int32_t) * 2 + 64);
		}

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

		if (seqKeyValueSize > 0 && seqKeyValueIndex != 0) {
			const auto seqKeyValueDataPos = sequenceDescPos + seqKeyValueIndex;
			stream.seek_u(seqKeyValueDataPos).read(sequenceDesc.keyValues, seqKeyValueSize);
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
		stream.read(bodyPart.base);
		const auto modelsOffset = stream.read<int32_t>();

		for (int j = 0; j < modelsCount; j++) {
			auto modelPos = modelsOffset + j * (64 + sizeof(float) + sizeof(int32_t) * 20);
			if (!seekAndValidate(stream, bodyPartPos + modelPos)) {
				return false;
			}

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
				if (!seekAndValidate(stream, bodyPartPos + modelPos + meshPos)) {
					return false;
				}

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

				if (meshFlexOffset != 0) {

					for (int m = 0; m < meshFlexCount; m++) {
						const auto flexPos = meshFlexOffset + m * (sizeof(int32_t) * 10 + sizeof(float) * 4 + sizeof(uint8_t) * 4);
						if (!seekAndValidate(stream, bodyPartPos + modelPos + meshPos + flexPos)) {
							return false;
						}

						auto& flex = mesh.flexes.emplace_back();

						stream
							.read(flex.flexDescIndex)
							.read(flex.target0)
							.read(flex.target1)
							.read(flex.target2)
							.read(flex.target3);

						const auto numverts = stream.read<int32_t>();
						const auto vertindex = stream.read<int32_t>();

						stream
							.read(flex.flexPair)
							.read(flex.vertAnimType);

						if (vertindex != 0) {
							// vertindex is relative to the flex structure base
							const auto vertAnimPos = bodyPartPos + modelPos + meshPos + flexPos + vertindex;
							if (!seekAndValidate(stream, vertAnimPos)) {
								return false;
							}

							if (flex.vertAnimType == 0) {
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
							} else if (flex.vertAnimType == 1) {
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
										.read(vertAnim.wrinkleDelta);
								}
							}
						}
					}
				}
			}

			if (eyeballsOffset != 0) {
				for (int k = 0; k < eyeballsCount; k++) {
					const auto eyeballPos = eyeballsOffset + k * (sizeof(int32_t) * 24 + sizeof(float) * 9 + sizeof(math::Vec3f) * 3 + sizeof(uint8_t) * 4);
					if (!seekAndValidate(stream, bodyPartPos + modelPos + eyeballPos)) {
						return false;
					}

					auto& eyeball = model.eyeballs.emplace_back();

					// note: eyeball names are not stored in compiled MDL files ?
					// sznameindex exists in the struct but is never populated by studiomdl
					stream.skip<int32_t>();  // sznameindex

					stream
						.read(eyeball.bone)
						.read(eyeball.org)
						.read(eyeball.zOffset)
						.read(eyeball.radius)
						.read(eyeball.up)
						.read(eyeball.forward)
						.read(eyeball.texture)
						.skip<int32_t>()  // unused1
						.read(eyeball.irisScale)
						.skip<int32_t>()  // unused2
						.read(eyeball.upperFlexDesc)
						.read(eyeball.lowerFlexDesc)
						.read(eyeball.upperTarget)
						.read(eyeball.lowerTarget)
						.read(eyeball.upperLidFlexDesc)
						.read(eyeball.lowerLidFlexDesc);

					stream.skip<int32_t>(4);  // unused[4]
					stream.read(eyeball.nonFACS);
					stream.skip<uint8_t>(3);  // unused3[3]
					stream.skip<int32_t>(7);  // unused4[7]
				}
			}

			if (modelAttachmentsOffset != 0) {
				for (int k = 0; k < modelAttachmentsCount; k++) {
					const auto attachmentPos = modelAttachmentsOffset + k * (sizeof(int32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(math::Mat3x4f) + sizeof(int32_t) * 8);
					if (!seekAndValidate(stream, bodyPartPos + modelPos + attachmentPos)) {
						return false;
					}

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

		// extract position from the last column of the matrix
		attachment.position[0] = attachment.localMatrix[0][3];
		attachment.position[1] = attachment.localMatrix[1][3];
		attachment.position[2] = attachment.localMatrix[2][3];

		stream.skip<int32_t>(8); // unused
	}

	if (localNodeCount > 0) {
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

	if (flexDescCount > 0) {
		stream.seek(flexDescIndex);
		for (int i = 0; i < flexDescCount; i++) {
			auto& flexDescName = this->flexDescs.emplace_back();
			parser::binary::readStringAtOffset(stream, flexDescName);
		}
	}

	for (int i = 0; i < flexControllerCount; i++) {
		const auto flexControllerPos = flexControllerIndex + i * (sizeof(int32_t) * 3 + sizeof(float) * 2);
		stream.seek_u(flexControllerPos);

		auto& flexController = this->flexControllers.emplace_back();

		// lambda to read at relative offset
		auto readStringAtRelativeOffset = [&](std::string& target) {
			if (const auto offset = stream.read<int32_t>(); offset != 0) {
				const auto savedPos = stream.tell();
				// skips if fail
				if (!seekAndValidate(stream, flexControllerPos + offset)) {
					return;
				}
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

	for (int i = 0; i < flexRulesCount; i++) {
		const auto flexRulePos = flexRulesIndex + i * (sizeof(int32_t) * 3);
		stream.seek_u(flexRulePos);

		auto& flexRule = this->flexRules.emplace_back();
		stream.read(flexRule.flex);

		const auto opCount = stream.read<int32_t>();
		const auto opIndex = stream.read<int32_t>();

		const auto opDataPos = flexRulePos + opIndex;
		stream.seek_u(opDataPos);

		for (int j = 0; j < opCount; j++) {
			auto& op = flexRule.ops.emplace_back();
			stream
				.read(op.op)
				.read(op.d.index);
		}
	}

	for (int i = 0; i < ikChainCount; i++) {
		const auto ikChainPos = ikChainIndex + i * (sizeof(int32_t) * 4);
		stream.seek_u(ikChainPos);

		auto& ikChain = this->ikChains.emplace_back();
		parser::binary::readStringAtOffset(stream, ikChain.name);
		stream.read(ikChain.linkType);

		const auto linkCount = stream.read<int32_t>();
		const auto linkIndex = stream.read<int32_t>();

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

	if (mouthsCount > 0) {
		stream.seek(mouthsIndex);
		for (int i = 0; i < mouthsCount; i++) {
			auto& mouth = this->mouths.emplace_back();
			stream
				.read(mouth.bone)
				.read(mouth.forward)
				.read(mouth.flexDescIndex);
		}
	}

	if (localPoseParamCount > 0) {
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

	if (surfacePropertyIndex != 0) {
		stream.seek(surfacePropertyIndex).read(this->surfaceProperty);
	}

	if (keyValueCount > 0) {
		stream.seek(keyValueIndex).read(this->keyValues, keyValueCount);
	}

	if (localIKAutoplayLockCount > 0) {
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

	for (int i = 0; i < includeModelCount; i++) {
		const auto includeModelPos = includeModelIndex + i * (sizeof(int32_t) * 2);
		stream.seek_u(includeModelPos);
		auto& includeModel = this->includeModels.emplace_back();

		// lambda to read at relative offset
		auto readStringAtRelativeOffset = [&](std::string& target) {
			if (const auto offset = stream.read<int32_t>(); offset != 0) {
				const auto savedPos = stream.tell();
				// skips if fail
				if (!seekAndValidate(stream, includeModelPos + offset)) {
					return;
				}
				stream.read(target);
				stream.seek_u(savedPos);
			}
		};

		readStringAtRelativeOffset(includeModel.label);
		readStringAtRelativeOffset(includeModel.name);
	}

	if (animationBlocksNameIndex != 0) {
		stream.seek(animationBlocksNameIndex).read(this->animationBlocksName);
	}

	if (animationBlocksCount > 0) {
		stream.seek(animationBlocksIndex);
		for (int i = 0; i < animationBlocksCount; i++) {
			auto& animBlock = this->animationBlocks.emplace_back();
			stream
				.read(animBlock.dataStart)
				.read(animBlock.dataEnd);
		}
	}

	if (boneTableNameIndex != 0) {
		stream.seek(boneTableNameIndex);
		this->boneTableByName.reserve(boneCount);
		for (int i = 0; i < boneCount; i++) {
			this->boneTableByName.push_back(stream.read<uint8_t>());
		}
	}

	for (int i = 0; i < flexControllerUICount; i++) {
		const auto flexControllerUIPos = flexControllerUIIndex + i * (sizeof(int32_t) * 4 + sizeof(uint8_t) * 4);
		stream.seek_u(flexControllerUIPos);

		auto& flexControllerUI = this->flexControllerUIs.emplace_back();
		parser::binary::readStringAtOffset(stream, flexControllerUI.name);

		const auto szIndex0 = stream.read<int32_t>();
		const auto szIndex1 = stream.read<int32_t>();
		const auto szIndex2 = stream.read<int32_t>();

		stream
			.read(flexControllerUI.remapType)
			.read(flexControllerUI.stereo);

		stream.skip<uint8_t>(2); // unused

		// lambda to resolve controller name from szIndex offset
		auto resolveControllerName = [&](int32_t szIndex, std::string& targetName) {
			if (szIndex != 0) {
				const auto savedPos = stream.tell();
				stream.seek_u(flexControllerUIPos + szIndex);
				stream.skip<int32_t>();
				if (const auto controllerNameOffset = stream.read<int32_t>(); controllerNameOffset != 0) {
					// skips if fail
					if (!seekAndValidate(stream, flexControllerUIPos + szIndex + controllerNameOffset)) {
						return;
					}
					stream.read(targetName);
				}
				stream.seek_u(savedPos);
			}
		};

		resolveControllerName(szIndex0, flexControllerUI.controllerName0);
		resolveControllerName(szIndex1, flexControllerUI.controllerName1);
		resolveControllerName(szIndex2, flexControllerUI.controllerName2);
	}

	return true;
}
