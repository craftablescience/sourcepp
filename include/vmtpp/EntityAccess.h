#pragma once

#include <sourcepp/Math.h>

namespace vmtpp {

/// Expose an interface to read values from the entity the VMT is attached to for material proxies.
class IEntityAccess {
public:
	virtual ~IEntityAccess() = default;

	/// "The number of seconds the current map has been running on the server for."
	[[nodiscard]] virtual uint64_t getCurrentTime() const = 0;

	[[nodiscard]] virtual float getRenderAlpha() const = 0;

	[[nodiscard]] virtual float getAnimationProgress() const = 0;

	/// "The distance in units between the current player and the origin of the entity that the material is applied to."
	[[nodiscard]] virtual float getDistanceToCurrentPlayer() const = 0;

	[[nodiscard]] virtual int getCurrentPlayerTeam() const = 0;

	[[nodiscard]] virtual int getTeam() const = 0;

	/// "The dot product of the current player's view angle and the relative origin of the material's entity."
	[[nodiscard]] virtual float getCurrentPlayerViewDotProduct() const = 0;

	[[nodiscard]] virtual float getCurrentPlayerSpeed() const = 0;

	[[nodiscard]] virtual sourcepp::math::Vec3f getCurrentPlayerPosition() const = 0;

	[[nodiscard]] virtual float getSpeed() const = 0;

	[[nodiscard]] virtual sourcepp::math::Vec3f getOrigin() const = 0;

	/// "A static random number associated with the entity the material is applied to."
	[[nodiscard]] virtual float getRandomNumber() const = 0;

	[[nodiscard]] virtual float getHealth() const = 0;

	[[nodiscard]] virtual bool isNPC() const = 0;

	[[nodiscard]] virtual bool isViewModel() const = 0;

	[[nodiscard]] virtual sourcepp::math::Vec3f getWorldDimensionsMinimum() const = 0;

	[[nodiscard]] virtual sourcepp::math::Vec3f getWorldDimensionsMaximum() const = 0;

	[[nodiscard]] virtual sourcepp::math::Vec3f getCurrentPlayerCrosshairColor() const = 0;
};

class EntityAccessEmpty : public IEntityAccess {
public:
	EntityAccessEmpty();

	[[nodiscard]] uint64_t getCurrentTime() const override;

	[[nodiscard]] float getRenderAlpha() const override;

	[[nodiscard]] float getAnimationProgress() const override;

	[[nodiscard]] float getDistanceToCurrentPlayer() const override;

	[[nodiscard]] int getCurrentPlayerTeam() const override;

	[[nodiscard]] int getTeam() const override;

	[[nodiscard]] float getCurrentPlayerViewDotProduct() const override;

	[[nodiscard]] float getCurrentPlayerSpeed() const override;

	[[nodiscard]] sourcepp::math::Vec3f getCurrentPlayerPosition() const override;

	[[nodiscard]] float getSpeed() const override;

	[[nodiscard]] sourcepp::math::Vec3f getOrigin() const override;

	[[nodiscard]] float getRandomNumber() const override;

	[[nodiscard]] float getHealth() const override;

	[[nodiscard]] bool isNPC() const override;

	[[nodiscard]] bool isViewModel() const override;

	[[nodiscard]] sourcepp::math::Vec3f getWorldDimensionsMinimum() const override;

	[[nodiscard]] sourcepp::math::Vec3f getWorldDimensionsMaximum() const override;

	[[nodiscard]] sourcepp::math::Vec3f getCurrentPlayerCrosshairColor() const override;

private:
	float random;
};

} // namespace vmtpp
