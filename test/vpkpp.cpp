#include <gtest/gtest.h>

#include <vpkpp/vpkpp.h>

#include <sourcepp/parser/Text.h>

#include "BufferStream.h"

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

TEST(vpkpp, pkg_read) {
	const auto pkg = PackFile::open("/home/lxlewis/.local/share/Steam/steamapps/common/PortalRTX/rtx-remix/mods/gameReadyAssets/mod_00.pkg");
	ASSERT_TRUE(pkg);
	VPKPP_PRINT_ALL_PATHS(pkg);

	//ASSERT_TRUE(pkg->hasEntry("SubUSDs/textures/0901F01E92F1E81C.dds"));
	//const auto data = pkg->readEntry("SubUSDs/textures/0901F01E92F1E81C.dds");
	//ASSERT_TRUE(data);
	//fs::writeFileBuffer("/home/lxlewis/.local/share/Steam/steamapps/common/PortalRTX/rtx-remix/mods/gameReadyAssets/mod_spp/SubUSDs/textures/0901F01E92F1E81C.dds", *data);

	ASSERT_TRUE(pkg->hasEntry("SubUSDs/textures/T_Fixture_Platform_Folding_A1_Albedo.dds"));
	const auto data2 = pkg->readEntry("SubUSDs/textures/T_Fixture_Platform_Folding_A1_Albedo.dds");
	ASSERT_TRUE(data2);
	fs::writeFileBuffer("/home/lxlewis/.local/share/Steam/steamapps/common/PortalRTX/rtx-remix/mods/gameReadyAssets/mod_spp/SubUSDs/textures/T_Fixture_Platform_Folding_A1_Albedo.dds", *data2);

	//ASSERT_TRUE(pkg->extractAll("/home/lxlewis/.local/share/Steam/steamapps/common/PortalRTX/rtx-remix/mods/gameReadyAssets/"));
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
