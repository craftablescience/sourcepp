#include <dmxpp/structs/Value.h>

#include <sstream>

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

bool Attribute::isArray() const {
	return this->type >= Value::ID::ARRAY_START;
}

std::string Attribute::getValue() const {
	switch (this->type) {
		using enum Value::ID;
		case INVALID:
			return Value::IDToString(this->type);
		case ELEMENT: {
			auto element = this->getValueAs<Value::Element>();
			if (element.index == -2) {
				return "GUID: " + element.stubGUID;
			}
			return '#' + std::to_string(element.index);
		}
		case INT:
			return std::to_string(this->getValueAs<int32_t>());
		case FLOAT:
			return std::to_string(this->getValueAs<float>());
		case BOOL:
			return this->getValueAs<bool>() ? "true" : "false";
		case STRING:
			return this->getValueAs<std::string>();
		case BYTEARRAY: {
			std::string out;
			for (auto byte : this->getValueAs<Value::ByteArray>()) {
				std::stringstream ss;
				ss << std::hex << std::uppercase;
				ss << static_cast<unsigned char>(byte);
				out += ss.str();
			}
			return out;
		}
		case TIME:
			return std::to_string(this->getValueAs<float>());
		case COLOR: {
			auto color = this->getValueAs<Value::Color>();
			return "rgba(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ", " + std::to_string(color.a) + ')';
		}
		case VECTOR2: {
			auto vec2 = this->getValueAs<Value::Vector2>();
			return '[' + std::to_string(vec2.x) + ", " + std::to_string(vec2.y) + ']';
		}
		case VECTOR3:
		case EULER_ANGLE: {
			auto vec3 = this->getValueAs<Value::Vector3>();
			return '[' + std::to_string(vec3.x) + ", " + std::to_string(vec3.y) + ", " + std::to_string(vec3.z) + ']';
		}
		case VECTOR4:
		case QUATERNION: {
			auto vec4 = this->getValueAs<Value::Vector4>();
			return '[' + std::to_string(vec4.x) + ", " + std::to_string(vec4.y) + ", " + std::to_string(vec4.z) + ", " + std::to_string(vec4.w) + ']';
		}
		case MATRIX_4X4: {
			auto mat4 = this->getValueAs<Value::Matrix4x4>();
			std::string out;
			for (int i = 0; i < 4; i++) {
				out += (i == 0 ? '[' : ' ');
				for (int j = 0; j < 4; j++) {
					out += std::to_string(mat4(i,j));
					if (j < 3) {
						out += ", ";
					} else if (i < 3) {
						out += ",\n";
					} else {
						out += ']';
					}
				}
			}
			return out;
		}
		case ARRAY_ELEMENT: {
			auto elements = this->getValueAs<std::vector<Value::Element>>();
			std::string out = "[";
			for (int i = 0; i < elements.size(); i++) {
				if (elements[i].index == -2) {
					out += (i == 0 ? "" : " ") + std::string{"GUID: "} + elements[i].stubGUID + (i == elements.size() - 1 ? "" : ",");
				} else {
					out += (i == 0 ? "" : " ") + std::string{"#"} + std::to_string(elements[i].index) + (i == elements.size() - 1 ? "" : ",");
				}
			}
			return out + ']';
		}
		case ARRAY_INT: {
			auto ints = this->getValueAs<std::vector<int>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(ints[i]) + (i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_FLOAT: {
			auto floats = this->getValueAs<std::vector<float>>();
			std::string out = "[";
			for (int i = 0; i < floats.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(floats[i]) + (i == floats.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_BOOL: {
			auto bools = this->getValueAs<std::vector<bool>>();
			std::string out = "[";
			for (int i = 0; i < bools.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{bools[i] ? "true" : "false"} + (i == bools.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_STRING: {
			auto strings = this->getValueAs<std::vector<std::string>>();
			std::string out = "[";
			for (int i = 0; i < strings.size(); i++) {
				out += (i == 0 ? "" : " ") + strings[i] + (i == strings.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_BYTEARRAY: {
			auto bytearrays = this->getValueAs<std::vector<Value::ByteArray>>();
			std::string out = "[";
			for (int i = 0; i < bytearrays.size(); i++) {
				std::string hex;
				for (auto byte : bytearrays[i]) {
					std::stringstream ss;
					ss << std::hex << std::uppercase;
					ss << static_cast<unsigned char>(byte);
					hex += ss.str();
				}
				out += (i == 0 ? "" : " ") + hex + (i == bytearrays.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_TIME: {
			auto times = this->getValueAs<std::vector<float>>();
			std::string out = "[";
			for (int i = 0; i < times.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(times[i]) + (i == times.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_COLOR: {
			auto colors = this->getValueAs<std::vector<Value::Color>>();
			std::string out = "[";
			for (int i = 0; i < colors.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"rgba("} + std::to_string(colors[i].r) + ", " + std::to_string(colors[i].g) + ", " + std::to_string(colors[i].b) + ", " + std::to_string(colors[i].a) + ')' + (i == colors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR2: {
			auto vectors = this->getValueAs<std::vector<Value::Vector2>>();
			std::string out = "[";
			for (int i = 0; i < vectors.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vectors[i].x) + ", " + std::to_string(vectors[i].y) + ']' + (i == vectors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR3:
		case ARRAY_EULER_ANGLE: {
			auto vectors = this->getValueAs<std::vector<Value::Vector3>>();
			std::string out = "[";
			for (int i = 0; i < vectors.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vectors[i].x) + ", " + std::to_string(vectors[i].y) + ", " + std::to_string(vectors[i].z) + ']' + (i == vectors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR4:
		case ARRAY_QUATERNION: {
			auto vectors = this->getValueAs<std::vector<Value::Vector4>>();
			std::string out = "[";
			for (int i = 0; i < vectors.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vectors[i].x) + ", " + std::to_string(vectors[i].y) + ", " + std::to_string(vectors[i].z) + ']' + ", " + std::to_string(vectors[i].w) + ']' + (i == vectors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_MATRIX_4X4: {
			auto matrices = this->getValueAs<std::vector<Value::Matrix4x4>>();
			std::string out = "[";
			for (int m = 0; m < matrices.size(); m++) {
				out += (m == 0 ? "[" : " [");
				for (int i = 0; i < 4; i++) {
					out += (i == 0 ? "" : "  ");
					for (int j = 0; j < 4; j++) {
						out += std::to_string(matrices[m](i,j));
						if (j < 3) {
							out += ", ";
						} else if (i < 3) {
							out += ",\n";
						} else {
							out += ']';
						}
					}
				}
				if (m < matrices.size() - 1) {
					out += ",\n";
				}
			}
			return out + ']';
		}
		default:
			break;
	}
	return "";
}
