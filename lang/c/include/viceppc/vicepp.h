#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#define VICEPP_KNOWN_CODES_DEFAULT "x9Ke0BY7";

#define VICEPP_KNOWN_CODES_CONTAGION_WEAPONS               "fUk0fF69"
#define VICEPP_KNOWN_CODES_CONTAGION_SCRIPTS               "5!rrFz6p"
#define VICEPP_KNOWN_CODES_COUNTER_STRIKE_SOURCE           "d7NSuLq2"
#define VICEPP_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE VICEPP_KNOWN_CODES_COUNTER_STRIKE_SOURCE
#define VICEPP_KNOWN_CODES_COUNTER_STRIKE_2                VICEPP_KNOWN_CODES_COUNTER_STRIKE_GLOBAL_OFFENSIVE
#define VICEPP_KNOWN_CODES_COUNTER_STRIKE_PROMOD           "H1aRQ0n1"
#define VICEPP_KNOWN_CODES_DAY_OF_DEFEAT_SOURCE            "Wl0u5B3F"
#define VICEPP_KNOWN_CODES_DYSTOPIA_1_2                    "pH3apO8w"
#define VICEPP_KNOWN_CODES_DYSTOPIA_1_3                    "G8stUh3F"
#define VICEPP_KNOWN_CODES_GOLDEN_EYE_SOURCE               "Gr3naDes"
#define VICEPP_KNOWN_CODES_HALF_LIFE_2_CTF                 "R1dj3axP"
#define VICEPP_KNOWN_CODES_HALF_LIFE_2_DM                  VICEPP_KNOWN_CODES_DEFAULT
#define VICEPP_KNOWN_CODES_INSURGENCY                      "DrA5e3EB"
#define VICEPP_KNOWN_CODES_LEFT_4_DEAD_2                   "SDhfi878"
#define VICEPP_KNOWN_CODES_NO_MORE_ROOM_IN_HELL            "lREeeapA"
#define VICEPP_KNOWN_CODES_NUCLEAR_DAWN                    "TA+*veh9"
#define VICEPP_KNOWN_CODES_TEAM_FORTRESS_2                 "E2NcUkG2"
#define VICEPP_KNOWN_CODES_TEAM_FORTRESS_2_ITEMS           "A5fSXbf7"
#define VICEPP_KNOWN_CODES_THE_SHIP                        "eb3A4m79"
#define VICEPP_KNOWN_CODES_ZOMBIE_PANIC_SOURCE             "5R0ni0pZ"

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vicepp_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vicepp_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code);
