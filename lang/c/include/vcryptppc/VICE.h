#pragma once

#include <sourceppc/Buffer.h>

SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_default,                         "x9Ke0BY7");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_bloody_good_time,                "K4PeJwL7");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_contagion_weapons,               "fUk0fF69");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_contagion_scripts,               "5!rrFz6p");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_counter_strike_source,           "d7NSuLq2");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_counter_strike_global_offensive, "d7NSuLq2");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_counter_strike_2,                "d7NSuLq2");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_counter_strike_promod,           "H1aRQ0n1");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_day_of_defeat_source,            "Wl0u5B3F");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_dystopia_1_2,                    "pH3apO8w");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_dystopia_1_3,                    "G8stUh3F");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_golden_eye_source,               "Gr3naDes");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_half_life_2_ctf,                 "R1dj3axP");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_half_life_2_dm,                  "x9Ke0BY7");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_insurgency,                      "DrA5e3EB");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_left_4_dead_2,                   "SDhfi878");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_no_more_room_in_hell,            "lREeeapA");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_nuclear_dawn,                    "TA+*veh9");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_tactical_intervention,           "71B4Dt1Z");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_team_fortress_2,                 "E2NcUkG2");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_team_fortress_2_items,           "A5fSXbf7");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_the_ship,                        "eb3A4m79");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_zombie_panic_source,             "5R0ni0pZ");

SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_ekv_gpu_default,                 "X8bU2qll");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_ekv_gpu_alien_swarm,             "sW9.JupP");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_ekv_gpu_left_4_dead_1,           "zp14Hi(]");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_ekv_gpu_left_4_dead_2,           "zp14Hi(]");
SOURCEPP_CLSVAR_EXT(vcryptpp, vice, const char*, known_codes_ekv_gpu_portal_2,                "UrE66!Ap");

SOURCEPP_STATIC(vcryptpp, vice, sourcepp_buffer_t, encrypt, const unsigned char* buffer, size_t bufferLen, const char* code); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_STATIC(vcryptpp, vice, sourcepp_buffer_t, decrypt, const unsigned char* buffer, size_t bufferLen, const char* code); // REQUIRES MANUAL FREE: sourcepp_buffer_free
