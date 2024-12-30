#pragma once

#include <sourceppc/Buffer.h>

#define VCRYPTPP_VICE_KNOWN_CODES_DEFAULT                         "x9Ke0BY7";
#define VCRYPTPP_VICE_BLOODY_GOOD_TIME                            "K4PeJwL7"
#define VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_WEAPONS               "fUk0fF69"
#define VCRYPTPP_VICE_KNOWN_CODES_CONTAGION_SCRIPTS               "5!rrFz6p"
#define VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_SOURCE           "d7NSuLq2"
#define VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE VICEPP_KNOWN_CODES_COUNTER_STRIKE_SOURCE
#define VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_2                VICEPP_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE
#define VCRYPTPP_VICE_KNOWN_CODES_COUNTER_STRIKE_PROMOD           "H1aRQ0n1"
#define VCRYPTPP_VICE_KNOWN_CODES_DAY_OF_DEFEAT_SOURCE            "Wl0u5B3F"
#define VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_2                    "pH3apO8w"
#define VCRYPTPP_VICE_KNOWN_CODES_DYSTOPIA_1_3                    "G8stUh3F"
#define VCRYPTPP_VICE_KNOWN_CODES_GOLDEN_EYE_SOURCE               "Gr3naDes"
#define VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_CTF                 "R1dj3axP"
#define VCRYPTPP_VICE_KNOWN_CODES_HALF_LIFE_2_DM                  VICEPP_KNOWN_CODES_DEFAULT
#define VCRYPTPP_VICE_KNOWN_CODES_INSURGENCY                      "DrA5e3EB"
#define VCRYPTPP_VICE_KNOWN_CODES_LEFT_4_DEAD_2                   "SDhfi878"
#define VCRYPTPP_VICE_KNOWN_CODES_NO_MORE_ROOM_IN_HELL            "lREeeapA"
#define VCRYPTPP_VICE_KNOWN_CODES_NUCLEAR_DAWN                    "TA+*veh9"
#define VCRYPTPP_VICE_KNOWN_CODES_TACTICAL_INTERVENTION           "71B4Dt1Z"
#define VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2                 "E2NcUkG2"
#define VCRYPTPP_VICE_KNOWN_CODES_TEAM_FORTRESS_2_ITEMS           "A5fSXbf7"
#define VCRYPTPP_VICE_KNOWN_CODES_THE_SHIP                        "eb3A4m79"
#define VCRYPTPP_VICE_KNOWN_CODES_ZOMBIE_PANIC_SOURCE             "5R0ni0pZ"

#define VCRYPTPP_VICE_KNOWN_CODES_EKV_GPU_DEFAULT                 "X8bU2qll"
#define VCRYPTPP_VICE_KNOWN_CODES_EKV_GPU_ALIEN_SWARM             "sW9.JupP"
#define VCRYPTPP_VICE_KNOWN_CODES_EKV_GPU_LEFT_4_DEAD_1           "zp14Hi(]"
#define VCRYPTPP_VICE_KNOWN_CODES_EKV_GPU_LEFT_4_DEAD_2           EKV_GPU_LEFT_4_DEAD_1
#define VCRYPTPP_VICE_KNOWN_CODES_EKV_GPU_PORTAL_2                "UrE66!Ap"

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code);
