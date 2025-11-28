#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Generic.h"

namespace mdlpp::MDL {

// Header2 structs
struct SrcBoneTransform {
	//int32_t nameIndex;
	std::string name;

	sourcepp::math::Mat3x4f pretransform;
	sourcepp::math::Mat3x4f posttransform;
};

struct LinearBone {
	int32_t boneCount;

	std::vector<int32_t> flags;
	std::vector<int32_t> parent;
	std::vector<sourcepp::math::Vec3f> position;
	std::vector<sourcepp::math::Quat> quaternion;
	std::vector<sourcepp::math::Vec3f> rotation;
	std::vector<sourcepp::math::Mat3x4f> poseToBone;
	std::vector<sourcepp::math::Vec3f> positionScale;
	std::vector<sourcepp::math::Vec3f> rotationScale;
	std::vector<sourcepp::math::Quat> quaternionAlignment;
};

struct BoneFlexDriverControl {
	int32_t boneComponent;
	int32_t flexControllerIndex;
	float min;
	float max;
};

struct BoneFlexDriver {
	int32_t boneIndex;

	//int32_t controlCount;
	//int32_t controlIndex;
	std::vector<BoneFlexDriverControl> controls;

	//int32_t unused[3];
};

// Bone structs
struct Bone {
	enum Flags : int32_t {
		FLAG_NONE                      = 0,
		FLAG_PHYSICALLY_SIMULATED      = 1 << 0,
		FLAG_PHYSICS_PROCEDURAL        = 1 << 1,
		FLAG_ALWAYS_PROCEDURAL         = 1 << 2,
		FLAG_SCREEN_ALIGN_SPHERE       = 1 << 3,
		FLAG_SCREEN_ALIGN_CYLINDER     = 1 << 4,
		FLAG_CALCULATE_MASK            = FLAG_PHYSICALLY_SIMULATED | FLAG_PHYSICS_PROCEDURAL | FLAG_ALWAYS_PROCEDURAL | FLAG_SCREEN_ALIGN_SPHERE | FLAG_SCREEN_ALIGN_CYLINDER,
		FLAG_USED_BY_HITBOX            = 1 << 8,
		FLAG_USED_BY_ATTACHMENT        = 1 << 9,
		FLAG_USED_BY_VERTEX_LOD0       = 1 << 10,
		FLAG_USED_BY_VERTEX_LOD1       = 1 << 11,
		FLAG_USED_BY_VERTEX_LOD2       = 1 << 12,
		FLAG_USED_BY_VERTEX_LOD3       = 1 << 13,
		FLAG_USED_BY_VERTEX_LOD4       = 1 << 14,
		FLAG_USED_BY_VERTEX_LOD5       = 1 << 15,
		FLAG_USED_BY_VERTEX_LOD6       = 1 << 16,
		FLAG_USED_BY_VERTEX_LOD7       = 1 << 17,
		FLAG_USED_BY_BONE_MERGE        = 1 << 18,
		FLAG_USED_BY_VERTEX_MASK       = FLAG_USED_BY_VERTEX_LOD0 | FLAG_USED_BY_VERTEX_LOD1 | FLAG_USED_BY_VERTEX_LOD2 | FLAG_USED_BY_VERTEX_LOD3 | FLAG_USED_BY_VERTEX_LOD4 | FLAG_USED_BY_VERTEX_LOD5 | FLAG_USED_BY_VERTEX_LOD6 | FLAG_USED_BY_VERTEX_LOD7,
		FLAG_USED_MASK                 = FLAG_USED_BY_HITBOX | FLAG_USED_BY_ATTACHMENT | FLAG_USED_BY_VERTEX_MASK | FLAG_USED_BY_BONE_MERGE,
		FLAG_USED_BY_ANYTHING          = FLAG_USED_MASK,
		FLAG_FIXED_ALIGNMENT           = 1 << 20,
		FLAG_HAS_SAVEFRAME_POS         = 1 << 21,
		FLAG_HAS_SAVEFRAME_ROT         = 1 << 22,
		FLAG_EMPTY_SLOT                = 1 << 23,
		FLAG_TYPE_MASK                 = FLAG_FIXED_ALIGNMENT | FLAG_HAS_SAVEFRAME_POS | FLAG_HAS_SAVEFRAME_ROT | FLAG_EMPTY_SLOT,
	};

	//int32_t nameIndex;
	std::string name;

	int32_t parent;
	std::array<int32_t, 6> boneController;
	sourcepp::math::Vec3f position;
	sourcepp::math::Quat rotationQuat;
	sourcepp::math::Vec3f rotationEuler;
	sourcepp::math::Vec3f positionScale;
	sourcepp::math::Vec3f rotationScale;
	sourcepp::math::Mat3x4f poseToBose;
	sourcepp::math::Quat alignment;
	Flags flags;
	int32_t procType;
	int32_t procIndex;
	int32_t physicsBone;

	//int32_t surfacePropNameIndex;
	std::string surfacePropName;

	int32_t contents;

	//int32_t _unused0[8];
};
SOURCEPP_BITFLAGS_ENUM(Bone::Flags)

struct BoneController {
	int32_t bone;
	int32_t type;
	float start;
	float end;
	int32_t rest;
	int32_t inputField;

	//int32_t _unused0[8];
};

// Hitbox structs
struct HitboxSet {
	//int32_t nameIndex;
	std::string name;

	//int32_t hitboxCount;
	//int32_t hitboxIndex;
	std::vector<BBox> hitboxes;
};

// Animation structs
struct AnimBoneData {
	enum Flags : uint8_t {
		FLAG_NONE     = 0,
		FLAG_RAW_POS  = 1 << 0,
		FLAG_RAW_ROT  = 1 << 1,
		FLAG_ANIM_POS = 1 << 2,
		FLAG_ANIM_ROT = 1 << 3,
		FLAG_DELTA    = 1 << 4,
		FLAG_RAW_ROT2 = 1 << 5,
	};

	uint8_t bone;
	Flags flags;

	// static data when RAW flags are set
	std::variant<std::monostate, sourcepp::math::QuatCompressed48, sourcepp::math::QuatCompressed64> staticRotation;
	std::optional<sourcepp::math::Vec3Compressed48> staticPosition;

	// keyframe data when ANIM flags are set
	std::optional<AnimValuePtr> animRotationPtr;
	std::optional<AnimValuePtr> animPositionPtr;
	std::vector<AnimValue> animRotationData;  // Keyframe arrays
	std::vector<AnimValue> animPositionData;  // Keyframe arrays
};
SOURCEPP_BITFLAGS_ENUM(AnimBoneData::Flags)

struct IKError {
	sourcepp::math::Vec3f position;
	sourcepp::math::Quat rotation;
};

struct CompressedIKError {
	sourcepp::math::Vec<6, float> scale;
	sourcepp::math::Vec<6, int16_t> offset;
	std::vector<AnimValue> animValues;
};

struct IKRule {
	int32_t index;
	int32_t type;
	int32_t chain;
	int32_t bone;
	int32_t slot;
	float height;
	float radius;
	float floor;
	sourcepp::math::Vec3f pos;
	sourcepp::math::Quat q;

	//int32_t compressedIKErrorIndex;
	std::optional<CompressedIKError> compressedIKError;

	//int32_t unused2;

	int32_t iStart;
	//int32_t ikErrorIndex;
	std::vector<IKError> ikErrors;

	float start;
	float peak;
	float tail;
	float end;
	//float unused3;
	float contact;
	float drop;
	float top;

	//int32_t unused6;
	//int32_t unused7;
	//int32_t unused8;

	//int32_t attachmentIndex;
	std::string attachment;  // just the name

	//int32_t unused[7];
};

struct LocalHierarchy {
	int32_t bone{};
	int32_t newParent{};
	float start{};
	float peak{};
	float tail{};
	float end{};
	int32_t startFrame{};

	//int32_t localAnimIndex;
	std::optional<CompressedIKError> compressedIKError;

	//int32_t unused[4];
};

struct AnimSection {
	int32_t animBlock;
	int32_t animIndex;
};

struct AnimDesc {
	enum Flags : int32_t {
		FLAG_NONE     = 0,
		FLAG_RAW_POS  = 1 << 0,
		FLAG_RAW_ROT  = 1 << 1,
		FLAG_ANIM_POS = 1 << 2,
		FLAG_ANIM_ROT = 1 << 3,
		FLAG_DELTA    = 1 << 4,
		FLAG_RAW_ROT2 = 1 << 5,
	};

	//int32_t basePointer;

	//int32_t nameIndex;
	std::string name;
	float fps;

	Flags flags;

	int32_t frameCount;

	//int32_t movementCount;
	//int32_t movementIndex;
	std::vector<Movement> movements;

	//int32_t _unused0[6];

	int32_t animBlock;
	int32_t animIndex;

	//int32_t ikRuleCount;
	//int32_t ikRuleIndex;
	std::vector<IKRule> ikRules;

	int32_t animBlockIKRuleIndex;

	//int32_t localHierarchyCount;
	//int32_t localHierarchyIndex;
	std::vector<LocalHierarchy> localHierarchies;

	//int32_t sectionIndex;
	int32_t sectionFrames;
	std::vector<AnimSection> sections;

	int16_t zeroFrameSpan;
	int16_t zeroFrameCount;
	int32_t zeroFrameIndex;
	float zeroFrameStallTime;

	// animation data
	std::vector<AnimBoneData> boneAnimations;
};
SOURCEPP_BITFLAGS_ENUM(AnimDesc::Flags)

// Sequence structs
struct Event {
	float cycle;
	int32_t event;
	int32_t type;

	//char options[64];
	std::string options;

	//int32_t eventNameIndex;
	std::string eventName;
};

struct AutoLayer {
	int16_t sequence;
	int16_t pose;
	int32_t flags;
	float start;
	float peak;
	float tail;
	float end;
};

struct SequenceDesc {
	enum Flags : int32_t {
		FLAG_NONE = 0,
		FLAG_LOOPING = 1 << 0,
	};

	//int32_t basePointer;

	//int32_t labelIndex;
	std::string label;

	//int32_t activityLabelIndex;
	std::string activityName;

	Flags flags;

	int32_t activity;
	int32_t activityWeight;

	//int32_t eventCount;
	//int32_t eventIndex;
	std::vector<Event> events;

	sourcepp::math::Vec3f boundingBoxMin;
	sourcepp::math::Vec3f boundingBoxMax;

	int32_t blendCount;

	//int32_t animIndexIndex;
	std::vector<int16_t> animIndices; // groupSize[0] * groupSize[1] (for animation blending)

	int32_t movementIndex;

	int32_t groupSize[2];
	int32_t paramIndex[2];
	float paramStart[2];
	float paramEnd[2];
	int32_t paramParent;

	float fadeInTime;
	float fadeOutTime;

	int32_t localEntryNode;
	int32_t localExitNode;

	int32_t nodeFlags;

	float entryPhase;
	float exitPhase;

	float lastFrame;

	int32_t nextSequence;
	int32_t pose;

	int32_t ikRuleCount;

	//int32_t autoLayerCount;
	//int32_t autoLayerIndex;
	std::vector<AutoLayer> autoLayers;

	//int32_t weightListIndex;
	std::vector<float> boneWeights;

	//int32_t poseKeyIndex;
	std::vector<float> poseKeys;

	//int32_t ikLockCount;
	//int32_t ikLockIndex;
	std::vector<IKLock> ikLocks;

	//int32_t keyValueIndex;
	//int32_t keyValueSize;
	std::string keyValues;

	int32_t cyclePoseIndex; // index into poseParameters

	//int32_t activityModifierCount;
	//int32_t activityModifierIndex;
	std::vector<std::string> activityModifiers;

	//int32_t _unused0[5];
};
SOURCEPP_BITFLAGS_ENUM(SequenceDesc::Flags)

// Material structs
struct Material {
	enum Flags : int32_t {
		FLAG_NONE                              = 0,
		FLAG_RELATIVE_TEXTURE_PATH_SPECIFIED   = 1 << 0,
		// Note: mstudiotexture_t.flags field exists in the format but is never set by the studiomdl compiler.
		// The engine might still check this flag, so it is exposed here anyway.
	};

	//int32_t nameIndex;
	std::string name;

	Flags flags;

	//int32_t used; // No idea what this is
	//int32_t _unused0[13];
};
SOURCEPP_BITFLAGS_ENUM(Material::Flags)

// BodyPart structs (Model, Mesh, Flex, Eyeball, Attachment)
struct Eyeball {
	//int32_t sznameindex;
	std::string name;

	int32_t bone;
	sourcepp::math::Vec3f org;
	float zOffset;
	float radius;
	sourcepp::math::Vec3f up;
	sourcepp::math::Vec3f forward;
	int32_t texture;

	//int32_t unused1;
	float irisScale;
	//int32_t unused2;

	std::array<int32_t, 3> upperFlexDesc;
	std::array<int32_t, 3> lowerFlexDesc;
	std::array<float, 3> upperTarget;
	std::array<float, 3> lowerTarget;

	int32_t upperLidFlexDesc;
	int32_t lowerLidFlexDesc;
	//int32_t unused[4];
	bool nonFACS;
	//char unused3[3];
	//int32_t unused4[7];
};

struct VertexAnim {
	uint16_t index;
	uint8_t speed;	// 255/max_length_in_flex
	uint8_t side;	// 255/left_right

	// pos delta
	std::array<int16_t, 3> delta;

	// normal delta
	std::array<int16_t, 3> ndelta;
};

struct VertexAnimWrinkle {
	uint16_t index;
	uint8_t speed;
	uint8_t side;
	std::array<int16_t, 3> delta;
	std::array<int16_t, 3> ndelta;
	int16_t wrinkleDelta;
};

struct MeshFlex {
	int32_t flexDescIndex;

	// control curve
	float target0;
	float target1;
	float target2;
	float target3;

	//int32_t numverts;
	//int32_t vertindex;
	std::vector<VertexAnim> vertAnims;
	std::vector<VertexAnimWrinkle> vertAnimsWrinkle;

	int32_t flexPair;
	uint8_t vertAnimType;  // 0=normal, 1=wrinkle
	//uint8_t unusedchar[3];
	//int32_t unused[6];
};

struct Mesh {
	int32_t material;

	//int32_t modelOffset;

	// These do not map to raw memory
	int32_t verticesCount;
	int32_t verticesOffset;

	//int32_t flexesCount;
	//int32_t flexesOffset;
	std::vector<MeshFlex> flexes;

	int32_t materialType;
	int32_t materialParam;

	int32_t meshID;

	sourcepp::math::Vec3f center;

	int32_t modelVertexData;
	std::array<int32_t, MAX_LOD_COUNT> numLODVertexes;
	//int32_t _unused[8];
};

struct Attachment {
	//int32_t nameIndex;
	std::string name;

	int32_t bone;
	sourcepp::math::Vec3f position;

	sourcepp::math::Mat3x4f localMatrix;
	//int32_t _unused[8];
};

struct Model {
	//char name[64];
	std::string name;

	int32_t type;

	float boundingRadius;

	//int32_t meshesCount;
	//int32_t meshesOffset;
	std::vector<Mesh> meshes;

	// These do not map to raw memory
	int32_t verticesCount;
	int32_t verticesOffset;
	int32_t tangentsOffset;

	//int32_t attachmentsCount;
	//int32_t attachmentsOffset;
	std::vector<Attachment> attachments;

	//int32_t eyeballsCount;
	//int32_t eyeballsOffset;
	std::vector<Eyeball> eyeballs;

	//int32_t _unused0[10];
};

struct BodyPart {
	//int32_t nameOffset;
	std::string name;

	//int32_t modelsCount;
	int32_t base; // multiplier for bodygroup skin index calculation?
	//int32_t modelsOffset;
	std::vector<Model> models;
};

// Flex structs
struct FlexController {
	//int32_t typeIndex;
	std::string type;

	//int32_t nameIndex;
	std::string name;

	int32_t localToGlobal;
	float min;
	float max;
};

enum FlexOpType : int32_t {
	FLEX_OP_CONST = 1,
	FLEX_OP_FETCH1 = 2,
	FLEX_OP_FETCH2 = 3,
	FLEX_OP_ADD = 4,
	FLEX_OP_SUB = 5,
	FLEX_OP_MUL = 6,
	FLEX_OP_DIV = 7,
	FLEX_OP_NEG = 8,
	FLEX_OP_EXP = 9,
	FLEX_OP_OPEN = 10,
	FLEX_OP_CLOSE = 11,
	FLEX_OP_COMMA = 12,
	FLEX_OP_MAX = 13,
	FLEX_OP_MIN = 14,
	FLEX_OP_2WAY_0 = 15,
	FLEX_OP_2WAY_1 = 16,
	FLEX_OP_NWAY = 17,
	FLEX_OP_COMBO = 18,
	FLEX_OP_DOMINATE = 19,
	FLEX_OP_DME_LOWER_EYELID = 20,
	FLEX_OP_DME_UPPER_EYELID = 21,
};

struct FlexOp {
	int32_t op;
	union {
		int32_t index;
		float value;
	} d;
};

struct FlexRule {
	int32_t flex;

	//int32_t opCount;
	//int32_t opIndex;
	std::vector<FlexOp> ops;
};

// IK structs
struct IKLink {
	int32_t bone;
	sourcepp::math::Vec3f kneeDir;
	//sourcepp::math::Vec3f unused;
};

struct IKChain {
	//int32_t nameIndex;
	std::string name;

	int32_t linkType;

	//int32_t linkCount;
	//int32_t linkIndex;
	std::vector<IKLink> links;
};

// Mouth struct
struct Mouth {
	int32_t bone;
	sourcepp::math::Vec3f forward;
	int32_t flexDescIndex;
};

// Pose parameter struct
struct PoseParameter {
	//int32_t nameIndex;
	std::string name;

	int32_t flags;
	float start;
	float end;
	float loop;
};

// Include model struct
struct IncludeModel {
	//int32_t labelIndex;
	std::string label;

	//int32_t nameIndex;
	std::string name;
};

// Animation block struct
struct AnimBlock {
	// external offsets
	int32_t dataStart;
	int32_t dataEnd;
};

// Flex controller UI struct
struct FlexControllerUI {
	//int32_t nameIndex;
	std::string name;

	// SIMPLE/STEREO/NWAY
	//int32_t szindex0;
	//int32_t szindex1;
	//int32_t szindex2;
	std::string controllerName0;  // non-stereo || left controller (stereo)
	std::string controllerName1;  // right controller (stereo)
	std::string controllerName2;  // value controller (NWAY only)

	uint8_t remapType;
	bool stereo;
	//uint8_t unused[2];
};

// Main struct
struct MDL {
	[[nodiscard]] bool open(const std::byte* data, std::size_t size);

	enum Flags : int32_t {
		FLAG_NONE                           = 0,
		FLAG_AUTOGENERATED_HITBOX           = 1 <<  0,
		FLAG_FORCE_OPAQUE                   = 1 <<  2,
		FLAG_TRANSLUCENT_TWO_PASS           = 1 <<  3,
		FLAG_STATIC_PROP                    = 1 <<  4,
		FLAG_HAS_SHADOW_LOD                 = 1 <<  6,
		FLAG_USE_SHADOW_LOD_MATERIALS       = 1 <<  8,
		FLAG_OBSOLETE                       = 1 <<  9,
		FLAG_NO_FORCED_FADE                 = 1 << 11,
		FLAG_FORCE_PHONEME_CROSSFADE        = 1 << 12,
		FLAG_CONSTANT_DIRECTIONAL_LIGHT_DOT = 1 << 13,
		FLAG_FLEXES_CONVERTED               = 1 << 14,
		FLAG_BUILT_IN_PREVIEW_MODE          = 1 << 15,
		FLAG_DO_NOT_CAST_SHADOWS            = 1 << 17,
		FLAG_CAST_TEXTURE_SHADOWS           = 1 << 18,
		FLAG_SUBDIVISION_SURFACE            = 1 << 19,
		FLAG_VERT_ANIM_FIXED_POINT_SCALE    = 1 << 21,
		FLAG_EXTRA_VERTEX_DATA              = 1 << 26,
	};

	//int32_t id;
	int32_t version;
	int32_t checksum;

	//char name[64];
	std::string name;
	//int32_t dataLength;

	sourcepp::math::Vec3f eyePosition;
	sourcepp::math::Vec3f illuminationPosition;
	sourcepp::math::Vec3f hullMin;
	sourcepp::math::Vec3f hullMax;
	sourcepp::math::Vec3f viewBBoxMin;
	sourcepp::math::Vec3f viewBBoxMax;

	Flags flags;

	//int32_t boneCount;
	//int32_t boneOffset;
	std::vector<Bone> bones;

	//int32_t boneControllerCount;
	//int32_t boneControllerOffset;
	std::vector<BoneController> boneControllers;

	//int32_t hitboxCount;
	//int32_t hitboxOffset;
	std::vector<HitboxSet> hitboxSets;

	//int32_t localAnimationCount;
	//int32_t localAnimationOffset;
	std::vector<AnimDesc> animations;

	//int32_t localSequenceCount;
	//int32_t localSequenceOffset;
	std::vector<SequenceDesc> sequences;

	int32_t activityListVersion;
	int32_t eventsIndexed;

	//int32_t materialCount;
	//int32_t materialOffset;
	std::vector<Material> materials;

	//int32_t materialDirCount;
	//int32_t materialDirOffset;
	std::vector<std::string> materialDirectories;

	//int32_t skinReferenceCount;
	//int32_t skinReferenceFamilyCount;
	//int32_t skinReferenceIndex;
	std::vector<std::vector<int16_t>> skins;

	//int32_t bodyPartCount;
	//int32_t bodyPartOffset;
	std::vector<BodyPart> bodyParts;

	//int32_t attachmentCount;
	//int32_t attachmentOffset;
	std::vector<Attachment> attachments;

	//int32_t localNodeCount;
	//int32_t localNodeIndex;
	//int32_t localNodeNameIndex;
	std::vector<std::string> localNodeNames;
	std::vector<uint8_t> localNodeTransitions;

	//int32_t flexDescCount;
	//int32_t flexDescIndex;
	std::vector<std::string> flexDescs;

	//int32_t flexControllerCount;
	//int32_t flexControllerIndex;
	std::vector<FlexController> flexControllers;

	//int32_t flexRulesCount;
	//int32_t flexRulesIndex;
	std::vector<FlexRule> flexRules;

	//int32_t ikChainCount;
	//int32_t ikChainIndex;
	std::vector<IKChain> ikChains;

	//int32_t mouthsCount;
	//int32_t mouthsIndex;
	std::vector<Mouth> mouths;

	//int32_t localPoseParamCount;
	//int32_t localPoseParamIndex;
	std::vector<PoseParameter> poseParameters;

	//int32_t surfacePropertyIndex;
	std::string surfaceProperty;

	//int32_t keyValueIndex;
	//int32_t keyValueCount;
	std::string keyValues;

	//int32_t localIKAutoplayLockCount;
	//int32_t localIKAutoplayLockIndex;
	std::vector<IKLock> ikAutoplayLocks;

	float mass;
	int32_t contentsFlags;

	//int32_t includeModelCount;
	//int32_t includeModelIndex;
	std::vector<IncludeModel> includeModels;

	// int32_t virtualModel;

	//int32_t animationBlocksNameIndex;
	//int32_t animationBlocksCount;
	//int32_t animationBlocksIndex;
	std::string animationBlocksName;
	std::vector<AnimBlock> animationBlocks;

	// int32_t animationBlockModel;

	//int32_t boneTableByNameIndex;
	std::vector<uint8_t> boneTableByName;

	// int32_t vertexBase;
	// int32_t offsetBase;

	uint8_t directionalDotProduct;
	uint8_t rootLOD;
	uint8_t numAllowedRootLODs;
	//uint8_t _unused0;

	//int32_t _unused1;

	//int32_t flexControllerUICount;
	//int32_t flexControllerUIIndex;
	std::vector<FlexControllerUI> flexControllerUIs;

	float vertAnimFixedPointScale;
	//int32_t _unused2;

	int32_t studioHdr2Index;

	//int32_t _unused3;

	struct Header2 {
		int32_t srcBoneTransformCount;
		int32_t srcBoneTransformIndex;
		int32_t illumPositionAttachmentIndex;
		float maxEyeDeflection;
		int32_t linearBoneIndex;
		int32_t nameIndex;
		int32_t boneFlexDriverCount;
		int32_t boneFlexDriverIndex;
		//int32_t reserved[56];
	};
	Header2 header2{};
	bool hasHeader2 = false;

	std::vector<SrcBoneTransform> srcBoneTransforms;
	std::optional<LinearBone> linearBone;
	std::vector<BoneFlexDriver> boneFlexDrivers;
};
SOURCEPP_BITFLAGS_ENUM(MDL::Flags)

} // namespace mdlpp::MDL
