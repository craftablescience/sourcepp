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
	const auto* thumbnail = vtf.getResource(ResourceType::THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, ResourceFlag::NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::dataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(ResourceType::IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, ResourceFlag::NONE);
	// todo: dataLength that accepts faces/frames/mips
	//EXPECT_EQ(image->data.size(), ImageFormatDetails::dataLength(vtf.getFormat(), vtf.getWidth(), vtf.getHeight()));

	const auto* lodControlInfo = vtf.getResource(ResourceType::LOD_CONTROL_INFO);
	ASSERT_TRUE(lodControlInfo);
	EXPECT_EQ(lodControlInfo->flags, ResourceFlag::NO_DATA);
	auto lodControlInfoData = lodControlInfo->getDataAsLODControlInfo();
	EXPECT_EQ(lodControlInfoData.first, 31);
	EXPECT_EQ(lodControlInfoData.second, 31);

	const auto* keyValues = vtf.getResource(ResourceType::KEYVALUES_DATA);
	ASSERT_TRUE(keyValues);
	EXPECT_EQ(keyValues->flags, ResourceFlag::NONE);
	auto keyValuesData = keyValues->getDataAsKeyValuesData();
	EXPECT_STREQ(keyValuesData.data(), "\"Information\"\r\n{\r\n\t\"Author\" \"craftablescience\"\r\n\t\"Contact\" \"contact\"\r\n\t\"Version\" \"version\"\r\n\t\"Modification\" \"modification\"\r\n\t\"Description\" \"description\"\r\n\t\"Comments\" \"comments\"\r\n}\r\n");
}
