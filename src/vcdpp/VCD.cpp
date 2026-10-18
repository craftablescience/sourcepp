// ReSharper disable CppUseStructuredBinding

#include <vcdpp/VCD.h>

#include <format>
#include <stdexcept>

#include <BufferStream.h>

#include <sourcepp/parser/Text.h>

using namespace sourcepp;
using namespace vcdpp;

namespace {

template<typename V>
concept ValueType = std::same_as<V, bool> || std::integral<V> || std::floating_point<V> || std::same_as<V, std::string> || std::same_as<V, std::string_view>;

} // namespace

std::string VCD::getMessageForParseErrorCode(ParseError error, std::string_view token) {
	switch (error) {
		case ParseError::OK:
			break;
		case ParseError::EXPECTED_OPENING_BRACE:
			return std::format("Expected opening brace '{{', got \"{}\"", token);
		case ParseError::UNKNOWN_EVENT_TYPE:
			return std::format("Unknown event type \"{}\"", token);
		case ParseError::UNKNOWN_ABSOLUTETAGS_TYPE:
			return std::format("Unknown absolutetags type \"{}\"", token);
		case ParseError::UNKNOWN_CC_TYPE:
			return std::format("Unknown cc type \"{}\"", token);
		case ParseError::INVALID_CURVE_TOKEN:
			return std::format("Invalid curve token \"{}\"", token);
		case ParseError::UNKNOWN_CURVE_TYPE:
			return std::format("Unknown curve type starting at \"{}\"", token);
		case ParseError::UNKNOWN_EVENT_KEY:
			return std::format("Unknown event key \"{}\"", token);
		case ParseError::UNKNOWN_CHANNEL_KEY:
			return std::format("Unknown channel key \"{}\"", token);
		case ParseError::UNKNOWN_ACTOR_KEY:
			return std::format("Unknown actor key \"{}\"", token);
		case ParseError::UNKNOWN_SCENE_SETTINGS_KEY:
			return std::format("Unknown scene settings key \"{}\"", token);
		case ParseError::UNKNOWN_TOP_LEVEL_KEY:
			return std::format("Unknown top level key \"{}\"", token);
	}
	return "";
}

VCD::VCD(std::string_view vcdData) {
	this->platform = Platform::VCD;

	// Version
	{
		BufferStreamReadOnly stream{vcdData};
		parser::text::eatWhitespace(stream);
		if (parser::text::readUnquotedString(stream) == "//") {
			parser::text::eatWhitespace(stream);
			if (string::iequals(parser::text::readUnquotedString(stream), "Choreo")) {
				parser::text::eatWhitespace(stream);
				if (string::iequals(parser::text::readUnquotedString(stream), "version")) {
					parser::text::eatWhitespace(stream);
					auto versionStr = parser::text::readUnquotedString(stream);

					// Handle the VCE comment header
					if (versionStr.ends_with(':')) {
						versionStr.pop_back();
						parser::text::eatWhitespace(stream);
						if (
							const auto eventCount = parser::text::readUnquotedString(stream);
							eventCount.size() >= 3 && eventCount.starts_with('<') && eventCount.ends_with('>') && string::isInt<int>(eventCount.substr(1, eventCount.size() - 2))
						) {
							this->platform = Platform::VCE;
						}
					}

					// Check/set version
					if (!string::toInt(versionStr, this->version)) {
						this->platform = Platform::UNKNOWN;
						return;
					}
				}
			}
		}
	}

	// Checksum doesn't exist in text format
	this->checksum = 0;

	// Ok now start parsing tokens
	std::string backingString;
	auto tokens = parser::text::Tokenizer{}
		.setMultiLineCommentStart("")
		.setMultiLineCommentEnd("")
		.parse(vcdData, backingString);

	const auto parseValue = [&tokens]<typename V>(V& value) requires ValueType<V> || (requires { typename V::value_type; } && std::same_as<V, std::optional<typename V::value_type>> && ValueType<typename V::value_type>) {
		const auto token = tokens.next();
		if constexpr (std::same_as<V, bool>) { // NOLINT(*-branch-clone)
			value = string::iequals(token, "on") || string::iequals(token, "1");
		} else if constexpr (std::integral<V>) {
			string::toInt(token, value);
		} else if constexpr (std::floating_point<V>) {
			string::toFloat(token, value);
		} else if constexpr (std::same_as<V, std::string> || std::same_as<V, std::string_view>) { // NOLINT(*-branch-clone)
			value = token;
		} else if constexpr (std::same_as<typename V::value_type, bool>) {
			value = string::iequals(token, "on") || string::iequals(token, "1");
		} else if constexpr (std::integral<typename V::value_type>) {
			typename V::value_type v;
			string::toInt(token, v);
			value = v;
		} else if constexpr (std::floating_point<typename V::value_type>) {
			typename V::value_type v;
			string::toFloat(token, v);
			value = v;
		} else if constexpr (std::same_as<typename V::value_type, std::string> || std::same_as<typename V::value_type, std::string_view>) {
			value = token;
		} else {
			// you fucked up!
			value = token;
		}
	};

	static constexpr auto parseCurve = [](Event::Curve& curve, std::string_view token) {
		auto curveToken = token;

		const auto parseInterpolateName = [token, &curveToken](Event::Interpolate& interpolate) {
			// Ordered specifically to check for substrings last!
			static constexpr std::array<std::pair<Event::Interpolate, std::string_view>, 16> INTERPOLATE{{
				{Event::Interpolate::DEFAULT, "default"},
				{Event::Interpolate::CATMULLROM_NORMALIZE_X, "catmullrom_normalize_x"},
				{Event::Interpolate::CATMULLROM_NORMALIZE, "catmullrom_normalize"},
				{Event::Interpolate::CATMULLROM_TANGENT, "catmullrom_tangent"},
				{Event::Interpolate::CATMULLROM, "catmullrom"},
				{Event::Interpolate::EASEINOUT, "easeinout"},
				{Event::Interpolate::EASEIN, "easein"},
				{Event::Interpolate::EASEOUT, "easeout"},
				{Event::Interpolate::BSPLINE, "bspline"},
				{Event::Interpolate::LINEAR_INTERP, "linear_interp"},
				{Event::Interpolate::KOCHANEK_EARLY, "kochanek_early"},
				{Event::Interpolate::KOCHANEK_LATE, "kochanek_late"},
				{Event::Interpolate::KOCHANEK, "kochanek"},
				{Event::Interpolate::SIMPLE_CUBIC, "simple_cubic"},
				{Event::Interpolate::EXPONENTIAL_DECAY, "exponential_decay"},
				{Event::Interpolate::HOLD, "hold"},
			}};
			static_assert(static_cast<uint8_t>(Event::Interpolate::COUNT) == INTERPOLATE.size());

			if (!curveToken.starts_with("curve_")) {
				throw SyntaxError{ParseError::INVALID_CURVE_TOKEN, token};
			}
			curveToken = curveToken.substr(6);

			for (auto [currentInterpolate, currentName] : INTERPOLATE) {
				if (curveToken.starts_with(currentName)) {
					interpolate = currentInterpolate;
					curveToken = curveToken.substr(currentName.size());
					return;
				}
			}
			throw SyntaxError{ParseError::UNKNOWN_CURVE_TYPE, curveToken};
		};

		parseInterpolateName(curve.from);

		if (!curveToken.starts_with("_to_")) {
			throw SyntaxError{ParseError::INVALID_CURVE_TOKEN, token};
		}
		curveToken = curveToken.substr(4);

		parseInterpolateName(curve.to);
	};

	const auto parseEdge = [&tokens, &parseValue](Event::Edge& edge) {
		parseCurve(edge.curve, tokens.next());
		parseValue(edge.time);
	};

	const auto parseSamples = [&tokens, &parseValue](std::vector<Event::Sample>& samples) {
		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.peek()) != "}") {
			auto& sample = samples.emplace_back();
			parseValue(sample.time);
			parseValue(sample.value);
			if (tokens.peek() != "}" && !string::isFloat<float>(tokens.peek())) {
				Event::Curve curve;
				parseCurve(curve, tokens.next());
				sample.curve = curve;
			}
		}
		tokens.skip();
	};

	const auto parseTags = [&tokens, &parseValue](std::vector<Event::Tag>& tags) {
		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			auto& tag = tags.emplace_back();
			tag.name = token;
			parseValue(tag.value);
		}
	};

	const auto parseFlexTimingTags = [&tokens, &parseValue](std::vector<Event::FlexTimingTag>& tags) {
		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			auto& tag = tags.emplace_back();
			tag.tag.name = token;
			parseValue(tag.tag.value);
			parseValue(tag.locked);
		}
	};

	const auto parseRamp = [&tokens, &parseEdge, &parseSamples](Event::Ramp& ramp) {
		if (tokens.peek() == "leftedge") {
			tokens.skip();
			Event::Edge edge;
			parseEdge(edge);
			ramp.leftEdge = edge;
		}

		if (tokens.peek() == "rightedge") {
			tokens.skip();
			Event::Edge edge;
			parseEdge(edge);
			ramp.rightEdge = edge;
		}

		parseSamples(ramp.samples);
	};

	const auto parseFlexAnimation = [&tokens, &parseValue, &parseEdge, &parseSamples](Event::FlexAnimation& flexAnimation) {
		if (tokens.peek() == "samples_use_time") {
			flexAnimation.samplesUseTime = true;
			tokens.skip();
		}

		static constexpr std::string_view DEFAULT_CURVE_TYPE_PREFIX = "defaultcurvetype=";
		if (tokens.peek().starts_with(DEFAULT_CURVE_TYPE_PREFIX)) {
			Event::Curve curve;
			parseCurve(curve, tokens.next().substr(DEFAULT_CURVE_TYPE_PREFIX.size()));
			flexAnimation.curve = curve;
		}

		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			auto& track = flexAnimation.tracks.emplace_back();
			track.name = token;

			if (tokens.peek() == "disabled") {
				track.disabled = true;
				tokens.skip();
			}

			bool hasSecondary = false;
			if (tokens.peek() == "combo") {
				hasSecondary = true;
				tokens.skip();
			}

			if (tokens.peek() == "range") {
				tokens.skip();
				parseValue(track.minRange);
				parseValue(track.maxRange);
			}

			if (tokens.peek() == "leftedge") {
				tokens.skip();
				Event::Edge edge;
				parseEdge(edge);
				track.leftEdge = edge;
			}

			if (tokens.peek() == "rightedge") {
				tokens.skip();
				Event::Edge edge;
				parseEdge(edge);
				track.rightEdge = edge;
			}

			if (tokens.peek() != "{") {
				throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, tokens.peek()};
			}
			parseSamples(track.primary);
			if (hasSecondary) {
				if (tokens.peek() != "{") {
					throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, tokens.peek()};
				}
				std::vector<Event::Sample> secondary;
				parseSamples(secondary);
				track.secondary = std::move(secondary);
			}
		}
	};

	const auto parseEvent = [&tokens, &parseValue, &parseTags, &parseFlexTimingTags, &parseRamp, &parseFlexAnimation](Event& event) {
		// Type
		{
			if (
				const auto token = tokens.next();
				string::iequals(token, "unspecified")
			) {
				event.type = Event::Type::UNSPECIFIED;
			} else if (string::iequals(token, "section")) {
				event.type = Event::Type::SECTION;
			} else if (string::iequals(token, "expression")) {
				event.type = Event::Type::EXPRESSION;
			} else if (string::iequals(token, "lookat")) {
				event.type = Event::Type::LOOK_AT;
			} else if (string::iequals(token, "moveto")) {
				event.type = Event::Type::MOVE_TO;
			} else if (string::iequals(token, "speak")) {
				event.type = Event::Type::SPEAK;
			} else if (string::iequals(token, "gesture")) {
				event.type = Event::Type::GESTURE;
			} else if (string::iequals(token, "sequence")) {
				event.type = Event::Type::SEQUENCE;
			} else if (string::iequals(token, "face")) {
				event.type = Event::Type::FACE;
			} else if (string::iequals(token, "firetrigger")) {
				event.type = Event::Type::FIRE_TRIGGER;
			} else if (string::iequals(token, "flexanimation")) {
				event.type = Event::Type::FLEX_ANIMATION;
			} else if (string::iequals(token, "subscene")) {
				event.type = Event::Type::SUBSCENE;
			} else if (string::iequals(token, "loop")) {
				event.type = Event::Type::LOOP;
			} else if (string::iequals(token, "interrupt")) {
				event.type = Event::Type::INTERRUPT;
			} else if (string::iequals(token, "stoppoint")) {
				event.type = Event::Type::STOP_POINT;
			} else if (string::iequals(token, "permitresponses")) {
				event.type = Event::Type::PERMIT_RESPONSES;
			} else if (string::iequals(token, "generic")) {
				event.type = Event::Type::GENERIC;
			} else if (string::iequals(token, "camera")) {
				event.type = Event::Type::CAMERA;
			} else if (string::iequals(token, "script")) {
				event.type = Event::Type::SCRIPT;
			} else {
				event.type = Event::Type::UNSPECIFIED;
				throw SyntaxError{ParseError::UNKNOWN_EVENT_TYPE, token};
			}
			parseValue(event.name);
		}

		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			if (string::iequals(token, "time")) {
				parseValue(event.startTime);
				if (string::isFloat<float>(tokens.peek())) {
					parseValue(event.endTime);
				}
			} else if (string::iequals(token, "param")) {
				parseValue(event.param);
			} else if (string::iequals(token, "param2")) {
				parseValue(event.param2);
			} else if (string::iequals(token, "param3")) {
				parseValue(event.param3);
			} else if (string::iequals(token, "ramp")) {
				Event::RampLegacy ramp;
				parseValue(ramp.attack);
				if (string::isFloat<float>(tokens.peek())) {
					parseValue(ramp.sustain);
					if (string::isFloat<float>(tokens.peek())) {
						parseValue(ramp.decay);
					}
				}
				event.eventRampLegacy = ramp;
			} else if (string::iequals(token, "event_ramp")) {
				Event::Ramp ramp;
				parseRamp(ramp);
				event.eventRamp = ramp;
			} else if (string::iequals(token, "pitch")) {
				parseValue(event.pitch);
			} else if (string::iequals(token, "yaw")) {
				parseValue(event.yaw);
			} else if (string::iequals(token, "resumecondition")) {
				event.resumeCondition = true;
			} else if (string::iequals(token, "lockbodyfacing")) {
				event.lockBodyFacing = true;
			} else if (string::iequals(token, "distancetotarget")) {
				parseValue(event.distanceToTarget);
			} else if (string::iequals(token, "fixedlength")) {
				event.fixedLength = true;
			} else if (string::iequals(token, "forceshortmovement")) {
				event.forceShortMovement = true;
			} else if (string::iequals(token, "synctofollowinggesture")) {
				event.syncToFollowingGesture = true;
			} else if (string::iequals(token, "playoverscript")) {
				event.playOverScript = true;
			} else if (string::iequals(token, "relativetag")) {
				Event::RelativeTagLegacy relativeTag;
				parseValue(relativeTag.name);
				parseValue(relativeTag.filename);
				event.relativeTagLegacy = relativeTag;
			} else if (string::iequals(token, "tags")) {
				parseTags(event.relativeTags);
			} else if (string::iequals(token, "absolutetags")) {
				token = tokens.next();
				if (string::iequals(token, "playback_time")) {
					parseTags(event.absoluteTagsPlaybackTimes);
				} else if (string::iequals(token, "shifted_time")) {
					parseTags(event.absoluteTagsShiftedTimes);
				} else {
					throw SyntaxError{ParseError::UNKNOWN_ABSOLUTETAGS_TYPE, token};
				}
			} else if (string::iequals(token, "flextimingtags")) {
				parseFlexTimingTags(event.flexTimingTags);
			} else if (string::iequals(token, "flexanimations")) {
				parseFlexAnimation(event.flexAnimations.emplace_back());
			} else if (string::iequals(token, "sequenceduration")) {
				parseValue(event.sequenceDuration);
			} else if (string::iequals(token, "cctype")) {
				if (
					token = tokens.next();
					string::iequals(token, "cc_master")
				) {
					event.speechData.ccType = Event::SpeechData::CCType::CC_MASTER;
				} else if (string::iequals(token, "cc_slave")) {
					event.speechData.ccType = Event::SpeechData::CCType::CC_SLAVE;
				} else if (string::iequals(token, "cc_disabled")) {
					event.speechData.ccType = Event::SpeechData::CCType::CC_DISABLED;
				} else {
					throw SyntaxError{ParseError::UNKNOWN_CC_TYPE, token};
				}
			} else if (string::iequals(token, "cctoken")) {
				parseValue(event.speechData.ccToken);
			} else if (string::iequals(token, "cc_usingcombinedfile")) {
				event.speechData.ccFlags |= Event::SpeechData::CCFlags::USING_COMBINED_FILE;
			} else if (string::iequals(token, "cc_combinedusesgender")) {
				event.speechData.ccFlags |= Event::SpeechData::CCFlags::COMBINED_USING_GENDER_TOKEN;
			} else if (string::iequals(token, "cc_noattenuate") || string::iequals(token, "cc_noattentuate")) {
				// cc_noattentuate??? lol??? it shows up in HL2 and P1 but this string doesn't exist in newer games...
				event.speechData.ccFlags |= Event::SpeechData::CCFlags::NO_ATTENUATE;
			} else if (string::iequals(token, "loopcount")) {
				parseValue(event.loopCount);
			} else if (string::iequals(token, "active")) {
				parseValue(event.active);
			} else {
				throw SyntaxError{ParseError::UNKNOWN_EVENT_KEY, token};
			}
		}
	};

	const auto parseActor = [&tokens, &parseValue, &parseEvent](Actor& actor) {
		const auto parseChannel = [&tokens, &parseValue, &parseEvent](Actor::Channel& channel) {
			parseValue(channel.name);

			auto token = tokens.next();
			if (token != "{") {
				throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
			}
			while ((token = tokens.next()) != "}") {
				if (string::iequals(token, "event")) {
					parseEvent(channel.events.emplace_back());
				} else if (string::iequals(token, "active")) {
					parseValue(channel.active);
				} else {
					throw SyntaxError{ParseError::UNKNOWN_CHANNEL_KEY, token};
				}
			}
		};

		parseValue(actor.name);

		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			if (string::iequals(token, "faceposermodel")) {
				parseValue(actor.faceposerModel);
			} else if (string::iequals(token, "channel")) {
				parseChannel(actor.channels.emplace_back());
			} else if (string::iequals(token, "active")) {
				parseValue(actor.active);
			} else {
				throw SyntaxError{ParseError::UNKNOWN_ACTOR_KEY, token};
			}
		}
	};

	const auto parseScaleSettings = [&tokens, &parseValue](FaceposerSettings::ScaleSettings& scaleSettings) {
		auto token = tokens.next();
		if (token != "{") {
			throw SyntaxError{ParseError::EXPECTED_OPENING_BRACE, token};
		}
		while ((token = tokens.next()) != "}") {
			if (string::iequals(token, "CChoreoView")) {
				parseValue(scaleSettings.choreoView);
			} else if (string::iequals(token, "ExpressionTool")) {
				parseValue(scaleSettings.expressionTool);
			} else if (string::iequals(token, "RampTool")) {
				parseValue(scaleSettings.rampTool);
			} else if (string::iequals(token, "GestureTool")) {
				parseValue(scaleSettings.gestureTool);
			} else if (string::iequals(token, "SceneRampTool")) {
				parseValue(scaleSettings.sceneRampTool);
			} else {
				throw SyntaxError{ParseError::UNKNOWN_SCENE_SETTINGS_KEY, token};
			}
		}
	};

	try {
		while (true) {
			if (const auto token = tokens.next(); string::iequals(token, "event")) {
				parseEvent(this->globalEvents.emplace_back());
			} else if (string::iequals(token, "actor")) {
				parseActor(this->actors.emplace_back());
			} else if (string::iequals(token, "scalesettings")) {
				FaceposerSettings::ScaleSettings scaleSettings;
				parseScaleSettings(scaleSettings);
				this->faceposerSettings.scaleSettings = scaleSettings;
			} else if (string::iequals(token, "mapname")) {
				parseValue(this->faceposerSettings.mapName);
			} else if (string::iequals(token, "fps")) {
				parseValue(this->faceposerSettings.fps);
			} else if (string::iequals(token, "snap")) {
				parseValue(this->faceposerSettings.snap);
			} else if (string::iequals(token, "ignorePhonemes")) {
				parseValue(this->faceposerSettings.ignorePhonemes);
			} else if (string::iequals(token, "scene_ramp")) {
				Event::Ramp ramp;
				parseRamp(ramp);
				this->sceneRamp = ramp;
			} else {
				throw SyntaxError{ParseError::UNKNOWN_TOP_LEVEL_KEY, token};
			}
		}
	} catch (const std::overflow_error&) {}
}

VCD::Platform VCD::getPlatform() const {
	return this->platform;
}

uint8_t VCD::getVersion() const {
	return this->version;
}

uint32_t VCD::getChecksum() const {
	return this->checksum;
}

const std::vector<Event>& VCD::getGlobalEvents() const {
	return this->globalEvents;
}

const std::vector<Actor>& VCD::getActors() const {
	return this->actors;
}

const std::optional<Event::Ramp>& VCD::getSceneRamp() const {
	return this->sceneRamp;
}

const FaceposerSettings& VCD::getFaceposerSettings() const {
	return this->faceposerSettings;
}
