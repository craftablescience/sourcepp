#include <gtest/gtest.h>

#include <vtfpp/vtfpp.h>

#include "Helpers.h"

using namespace vtfpp;

TEST(vtfpp, read_fmt_rgb888) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/fmt_rgb888.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::RGB888);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));

#if 0
	// Write PNG
	auto data = vtf.convertImageDataToFile();
#endif
}

TEST(vtfpp, read_v70) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v70.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v70_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v70_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v70_nothumb) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v70_nothumb.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v70_nothumb_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v70_nothumb_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v71) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v71.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v71_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v71_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v71_nothumb) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v71_nothumb.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v71_nothumb_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v71_nothumb_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v72) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v72.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v72_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v72_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v72_nothumb) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v72_nothumb.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v72_nothumb_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v72_nothumb_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v75) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v75.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 4);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));

	const auto* lodControlInfo = vtf.getResource(Resource::TYPE_LOD_CONTROL_INFO);
	ASSERT_TRUE(lodControlInfo);
	EXPECT_EQ(lodControlInfo->flags, Resource::FLAG_NO_DATA);
	auto lodControlInfoData = lodControlInfo->getDataAsLODControlInfo();
	EXPECT_EQ(lodControlInfoData.first, 31);
	EXPECT_EQ(lodControlInfoData.second, 31);

	const auto* keyValues = vtf.getResource(Resource::TYPE_KEYVALUES_DATA);
	ASSERT_TRUE(keyValues);
	EXPECT_EQ(keyValues->flags, Resource::FLAG_NONE);
	auto keyValuesData = keyValues->getDataAsKeyValuesData();
	EXPECT_STREQ(keyValuesData.data(), "\"Information\"\r\n{\r\n\t\"Author\" \"craftablescience\"\r\n\t\"Contact\" \"contact\"\r\n\t\"Version\" \"version\"\r\n\t\"Modification\" \"modification\"\r\n\t\"Description\" \"description\"\r\n\t\"Comments\" \"comments\"\r\n}\r\n");
}

TEST(vtfpp, read_v75_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v75_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v75_nothumb) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v75_nothumb.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NONE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v75_nothumb_nomip) {
	VTF vtf{::readFileToBuffer(ASSET_ROOT "vtfpp/v75_nothumb_nomip.vtf"), {}};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getMajorVersion(), 7);
	EXPECT_EQ(vtf.getMinorVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().x, 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().y, 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity().z, 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_FALSE(thumbnail);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}
