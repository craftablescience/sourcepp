#include <gtest/gtest.h>

#include <dmxpp/dmxpp.h>

#include "Helpers.h"

using namespace dmxpp;

// v1 and v2 are identical afaik...
TEST(dmxpp, v2_read) {
	DMX dmx{::readFile(ASSET_ROOT "dmxpp/binary/v2.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v3_read) {
	DMX dmx{::readFile(ASSET_ROOT "dmxpp/binary/v3.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v4_read) {
	DMX dmx{::readFile(ASSET_ROOT "dmxpp/binary/v4.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v5_read) {
	DMX dmx{::readFile(ASSET_ROOT "dmxpp/binary/v5.dmx")};
	ASSERT_TRUE(dmx);
}
