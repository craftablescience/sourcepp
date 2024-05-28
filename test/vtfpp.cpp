#include <gtest/gtest.h>

#include <vtfpp/vtfpp.h>

#include "Helpers.h"

using namespace vtfpp;

TEST(vtfpp, v75_read_vtf) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v75.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTFFlag::NONE);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
	EXPECT_EQ(vtf.getSliceCount(), 1);

	// Resources
	const auto& resources = vtf.getResources();
	ASSERT_EQ(resources.size(), 4);
	EXPECT_EQ(resources[0].type, ResourceType::THUMBNAIL_DATA);
	EXPECT_EQ(resources[0].flags, ResourceFlag::NONE);
	EXPECT_EQ(resources[0].data.size(), ImageFormatDetails::dataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));
	EXPECT_EQ(resources[1].type, ResourceType::IMAGE_DATA);
	EXPECT_EQ(resources[1].flags, ResourceFlag::NONE);
	// todo: dataLength that accepts faces/frames/mips
	//EXPECT_EQ(resources[1].data.size(), ImageFormatDetails::dataLength(vtf.getFormat(), vtf.getWidth(), vtf.getHeight()));
	EXPECT_EQ(resources[2].type, ResourceType::LOD_CONTROL_INFO);
	EXPECT_EQ(resources[2].flags, ResourceFlag::NO_DATA);
	EXPECT_EQ(resources[3].type, ResourceType::KEYVALUES_DATA);
	EXPECT_EQ(resources[3].flags, ResourceFlag::NONE);
}
