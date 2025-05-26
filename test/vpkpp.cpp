#include <gtest/gtest.h>

#include <vpkpp/vpkpp.h>

using namespace sourcepp;
using namespace vpkpp;

#ifdef SOURCEPP_BUILD_TESTS_EXTRA
	#define VPKPP_PRINT_ALL_PATHS(packfile) packfile->runForAllEntries([](const std::string& path, const Entry&) {std::cout << path << std::endl;})
#else
	#define VPKPP_PRINT_ALL_PATHS(packfile) static_cast<void>(packfile)
#endif

TEST(vpkpp, hog_read) {
	const auto hog = PackFile::open(ASSET_ROOT "vpkpp/hog/chaos.hog");
	ASSERT_TRUE(hog);
	VPKPP_PRINT_ALL_PATHS(hog);
	EXPECT_EQ(hog->getEntryCount(), 5);
	EXPECT_TRUE(hog->hasEntry("chaos1.rdl"));
}

TEST(vpkpp, ore_read) {
	const auto ore = PackFile::open(ASSET_ROOT "vpkpp/ore/file.ore");
	ASSERT_TRUE(ore);
	VPKPP_PRINT_ALL_PATHS(ore);
	EXPECT_EQ(ore->getEntryCount(), 147);
	EXPECT_TRUE(ore->hasEntry("startup.cfg"));
}

TEST(vpkpp, vpp_v1_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v1.vpp");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 29);
	EXPECT_TRUE(vpp->hasEntry("maps.txt"));
}

TEST(vpkpp, vpp_v2_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v2.vpp");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 32);
	EXPECT_TRUE(vpp->hasEntry("credits.tbl"));
}

TEST(vpkpp, vpp_v3_lil_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v3.vpp_pc");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 128);
	EXPECT_TRUE(vpp->hasEntry("address.rfgvpx.str2_pc"));
}

TEST(vpkpp, vpp_v3_big_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v3.vpp_xbox2");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 20);
	EXPECT_TRUE(vpp->hasEntry("pretty.txt"));
}

TEST(vpkpp, vpp_v4_lil_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v4.vpp_pc");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 730);
	EXPECT_TRUE(vpp->hasEntry("sr2_skybox.hmap_pc"));
}

TEST(vpkpp, vpp_v4_big_read) {
	const auto vpp = PackFile::open(ASSET_ROOT "vpkpp/vpp/v4.vpp_xbox2");
	ASSERT_TRUE(vpp);
	VPKPP_PRINT_ALL_PATHS(vpp);
	EXPECT_EQ(vpp->getEntryCount(), 118);
	EXPECT_TRUE(vpp->hasEntry("activity_level.lua"));
}
