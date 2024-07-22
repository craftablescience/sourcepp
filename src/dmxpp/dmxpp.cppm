module;

#include <dmxpp/dmxpp.h>

export module dmxpp;

export {

	// structs/Value.h
	namespace dmxpp {
		namespace Value {
			using dmxpp::Value::Invalid;
			using dmxpp::Value::Element;
			using dmxpp::Value::ByteArray;
			using dmxpp::Value::Time;
			using dmxpp::Value::Color;
			using dmxpp::Value::Vector2;
			using dmxpp::Value::Vector3;
			using dmxpp::Value::Vector4;
			using dmxpp::Value::EulerAngles;
			using dmxpp::Value::Quaternion;
			using dmxpp::Value::Matrix4x4;
			using dmxpp::Value::Generic;
			using dmxpp::Value::ID;
			using dmxpp::Value::IDToByte;
			using dmxpp::Value::byteToID;
			using dmxpp::Value::arrayIDToInnerID;
			using dmxpp::Value::innerIDToArrayID;
			using dmxpp::Value::IDToString;
			using dmxpp::Value::stringToID;
		} // namespace Value

		using dmxpp::DMXAttribute;
		using dmxpp::DMXElement;
	} // namespace dmxpp

	// dmxpp.h
	namespace dmxpp {
		constexpr auto FORMAT_TEXT = Format::TEXT;
		constexpr auto FORMAT_BINARY = Format::BINARY;
		using dmxpp::DMX;
	} // namespace dmxpp

} // export
