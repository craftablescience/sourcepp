#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "EntityAccess.h"
#include "Proxy.h"

namespace vmtpp {

namespace Value {

enum class Type {
	INT,
	FLOAT,
	VEC3,
	COLOR,
};

[[nodiscard]] Type getProbableType(std::string_view value);

[[nodiscard]] Type getProbableTypeBasedOnAssociatedValues(std::string_view value, std::initializer_list<std::string_view> others);

[[nodiscard]] int toInt(std::string_view value);

[[nodiscard]] std::string fromInt(int value);

[[nodiscard]] float toFloat(std::string_view value);

[[nodiscard]] std::string fromFloat(float value);

[[nodiscard]] sourcepp::math::Vec3f toVec3(std::string_view value);

[[nodiscard]] std::string fromVec3(sourcepp::math::Vec3f value);

[[nodiscard]] sourcepp::math::Vec4f toColor(std::string_view value);

[[nodiscard]] std::string fromColor(sourcepp::math::Vec4f value);

} // namespace Value

class VMT {
public:
	explicit VMT(std::string_view vmt, const IEntityAccess& entityAccess_ = EntityAccessEmpty{}, int dxLevel = 98, int shaderDetailLevel = 3, std::string_view shaderFallbackSuffix = "DX9");

	[[nodiscard]] std::string_view getShader() const;

	[[nodiscard]] bool hasCompileFlag(std::string_view flag) const;

	[[nodiscard]] const std::vector<std::string>& getCompileFlags() const;

	[[nodiscard]] bool hasVariable(std::string_view key) const;

	[[nodiscard]] std::string_view getVariable(std::string_view key) const;

	[[nodiscard]] std::string_view operator[](std::string_view key) const;

	void update();

private:
	const IEntityAccess& entityAccess;
	std::string shader;
	std::vector<std::string> compileFlags;
	std::unordered_map<std::string, std::string> variables;
	std::vector<Proxy::Data> proxies;
};

} // namespace vmtpp
