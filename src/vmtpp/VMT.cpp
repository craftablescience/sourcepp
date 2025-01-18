#include <vmtpp/VMT.h>

#include <algorithm>
#include <utility>

#include <kvpp/kvpp.h>
#include <sourcepp/String.h>
#include <vmtpp/EntityAccess.h>

using namespace kvpp;
using namespace sourcepp;
using namespace vmtpp;

Value::Type Value::getProbableType(std::string_view value) {
	if (auto spaceCount = std::count(value.begin(), value.end(), ' '); spaceCount >= 3) {
		return Type::COLOR;
	} else if (spaceCount == 2) {
		return Type::VEC3;
	} else if (value.find('.') != std::string_view::npos || value.find('e') != std::string_view::npos) {
		return Type::FLOAT;
	} else {
		return Type::INT;
	}
}

Value::Type Value::getProbableTypeBasedOnAssociatedValues(std::string_view value, std::initializer_list<std::string_view> others) {
	if (auto type = getProbableType(value); type != Type::INT) {
		return type;
	}
	for (auto other : others) {
		if (getProbableType(other) == Type::FLOAT) {
			return Type::FLOAT;
		}
	}
	return Type::INT;
}

int Value::toInt(std::string_view value) {
	int num = 0;
	string::toInt(string::trim(value), num);
	return num;
}

std::string Value::fromInt(int value) {
	return std::to_string(value);
}

float Value::toFloat(std::string_view value) {
	float num = 0.f;
	string::toFloat(string::trim(value), num);
	return num;
}

std::string Value::fromFloat(float value) {
	return std::to_string(value);
}

math::Vec3f Value::toVec3(std::string_view value) {
	float scale = 1.f;
	if (value.starts_with('{')) {
		scale = 1.f / 255.f;
	}
	auto values = string::split(string::trim(string::trim(value, "{}[]")), ' ');
	math::Vec3f out{};
	if (values.size() != 3) {
		return out;
	}
	string::toFloat(values[0], out[0]);
	string::toFloat(values[1], out[1]);
	string::toFloat(values[2], out[2]);
	out *= scale;
	return out;
}

std::string Value::fromVec3(math::Vec3f value) {
	return '[' + std::to_string(value[0]) + ' ' + std::to_string(value[1]) + ' ' + std::to_string(value[2]) + ']';
}

math::Vec4f Value::toColor(std::string_view value) {
	float scale = 1.f;
	if (value.starts_with('{')) {
		scale = 1.f / 255.f;
	}
	auto values = string::split(string::trim(string::trim(value, "{}[]")), ' ');
	math::Vec4f out{};
	if (values.size() != 3 && values.size() != 4) {
		return out;
	}
	string::toFloat(values[0], out[0]);
	string::toFloat(values[1], out[1]);
	string::toFloat(values[2], out[2]);
	out *= scale;
	if (values.size() == 4) {
		string::toFloat(values[3], out[3]);
		out[4] *= scale;
	} else {
		out[4] = 1.f;
	}
	return out;
}

std::string Value::fromColor(math::Vec4f value) {
	return '[' + std::to_string(value[0]) + ' ' + std::to_string(value[1]) + ' ' + std::to_string(value[2]) + (value[3] == 1.f ? (' ' + std::to_string(value[3])) : "") + ']';
}

VMT::VMT(std::string_view vmt, const IEntityAccess& entityAccess_, int dxLevel, int shaderDetailLevel, std::string_view shaderFallbackSuffix)
		: entityAccess(entityAccess_) {
	KV1 keyvalues{vmt};
	if (keyvalues.getChildCount() < 1) {
		return;
	}

	// Read shader name
	this->shader = keyvalues[0].getKey();

	for (const auto& element : keyvalues[0].getChildren()) {
		// Add compile flags
		if (element.getKey().starts_with('%') && Value::toInt(element.getValue())) {
			this->compileFlags.emplace_back(element.getKey().substr(1));
			string::toLower(this->compileFlags.back());
		}

		// todo: careful parsing! ALL keys need to be lowercased and values need to be normalized for value getters and proxies to work

		// todo: check every key for gpu levels / dx levels / shader fallbacks / proxies

		this->variables[string::toLower(element.getKey().substr(1))] = element.getValue();
	}
}

std::string_view VMT::getShader() const {
	return this->shader;
}

bool VMT::hasCompileFlag(std::string_view flag) const {
	if (flag.starts_with('%')) {
		flag = flag.substr(1);
	}
	return std::any_of(this->compileFlags.begin(), this->compileFlags.end(), [flag_=string::toLower(flag)](const std::string& existingFlag) {
		return existingFlag == flag_;
	});
}

const std::vector<std::string>& VMT::getCompileFlags() const {
	return this->compileFlags;
}

bool VMT::hasVariable(std::string_view key) const {
	if (key.starts_with('$')) {
		key = key.substr(1);
	}
	return this->variables.contains(string::toLower(key));
}

std::string_view VMT::getVariable(std::string_view key) const {
	if (key.starts_with('$')) {
		key = key.substr(1);
	}
	const auto keyLower = string::toLower(key);
	if (!this->variables.contains(keyLower)) {
		return "";
	}
	return this->variables.at(string::toLower(key));
}

std::string_view VMT::operator[](std::string_view key) const {
	return this->getVariable(key);
}

void VMT::update() {
	for (auto& proxy : this->proxies) {
		Proxy::exec(proxy, this->variables, this->entityAccess);
	}
}
