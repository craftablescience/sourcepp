#pragma once

#include <sourceppc/Buffer.h>

extern const char* VCRYPTPP_VICE_KNOWN_CODES_DEFAULT;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_BLOODY_GOOD_TIME;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_WEAPONS;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_SCRIPTS;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_SOURCE;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_PROMOD;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_DAY_OF_DEFEAT_SOURCE;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_3;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_FORTRESS_FOREVER_PRE_GREENLIGHT;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GOLDEN_EYE_SOURCE;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_CTF;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_DM;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_INSURGENCY;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_LEFT_4_DEAD_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_NO_MORE_ROOM_IN_HELL;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_NUCLEAR_DAWN;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_TACTICAL_INTERVENTION;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2_ITEMS;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_FAIRY_TALE_BUSTERS;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_THE_SHIP;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_VALVE_TRACKER;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_ZOMBIE_PANIC_SOURCE;

extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_DEFAULT;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_ALIEN_SWARM;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_DOTA_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_HALF_LIFE_2_EPISODE_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_LEFT_4_DEAD_1;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_LEFT_4_DEAD_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_PORTAL_2;
extern const char* VCRYPTPP_VICE_KNOWN_CODES_GPU_TEAM_FORTRESS_2;

SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code); // REQUIRES MANUAL FREE: sourcepp_buffer_free

// C++ conversion routines
#ifdef __cplusplus

#include <vcryptpp/vcryptpp.h>

#endif
