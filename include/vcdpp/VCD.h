// ReSharper disable CppUnusedIncludeDirective

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace vcdpp {

struct Event {
	enum class Type : uint8_t {
		UNSPECIFIED      = 0,
		SECTION          = 1,
		EXPRESSION       = 2,
		LOOK_AT          = 3,
		MOVE_TO          = 4,
		SPEAK            = 5,
		GESTURE          = 6,
		SEQUENCE         = 7,
		FACE             = 8,
		FIRE_TRIGGER     = 9,
		FLEX_ANIMATION   = 10,
		SUBSCENE         = 11,
		LOOP             = 12,
		INTERRUPT        = 13,
		STOP_POINT       = 14,
		PERMIT_RESPONSES = 15,
		GENERIC          = 16,
		CAMERA           = 17,
		SCRIPT           = 18,
	} type;
	std::string name;
	float startTime = 0.f;
	float endTime = 0.f;
	std::string param;
	std::string param2;
	std::string param3;

	float pitch = 0.f;
	float yaw = 0.f;

	enum class Interpolate : uint8_t {
		DEFAULT = 0,
		CATMULLROM_NORMALIZE_X,
		EASEIN,
		EASEOUT,
		EASEINOUT,
		BSPLINE,
		LINEAR_INTERP,
		KOCHANEK,
		KOCHANEK_EARLY,
		KOCHANEK_LATE,
		SIMPLE_CUBIC,
		CATMULLROM,
		CATMULLROM_NORMALIZE,
		CATMULLROM_TANGENT,
		EXPONENTIAL_DECAY,
		HOLD,
		COUNT,
	};

	struct Curve {
		Interpolate from = Interpolate::DEFAULT;
		Interpolate to = Interpolate::DEFAULT;
	};

	struct Edge {
		Curve curve;
		float time = 0.f;
	};

	struct Sample {
		float time = 0.f;
		float value = 0.f;
		std::optional<Curve> curve;
	};

	struct RampLegacy {
		float attack = 1.f;
		float sustain = 1.f;
		float decay = 1.f;
	};
	std::optional<RampLegacy> eventRampLegacy;

	struct Ramp {
		std::optional<Curve> curve;
		std::optional<Edge> leftEdge;
		std::optional<Edge> rightEdge;
		std::vector<Sample> samples;
	};
	std::optional<Ramp> eventRamp;

	bool resumeCondition = false;
	bool lockBodyFacing = false;
	float distanceToTarget = 0.f;
	bool fixedLength = false;
	bool forceShortMovement = false;
	bool syncToFollowingGesture = false;
	bool playOverScript = false;

	struct RelativeTagLegacy {
		std::string name;
		std::string filename;
	};

	std::optional<RelativeTagLegacy> relativeTagLegacy;

	struct Tag {
		std::string name;
		float value = 0.f;
	};

	std::vector<Tag> relativeTags;
	std::vector<Tag> absoluteTagsPlaybackTimes;
	std::vector<Tag> absoluteTagsShiftedTimes;

	struct FlexTimingTag {
		Tag tag;
		bool locked = false;
	};

	std::vector<FlexTimingTag> flexTimingTags;

	struct FlexAnimation {
		bool samplesUseTime = false;
		std::optional<Curve> curve;

		struct Track {
			std::string name;
			bool disabled = false;
			float minRange = 0.f;
			float maxRange = 1.f;
			std::optional<Edge> leftEdge;
			std::optional<Edge> rightEdge;
			std::vector<Sample> primary;
			std::optional<std::vector<Sample>> secondary;
		};

		std::vector<Track> tracks;
	};

	std::vector<FlexAnimation> flexAnimations;

	float sequenceDuration = 0.f;

	struct SpeechData {
		enum class CCType : uint8_t {
			CC_MASTER   = 0,
			CC_SLAVE    = 1,
			CC_DISABLED = 2, // unused
		} ccType = CCType::CC_MASTER;
		std::string ccToken;
		enum class CCFlags : uint8_t {
			NONE                        = 0,
			USING_COMBINED_FILE         = 1 << 0, // unused
			COMBINED_USING_GENDER_TOKEN = 1 << 1, // unused
			NO_ATTENUATE                = 1 << 2,
		} ccFlags = CCFlags::NONE;
	} speechData;

	int8_t loopCount = 0;

	bool active = true;
};
SOURCEPP_BITFLAGS_ENUM(Event::SpeechData::CCFlags);

struct Actor {
	struct Channel {
		std::string name;
		std::vector<Event> events;
		bool active = true;
	};

	std::string name;
	std::string faceposerModel;
	std::vector<Channel> channels;
	bool active = true;
};

struct FaceposerSettings {
	struct ScaleSettings {
		int choreoView     = 100;
		int expressionTool = 100;
		int gestureTool    = 100;
		int rampTool       = 100;
		int sceneRampTool  = 100;
	} scaleSettings;
	std::string mapName;
	uint8_t fps = 60;
	bool snap = false;
	std::optional<bool> ignorePhonemes;
};

class VCD {
public:
	enum class Platform {
		UNKNOWN = 0,
		VCD = 1,
		VCE = 2,
		BVCD = sourcepp::parser::binary::makeFourCC("bvcd"),
		XVCD = sourcepp::parser::binary::makeFourCC("xvcd"),
	};

	enum class ParseError {
		OK = 0,
		EXPECTED_OPENING_BRACE,
		UNKNOWN_EVENT_TYPE,
		UNKNOWN_ABSOLUTETAGS_TYPE,
		UNKNOWN_CC_TYPE,
		INVALID_CURVE_TOKEN,
		UNKNOWN_CURVE_TYPE,
		UNKNOWN_EVENT_KEY,
		UNKNOWN_CHANNEL_KEY,
		UNKNOWN_ACTOR_KEY,
		UNKNOWN_SCENE_SETTINGS_KEY,
		UNKNOWN_TOP_LEVEL_KEY,
	};

	[[nodiscard]] static std::string getMessageForParseErrorCode(ParseError error, std::string_view token);

	class SyntaxError : public std::runtime_error {
	public:
		explicit SyntaxError(ParseError error, std::string_view token) : std::runtime_error{getMessageForParseErrorCode(error, token)} {}
	};

	VCD() = default;

	explicit VCD(std::string_view vcdData);

	[[nodiscard]] Platform getPlatform() const;

	[[nodiscard]] uint8_t getVersion() const;

	[[nodiscard]] uint32_t getChecksum() const;

	[[nodiscard]] const std::vector<Event>& getGlobalEvents() const;

	[[nodiscard]] std::vector<Event>& getGlobalEvents();

	[[nodiscard]] const std::vector<Actor>& getActors() const;

	[[nodiscard]] std::vector<Actor>& getActors();

	[[nodiscard]] const std::optional<Event::Ramp>& getSceneRamp() const;

	[[nodiscard]] std::optional<Event::Ramp>& getSceneRamp();

	[[nodiscard]] const FaceposerSettings& getFaceposerSettings() const;

	[[nodiscard]] FaceposerSettings& getFaceposerSettings();

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::filesystem::path& vcdPath) const; // NOLINT(*-use-nodiscard)

protected:
	Platform platform = Platform::UNKNOWN;
	uint8_t version = 1;
	uint32_t checksum = 0;
	std::vector<Event> globalEvents;
	std::vector<Actor> actors;
	std::optional<Event::Ramp> sceneRamp;
	FaceposerSettings faceposerSettings;
};

namespace literals {

inline VCD operator""_vcd(const char* str, const std::size_t len) {
	return VCD{std::string_view{str, len}};
}

} // namespace literals

} // namespace vcdpp
