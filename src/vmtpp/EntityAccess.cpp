#include <vmtpp/EntityAccess.h>

#include <chrono>

using namespace sourcepp;
using namespace vmtpp;

EntityAccessEmpty::EntityAccessEmpty() {
	// MLP:FIM S09E20 04:58
	static float randomNumberGeneratorWinkWink = 0.f;
	this->random = randomNumberGeneratorWinkWink++;
}

uint64_t EntityAccessEmpty::getCurrentTime() const {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

float EntityAccessEmpty::getRenderAlpha() const {
	return 1.f;
}

float EntityAccessEmpty::getAnimationProgress() const {
	return 0.f;
}

float EntityAccessEmpty::getDistanceToCurrentPlayer() const {
	return 0.f;
}

int EntityAccessEmpty::getCurrentPlayerTeam() const {
	return 0;
}

int EntityAccessEmpty::getTeam() const {
	return 0;
}

float EntityAccessEmpty::getCurrentPlayerViewDotProduct() const {
	return 0.f;
}

float EntityAccessEmpty::getCurrentPlayerSpeed() const {
	return 0.f;
}

math::Vec3f EntityAccessEmpty::getCurrentPlayerPosition() const {
	return {};
}

float EntityAccessEmpty::getSpeed() const {
	return 0.f;
}

math::Vec3f EntityAccessEmpty::getOrigin() const {
	return {};
}

float EntityAccessEmpty::getRandomNumber() const {
	return this->random;
}

float EntityAccessEmpty::getHealth() const {
	return 0.f;
}

bool EntityAccessEmpty::isNPC() const {
	return false;
}

bool EntityAccessEmpty::isViewModel() const {
	return false;
}

math::Vec3f EntityAccessEmpty::getWorldDimensionsMinimum() const {
	return {};
}

math::Vec3f EntityAccessEmpty::getWorldDimensionsMaximum() const {
	return {};
}

math::Vec3f EntityAccessEmpty::getCurrentPlayerCrosshairColor() const {
	return {1.f, 1.f, 1.f};
}
