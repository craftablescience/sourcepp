#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#define VICEPP_KNOWN_CODES_DEFAULT           "x9Ke0BY7"
#define VICEPP_KNOWN_CODES_HL2DM             VICEPP_KNOWN_CODES_DEFAULT
#define VICEPP_KNOWN_CODES_CSS               "d7NSuLq2"
#define VICEPP_KNOWN_CODES_CSGO              VICEPP_KNOWN_CODES_CSS
#define VICEPP_KNOWN_CODES_DODS              "Wl0u5B3F"
#define VICEPP_KNOWN_CODES_TF2               "E2NcUkG2"
#define VICEPP_KNOWN_CODES_TF2_ITEMS         "A5fSXbf7"
#define VICEPP_KNOWN_CODES_L4D2              "SDhfi878"
#define VICEPP_KNOWN_CODES_THE_SHIP          "eb3A4m79"
#define VICEPP_KNOWN_CODES_CONTAGION_WEAPONS "fUk0fF69"
#define VICEPP_KNOWN_CODES_CONTAGION_SCRIPTS "5!rrFz6p"
#define VICEPP_KNOWN_CODES_ZPS               "5R0ni0pZ"

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vicepp_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vicepp_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code);
