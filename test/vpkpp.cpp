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

TEST(vpkpp, rez_v1_read) {
	const auto rez = PackFile::open(ASSET_ROOT "vpkpp/rez/v1.rez");
	ASSERT_TRUE(rez);
	VPKPP_PRINT_ALL_PATHS(rez);
	EXPECT_EQ(rez->getEntryCount(), 1);
	EXPECT_TRUE(rez->hasEntry("patch.txt"));
}

TEST(vpkpp, sdat) {
	const auto sdat = PackFile::open(ASSET_ROOT "vpkpp/sdat/steam_resources.sdat");
	ASSERT_TRUE(sdat);
	VPKPP_PRINT_ALL_PATHS(sdat);
	EXPECT_EQ(sdat->getEntryCount(), 90);
	EXPECT_TRUE(sdat->hasEntry("resource/steam_logo.tga"));
}

TEST(vpkpp, vpk) {
	auto vpk = PackFile::open("/home/lxlewis/Downloads/test/modified/hl2_misc_dir.vpk");
	ASSERT_TRUE(vpk);
	VPKPP_PRINT_ALL_PATHS(vpk);
	EXPECT_EQ(vpk->getEntryCount(), 18796);
	EXPECT_TRUE(vpk->hasEntry("cfg/valve.rc"));
	EXPECT_TRUE(vpk->hasPackFileSignature());
	EXPECT_TRUE(vpk->verifyPackFileSignature());

	//VPK::generateKeyPairFiles("/home/lxlewis/Downloads/test/modified/hl2_misc", VPK::SignatureType::LEGACY);
	//dynamic_cast<VPK*>(vpk.get())->sign("/home/lxlewis/Downloads/test/modified/testkey.privatekey.vdf", VPK::SignatureType::LEGACY);
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
