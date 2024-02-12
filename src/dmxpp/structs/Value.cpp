#include <dmxpp/structs/Value.h>

using namespace dmxpp;

// NOLINTNEXTLINE(*-no-recursion)
std::string Value::IDToString(ID id) {
	switch (id) {
		case ID::INVALID:
			break;
		case ID::ELEMENT:
			return "element";
		case ID::INT:
			return "int";
		case ID::FLOAT:
			return "float";
		case ID::BOOL:
			return "bool";
		case ID::STRING:
			return "string";
		case ID::BYTEARRAY:
			return "binary";
		case ID::TIME:
			return "time";
		case ID::COLOR:
			return "color";
		case ID::VECTOR2:
			return "vector2";
		case ID::EULER_ANGLE:
		case ID::VECTOR3:
			return "vector3";
		case ID::VECTOR4:
			return "vector4";
		case ID::QUATERNION:
			return "quaternion";
		case ID::MATRIX_4X4:
			return "matrix";
		case ID::ARRAY_ELEMENT:
		case ID::ARRAY_INT:
		case ID::ARRAY_FLOAT:
		case ID::ARRAY_BOOL:
		case ID::ARRAY_STRING:
		case ID::ARRAY_BYTEARRAY:
		case ID::ARRAY_TIME:
		case ID::ARRAY_COLOR:
		case ID::ARRAY_VECTOR2:
		case ID::ARRAY_EULER_ANGLE:
		case ID::ARRAY_VECTOR3:
		case ID::ARRAY_VECTOR4:
		case ID::ARRAY_QUATERNION:
		case ID::ARRAY_MATRIX_4X4:
			return IDToString(arrayIDToInnerID(id)) + "_array";
	}
	return "invalid";
}
