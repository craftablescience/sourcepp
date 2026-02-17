#include <vcryptppc/VICE.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vcryptpp;

const char* VCRYPTPP_VICE_KNOWN_CODES_DEFAULT                         = VICE::KnownCodes::DEFAULT.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_BLOODY_GOOD_TIME                = VICE::KnownCodes::BLOODY_GOOD_TIME.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_WEAPONS               = VICE::KnownCodes::CONTAGION_WEAPONS.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_SCRIPTS               = VICE::KnownCodes::CONTAGION_SCRIPTS.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_SOURCE           = VICE::KnownCodes::COUNTER_STRIKE_SOURCE.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE = VICE::KnownCodes::COUNTER_STRIKE_GLOBAL_OFFENSIVE.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_2                = VICE::KnownCodes::COUNTER_STRIKE_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_PROMOD           = VICE::KnownCodes::COUNTER_STRIKE_PROMOD.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_DAY_OF_DEFEAT_SOURCE            = VICE::KnownCodes::DAY_OF_DEFEAT_SOURCE.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_2                    = VICE::KnownCodes::DYSTOPIA_1_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_3                    = VICE::KnownCodes::DYSTOPIA_1_3.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_FORTRESS_FOREVER_PRE_GREENLIGHT = VICE::KnownCodes::FORTRESS_FOREVER_PRE_GREENLIGHT.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GOLDEN_EYE_SOURCE               = VICE::KnownCodes::GOLDEN_EYE_SOURCE.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_CTF                 = VICE::KnownCodes::HALF_LIFE_2_CTF.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_DM                  = VICE::KnownCodes::HALF_LIFE_2_DM.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_INSURGENCY                      = VICE::KnownCodes::INSURGENCY.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_LEFT_4_DEAD_2                   = VICE::KnownCodes::LEFT_4_DEAD_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_NO_MORE_ROOM_IN_HELL            = VICE::KnownCodes::NO_MORE_ROOM_IN_HELL.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_NUCLEAR_DAWN                    = VICE::KnownCodes::NUCLEAR_DAWN.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_TACTICAL_INTERVENTION           = VICE::KnownCodes::TACTICAL_INTERVENTION.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2                 = VICE::KnownCodes::TEAM_FORTRESS_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2_ITEMS           = VICE::KnownCodes::TEAM_FORTRESS_2_ITEMS.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_FAIRY_TALE_BUSTERS              = VICE::KnownCodes::FAIRY_TALE_BUSTERS.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_THE_SHIP                        = VICE::KnownCodes::THE_SHIP.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_VALVE_TRACKER                   = VICE::KnownCodes::VALVE_TRACKER.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_ZOMBIE_PANIC_SOURCE             = VICE::KnownCodes::ZOMBIE_PANIC_SOURCE.data();

const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_DEFAULT                     = VICE::KnownCodes::GPU_DEFAULT.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_ALIEN_SWARM                 = VICE::KnownCodes::GPU_ALIEN_SWARM.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_DOTA_2                      = VICE::KnownCodes::GPU_DOTA_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_HALF_LIFE_2_EPISODE_2       = VICE::KnownCodes::GPU_HALF_LIFE_2_EPISODE_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_LEFT_4_DEAD_1               = VICE::KnownCodes::GPU_LEFT_4_DEAD_1.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_LEFT_4_DEAD_2               = VICE::KnownCodes::GPU_LEFT_4_DEAD_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_PORTAL_2                    = VICE::KnownCodes::GPU_PORTAL_2.data();
const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_TEAM_FORTRESS_2             = VICE::KnownCodes::GPU_TEAM_FORTRESS_2.data();

SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(VICE::encrypt(std::span{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}

SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(VICE::decrypt(std::span{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}
