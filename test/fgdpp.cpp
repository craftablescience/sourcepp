#include <gtest/gtest.h>

import fgdpp;

using namespace fgdpp;

TEST(fgdpp, parseEmpty) {
	FGD fgd{ASSET_ROOT "fgdpp/empty.fgd"};
	EXPECT_EQ(fgd.getVersion(), 0);
	EXPECT_EQ(fgd.getMapSize().x, 0);
	EXPECT_EQ(fgd.getMapSize().y, 0);
	EXPECT_TRUE(fgd.getEntities().empty());
	EXPECT_TRUE(fgd.getMaterialExclusionDirs().empty());
	EXPECT_TRUE(fgd.getAutoVisGroups().empty());
}

TEST(fgdpp, parseIdeal) {
	FGD fgd{ASSET_ROOT "fgdpp/ideal.fgd"};
	EXPECT_EQ(fgd.getVersion(), 8);
	EXPECT_EQ(fgd.getMapSize().x, -16384);
	EXPECT_EQ(fgd.getMapSize().y, 16384);
	EXPECT_EQ(fgd.getEntities().size(), 1);
	EXPECT_EQ(fgd.getMaterialExclusionDirs().size(), 9);
	EXPECT_EQ(fgd.getAutoVisGroups().size(), 3);
}

TEST(fgdpp, parsePortal2) {
    FGD fgd{ASSET_ROOT "fgdpp/game/portal2.fgd"};
	EXPECT_EQ(fgd.getVersion(), 0);
	EXPECT_EQ(fgd.getMapSize().x, -16384);
	EXPECT_EQ(fgd.getMapSize().y, 16384);
	EXPECT_EQ(fgd.getEntities().size(), 494);
	EXPECT_TRUE(fgd.getMaterialExclusionDirs().empty());
	EXPECT_TRUE(fgd.getAutoVisGroups().empty());
}
