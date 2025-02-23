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
