#include <gtest/gtest.h>

#include <vpkpp/vpkpp.h>

using namespace sourcepp;
using namespace vpkpp;

TEST(vpkpp, hog_read) {
	const auto hog = PackFile::open(ASSET_ROOT "vpkpp/hog/chaos.hog");
	ASSERT_TRUE(hog);
	EXPECT_EQ(hog->getEntryCount(), 5);
}

TEST(vpkpp, vpp_v1_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v1.vpp");
	ASSERT_TRUE(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 29);
}

TEST(vpkpp, vpp_v2_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v2.vpp");
	ASSERT_TRUE(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 32);
}

TEST(vpkpp, vpp_v3_lil_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v3.vpp_pc");
	ASSERT_TRUE(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 128);
	const auto data = vpp->readEntry("ACTIVE.rfgvpx.str2_pc");
	ASSERT_TRUE(data);
}

TEST(vpkpp, vpp_v3_big_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v3.vpp_xbox2");
	ASSERT_TRUE(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 20);
	const auto data = vpp->readEntry("ai_debug.txt");
	ASSERT_TRUE(data);
}
