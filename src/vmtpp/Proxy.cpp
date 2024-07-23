#include <vmtpp/Proxy.h>

#include <algorithm>

#include <vmtpp/EntityAccess.h>
#include <vmtpp/VMT.h>

using namespace sourcepp;
using namespace vmtpp;

/*
 * Unimplemented proxies and rationale:
 *
 * - AnimatedEntityTexture   | treated the same as AnimatedTexture, maybe added later, need to figure out the interface
 * - AnimateSpecificTexture  | unused
 * - BreakableSurface        | maybe added later, need to figure out the interface
 * - ConveyorScroll          | maybe added later, need to figure out the interface
 * - Camo                    | unused, broken
 * - Dummy                   | pointless
 * - FleshInterior           | niche, don't understand how it works
 * - MaterialModify          | maybe added later, need to figure out the interface
 * - MaterialModifyAnimated  | maybe added later, need to figure out the interface
 * - ParticleSphereProxy     | used by one HL2 material, possibly pointless
 * - WaterLOD                | maybe added later, don't know how this entity works
 * - CustomSteamImageOnModel | TF2/CSGO-specific, there's no point to implementing an empty proxy
 * - invis                   | TF2-specific, implementation details unclear
 * - spy_invis               | TF2-specific, implementation details unclear
 * - weapon_invis            | TF2-specific, implementation details unclear
 * - vm_invis                | TF2-specific, implementation details unclear
 * - building_invis          | TF2-specific, unused, broken
 * - AnimatedWeaponSheen     | TF2-specific, implementation details unclear
 * - WeaponSkin              | TF2-specific, there's no point to implementing an empty proxy
 * - BBQLevel                | L4D2-specific, ???
 * - PortalOpenAmount        | P1/2-specific, should really be handled somewhere else
 * - PortalStaticModel       | P1/2-specific, implementation details unclear
 * - PortalStatic            | P1/2-specific, should really be handled somewhere else
 * - PortalPickAlphaMask     | P1/2-specific, implementation details unclear
 * - WheatlyEyeGlow          | P2-specific, unused, broken
 * - LightedFloorButton      | P2-specific, unused, broken
 * - survivalteammate        | CSGO-specific, should really be handled somewhere else
 * - MoneyProxy              | CSGO-specific, niche
 * - WeaponLabelText         | CSGO-specific, niche
 * - C4CompassArrow          | CSGO-specific, niche, implementation details unknown
 *
 * Proxies still left to implement:
 *
 * - SelectFirstIfNonZero
 * - WrapMinMax
 * - Exponential
 * - Sine
 * - LinearRamp
 * - CurrentTime
 * - UniformNoise
 * - GaussianNoise
 * - MatrixRotate
 * - Alpha
 * - Cycle
 * - PlayerProximity
 * - PlayerTeamMatch
 * - PlayerView
 * - PlayerSpeed
 * - PlayerPosition
 * - EntitySpeed
 * - EntityOrigin
 * - EntityRandom
 * - Health
 * - IsNPC
 * - WorldDims
 * - CrosshairColor
 * - AnimatedTexture / AnimatedOffsetTexture
 * - Pupil
 * - TextureTransform
 * - TextureScroll
 * - LampBeam
 * - LampHalo
 * - HeliBlade
 * - PlayerLogo
 * - Shadow
 * - ShadowModel
 * - Thermal
 * - ToggleTexture
 * - Empty
 * - ConVar
 * - EntityOriginAlyx
 * - Ep1IntroVortRefract
 * - VortEmissive
 * - Shield
 * - CommunityWeapon
 * - InvulnLevel
 * - BurnLevel
 * - YellowLevel
 * - ModelGlowColor
 * - ItemTintColor
 * - BuildingRescueLevel
 * - TeamTexture
 * - WeaponSkin
 * - ShieldFalloff
 * - StatTrakIllum
 * - StatTrakDigit
 * - StatTrakIcon
 * - StickybombGlowColor
 * - SniperRifleCharge
 * - Heartbeat
 * - WheatlyEyeGlow
 * - BenefactorLevel
 * - PlayerTeam
 * - BloodyHands
 * - IT
 * - BurnLevel
 * - NightVisionSelfIllum
 * - AlienSurfaceFX
 * - LanguagePreference
 * - FizzlerVortex
 * - Lightedmouth
 * - TractorBeam
 * - TauCharge
 * - Select
 * - GetTeamNumber
 * - RemapValClamp
 * - IronSightAmount
 * - ApproachValue
 */

namespace {

std::unordered_map<std::string, Proxy::Function>& getRegisteredProxies() {
	static std::unordered_map<std::string, Proxy::Function> proxies;
	return proxies;
}

} // namespace

Proxy::Function Proxy::add(const std::string& name, Function proxy) {
	getRegisteredProxies()[name] = proxy;
	return proxy;
}

Proxy::Function Proxy::get(const std::string& name) {
	if (!getRegisteredProxies().contains(name)) {
		return nullptr;
	}
	return getRegisteredProxies()[name];
}

void Proxy::exec(Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess& entity) {
	if (auto func = get(data.name)) {
		func(data, vmtVariables, entity);
	}
}

void Proxy::remove(const std::string& name) {
	getRegisteredProxies().erase(name);
}

/**
 * Add - add two variables
 * in: srcVar1
 * in: srcVar2
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Add, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	// todo: srcvars value might be an index into a vector variable
	if (!vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	auto type = Value::getProbableTypeBasedOnAssociatedValues(srcvar1, {srcvar2});
	if (type != Value::getProbableType(srcvar2)) {
		return;
	}
	switch (type) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(Value::toInt(srcvar1) + Value::toInt(srcvar2));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(Value::toFloat(srcvar1) + Value::toFloat(srcvar2));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1) + Value::toVec3(srcvar2));
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1) + Value::toColor(srcvar2));
			return;
	}
}));

/**
 * Subtract - subtract two variables
 * in: srcVar1
 * in: srcVar2
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Subtract, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	auto type = Value::getProbableTypeBasedOnAssociatedValues(srcvar1, {srcvar2});
	if (type != Value::getProbableType(srcvar2)) {
		return;
	}
	switch (type) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(Value::toInt(srcvar1) - Value::toInt(srcvar2));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(Value::toFloat(srcvar1) - Value::toFloat(srcvar2));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1) - Value::toVec3(srcvar2));
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1) - Value::toColor(srcvar2));
			return;
	}
}));

/**
 * Multiply - multiply two variables
 * in: srcVar1
 * in: srcVar2
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Multiply, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	auto type = Value::getProbableTypeBasedOnAssociatedValues(srcvar1, {srcvar2});
	if (type != Value::getProbableType(srcvar2)) {
		return;
	}
	switch (type) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(Value::toInt(srcvar1) * Value::toInt(srcvar2));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(Value::toFloat(srcvar1) * Value::toFloat(srcvar2));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1).mul(Value::toVec3(srcvar2)));
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1).mul(Value::toColor(srcvar2)));
			return;
	}
}));

/**
 * Divide - divide two variables
 * in: srcVar1
 * in: srcVar2
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Divide, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	auto type = Value::getProbableTypeBasedOnAssociatedValues(srcvar1, {srcvar2});
	if (type != Value::getProbableType(srcvar2)) {
		return;
	}
	switch (type) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(Value::toInt(srcvar1) / Value::toInt(srcvar2));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(Value::toFloat(srcvar1) / Value::toFloat(srcvar2));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1).div(Value::toVec3(srcvar2)));
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1).div(Value::toColor(srcvar2)));
			return;
	}
}));

/**
 * Modulo - take the modulus of two variables
 * in: srcVar1
 * in: srcVar2
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Modulo, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	auto type = Value::getProbableTypeBasedOnAssociatedValues(srcvar1, {srcvar2});
	if (type != Value::getProbableType(srcvar2)) {
		return;
	}
	switch (type) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(Value::toInt(srcvar1) % Value::toInt(srcvar2));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(std::fmod(Value::toFloat(srcvar1), Value::toFloat(srcvar2)));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1).mod(Value::toVec3(srcvar2)));
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1).mod(Value::toColor(srcvar2)));
			return;
	}
}));

/**
 * Equals - set one variable equal to another
 * in: srcVar1
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Equals, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"])) {
		return;
	}
	vmtVariables[data.variables["resultvar"]] = vmtVariables[data.variables["srcvar1"]];
}));

/**
 * Abs - compute the absolute value of a variable
 * in: srcVar1
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Abs, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	switch (Value::getProbableType(srcvar1)) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(std::abs(Value::toInt(srcvar1)));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(std::abs(Value::toFloat(srcvar1)));
			return;
		case Value::Type::VEC3:
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(Value::toVec3(srcvar1).abs());
			return;
		case Value::Type::COLOR:
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(Value::toColor(srcvar1).abs());
			return;
	}
}));

/**
 * Frac - get the decimal part of a variable
 * in: srcVar1
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Frac, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	switch (Value::getProbableType(srcvar1)) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = srcvar1;
			return;
		case Value::Type::FLOAT: {
			float out;
			std::modf(Value::toFloat(srcvar1), &out);
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(out);
			return;
		}
		case Value::Type::VEC3: {
			math::Vec3f out;
			auto in = Value::toVec3(srcvar1);
			std::modf(in[0], &out[0]);
			std::modf(in[1], &out[1]);
			std::modf(in[2], &out[2]);
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(out);
			return;
		}
		case Value::Type::COLOR: {
			math::Vec4f out;
			auto in = Value::toColor(srcvar1);
			std::modf(in[0], &out[0]);
			std::modf(in[1], &out[1]);
			std::modf(in[2], &out[2]);
			std::modf(in[3], &out[3]);
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(out);
			return;
		}
	}
}));

/**
 * Int - get the integer part of a variable
 * in: srcVar1
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Int, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("srcvar1") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["srcvar1"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	switch (Value::getProbableType(srcvar1)) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = srcvar1;
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(static_cast<int>(Value::toFloat(srcvar1)));
			return;
		case Value::Type::VEC3: {
			math::Vec3f out;
			auto in = Value::toVec3(srcvar1);
			out[0] = static_cast<int>(in[0]);
			out[1] = static_cast<int>(in[1]);
			out[2] = static_cast<int>(in[2]);
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(out);
			return;
		}
		case Value::Type::COLOR: {
			math::Vec4f out;
			auto in = Value::toColor(srcvar1);
			out[0] = static_cast<int>(in[0]);
			out[1] = static_cast<int>(in[1]);
			out[2] = static_cast<int>(in[2]);
			out[3] = static_cast<int>(in[3]);
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(out);
			return;
		}
	}
}));

/**
 * Clamp - clamp a variable's value between two ends
 * in: min <float>
 * in: max <float>
 * in: srcVar1
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(Clamp, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("min") || !data.variables.contains("max") || !data.variables.contains("srcvar1") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["min"]) || !vmtVariables.contains(data.variables["max"]) || !vmtVariables.contains(data.variables["srcvar1"])) {
		return;
	}
	const auto& min = vmtVariables[data.variables["min"]];
	const auto& max = vmtVariables[data.variables["max"]];
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	switch (Value::getProbableType(srcvar1)) {
		case Value::Type::INT:
			vmtVariables[data.variables["resultvar"]] = Value::fromInt(static_cast<int>(std::clamp(Value::toFloat(srcvar1), Value::toFloat(min), Value::toFloat(max))));
			return;
		case Value::Type::FLOAT:
			vmtVariables[data.variables["resultvar"]] = Value::fromFloat(std::clamp(Value::toFloat(srcvar1), Value::toFloat(min), Value::toFloat(max)));
			return;
		case Value::Type::VEC3: {
			math::Vec3f out;
			auto in = Value::toVec3(srcvar1);
			out[0] = std::clamp(in[0], Value::toFloat(min), Value::toFloat(max));
			out[1] = std::clamp(in[1], Value::toFloat(min), Value::toFloat(max));
			out[2] = std::clamp(in[2], Value::toFloat(min), Value::toFloat(max));
			vmtVariables[data.variables["resultvar"]] = Value::fromVec3(out);
			return;
		}
		case Value::Type::COLOR: {
			math::Vec4f out;
			auto in = Value::toColor(srcvar1);
			out[0] = std::clamp(in[0], Value::toFloat(min), Value::toFloat(max));
			out[1] = std::clamp(in[1], Value::toFloat(min), Value::toFloat(max));
			out[2] = std::clamp(in[2], Value::toFloat(min), Value::toFloat(max));
			out[3] = std::clamp(in[3], Value::toFloat(min), Value::toFloat(max));
			vmtVariables[data.variables["resultvar"]] = Value::fromColor(out);
			return;
		}
	}
}));

/**
 * LessOrEqual - compares the first value to the second
 * in: lessEqualVar
 * in: greaterVar
 * in: srcVar1 <float>
 * in: srcVar2 <float>
 * out: resultVar
 */
VMTPP_MATERIAL_PROXY(LessOrEqual, ([](Proxy::Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess&) {
	if (!data.variables.contains("lessequalvar") || !data.variables.contains("greatervar") || !data.variables.contains("srcvar1") || !data.variables.contains("srcvar2") || !data.variables.contains("resultvar")) {
		return;
	}
	if (!vmtVariables.contains(data.variables["lessequalvar"]) || !vmtVariables.contains(data.variables["greatervar"]) || !vmtVariables.contains(data.variables["srcvar1"]) || !vmtVariables.contains(data.variables["srcvar2"])) {
		return;
	}
	const auto& srcvar1 = vmtVariables[data.variables["srcvar1"]];
	const auto& srcvar2 = vmtVariables[data.variables["srcvar2"]];
	if (Value::toFloat(srcvar1) <= Value::toFloat(srcvar2)) {
		vmtVariables[data.variables["resultvar"]] = vmtVariables[data.variables["lessequalvar"]];
	} else {
		vmtVariables[data.variables["resultvar"]] = vmtVariables[data.variables["greatervar"]];
	}
}));
