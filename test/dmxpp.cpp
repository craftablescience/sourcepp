#include <gtest/gtest.h>

import dmxpp;
import sourcepp;

using namespace dmxpp;
using namespace sourcepp;

// v1 and v2 are identical afaik...
TEST(dmxpp, v2_read) {
	DMX dmx{fs::readFileBuffer(ASSET_ROOT "dmxpp/binary/v2.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v3_read) {
	DMX dmx{fs::readFileBuffer(ASSET_ROOT "dmxpp/binary/v3.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v4_read) {
	DMX dmx{fs::readFileBuffer(ASSET_ROOT "dmxpp/binary/v4.dmx")};
	ASSERT_TRUE(dmx);
}

TEST(dmxpp, v5_read) {
	DMX dmx{fs::readFileBuffer(ASSET_ROOT "dmxpp/binary/v5.dmx")};
	ASSERT_TRUE(dmx);
}
