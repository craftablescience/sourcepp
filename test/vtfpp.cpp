#include <gtest/gtest.h>

#include <sourcepp/FS.h>
#include <vtfpp/vtfpp.h>

using namespace sourcepp;
using namespace vtfpp;

TEST(vtfpp, read_write_ppl) {
	auto in = fs::readFileBuffer(ASSET_ROOT "vtfpp/test.ppl");

	PPL reader{in};
	EXPECT_EQ(reader.getVersion(), 0);
	EXPECT_EQ(reader.getModelChecksum(), 0xa9230a52);
	EXPECT_EQ(reader.getFormat(), ImageFormat::RGB888);

	const auto* image = reader.getImageRaw();
	ASSERT_TRUE(reader);

	PPL writer{reader.getModelChecksum(), reader.getFormat(), reader.getVersion()};
	writer.setImage(image->data, reader.getFormat(), image->width, image->height);

	auto out = writer.bake();

	ASSERT_EQ(in.size(), out.size());
	for (int i = 0; i < in.size(); i++) {
		EXPECT_EQ(in[i], out[i]);
	}
}

TEST(vtfpp, read_write_ttx) {
	std::pair<std::vector<std::byte>, std::vector<std::byte>> writtenData;
	{
		TTX ttx{ASSET_ROOT "vtfpp/ttx/gloflra.tth", ASSET_ROOT "vtfpp/ttx/gloflra.ttz"};
		ASSERT_TRUE(ttx);

		// Header
		EXPECT_EQ(ttx.getMajorVersion(), 1);
		EXPECT_EQ(ttx.getMinorVersion(), 0);
		ASSERT_EQ(ttx.getMipFlags().size(), 11);
		EXPECT_EQ(ttx.getAspectRatioType(), 3);
		EXPECT_EQ(ttx.getMipFlags()[0], 192);
		EXPECT_EQ(ttx.getMipFlags()[1], 200);
		EXPECT_EQ(ttx.getMipFlags()[2], 208);
		EXPECT_EQ(ttx.getMipFlags()[3], 216);
		EXPECT_EQ(ttx.getMipFlags()[4], 188978561272);
		EXPECT_EQ(ttx.getMipFlags()[5], 734439407992);
		EXPECT_EQ(ttx.getMipFlags()[6], 2740189135736);
		EXPECT_EQ(ttx.getMipFlags()[7], 9620726745976);
		EXPECT_EQ(ttx.getMipFlags()[8], 36378373008248);
		EXPECT_EQ(ttx.getMipFlags()[9], 145384643013496);
		EXPECT_EQ(ttx.getMipFlags()[10], 577557137369976);

		// VTF
		const auto& vtf = ttx.getVTF();
		EXPECT_EQ(vtf.getVersion(), 1);
		EXPECT_EQ(vtf.getWidth(), 1024);
		EXPECT_EQ(vtf.getHeight(), 1024);
		EXPECT_EQ(vtf.getFlags(), 0);
		EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
		EXPECT_EQ(vtf.getMipCount(), 11);
		EXPECT_EQ(vtf.getFrameCount(), 1);
		EXPECT_EQ(vtf.getFaceCount(), 1);
		EXPECT_EQ(vtf.getSliceCount(), 1);
		EXPECT_EQ(vtf.getStartFrame(), 0);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.20940751f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.21449225f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.18495138f);
		EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
		EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
		EXPECT_EQ(vtf.getThumbnailWidth(), 16);
		EXPECT_EQ(vtf.getThumbnailHeight(), 16);

		writtenData = ttx.bake();
	}
	{
		TTX ttx{writtenData.first, writtenData.second};
		ASSERT_TRUE(ttx);

		// Header
		EXPECT_EQ(ttx.getMajorVersion(), 1);
		EXPECT_EQ(ttx.getMinorVersion(), 0);
		ASSERT_EQ(ttx.getMipFlags().size(), 11);
		EXPECT_EQ(ttx.getAspectRatioType(), 3);
		EXPECT_EQ(ttx.getMipFlags()[0], 192);
		EXPECT_EQ(ttx.getMipFlags()[1], 200);
		EXPECT_EQ(ttx.getMipFlags()[2], 208);
		EXPECT_EQ(ttx.getMipFlags()[3], 216);
		EXPECT_EQ(ttx.getMipFlags()[4], 188978561272);
		EXPECT_EQ(ttx.getMipFlags()[5], 734439407992);
		EXPECT_EQ(ttx.getMipFlags()[6], 2740189135736);
		EXPECT_EQ(ttx.getMipFlags()[7], 9620726745976);
		EXPECT_EQ(ttx.getMipFlags()[8], 36378373008248);
		EXPECT_EQ(ttx.getMipFlags()[9], 145384643013496);
		EXPECT_EQ(ttx.getMipFlags()[10], 577557137369976);

		// VTF
		const auto& vtf = ttx.getVTF();
		EXPECT_EQ(vtf.getVersion(), 1);
		EXPECT_EQ(vtf.getWidth(), 1024);
		EXPECT_EQ(vtf.getHeight(), 1024);
		EXPECT_EQ(vtf.getFlags(), 0);
		EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
		EXPECT_EQ(vtf.getMipCount(), 11);
		EXPECT_EQ(vtf.getFrameCount(), 1);
		EXPECT_EQ(vtf.getFaceCount(), 1);
		EXPECT_EQ(vtf.getSliceCount(), 1);
		EXPECT_EQ(vtf.getStartFrame(), 0);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.20940751f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.21449225f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.18495138f);
		EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
		EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
		EXPECT_EQ(vtf.getThumbnailWidth(), 16);
		EXPECT_EQ(vtf.getThumbnailHeight(), 16);
	}
}

TEST(vtfpp, read_ttx_no_ttz) {
	TTX ttx{ASSET_ROOT "vtfpp/ttx/cablenormalmap.tth"};
	ASSERT_TRUE(ttx);

	// Header
	EXPECT_EQ(ttx.getMajorVersion(), 1);
	EXPECT_EQ(ttx.getMinorVersion(), 0);
	ASSERT_EQ(ttx.getMipFlags().size(), 5);
	EXPECT_EQ(ttx.getAspectRatioType(), 5);
	EXPECT_EQ(ttx.getMipFlags()[0], 96);
	EXPECT_EQ(ttx.getMipFlags()[1], 99);
	EXPECT_EQ(ttx.getMipFlags()[2], 105);
	EXPECT_EQ(ttx.getMipFlags()[3], 117);
	EXPECT_EQ(ttx.getMipFlags()[4], 165);

	// VTF
	const auto& vtf = ttx.getVTF();
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 16);
	EXPECT_EQ(vtf.getHeight(), 4);
	EXPECT_EQ(vtf.getFlags(), 196); // VTF flags are probably different on this engine branch
	EXPECT_EQ(vtf.getFormat(), ImageFormat::BGR888);
	EXPECT_EQ(vtf.getMipCount(), 5);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), static_cast<uint16_t>(-1));
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.32940885f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.21175662f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.74329108f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 4);
}

#ifdef SOURCEPP_BUILD_TESTS_EXTRA

#define TEST_WRITE_FMT(Format, Flags) \
		TEST(vtfpp, write_fmt_##Format) { \
			VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/fmt/" #Format ".vtf")}; \
			ASSERT_TRUE(vtf); \
			EXPECT_EQ(vtf.getWidth(), 256); \
			EXPECT_EQ(vtf.getHeight(), 256); \
			EXPECT_EQ(vtf.getFlags(), (Flags)); \
			EXPECT_EQ(vtf.getFormat(), ImageFormat::Format); \
			const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA); \
			ASSERT_TRUE(image); \
			EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount())); \
			fs::writeFileBuffer("fmt_" #Format ".png", vtf.saveImageToFile(0, 0, 0, 0, ImageConversion::FileFormat::PNG)); \
		}

TEST_WRITE_FMT(RGBA8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(ABGR8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(RGB888,             0)
TEST_WRITE_FMT(BGR888,             VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(RGB565,             VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(I8,                 VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(IA88,               VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(A8,                 VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(RGB888_BLUESCREEN,  VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(BGR888_BLUESCREEN,  VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(ARGB8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(BGRA8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(DXT1,               VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(DXT3,               VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(DXT5,               VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(BGRX8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(BGR565,             VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(BGRX5551,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(BGRA4444,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(DXT1_ONE_BIT_ALPHA, VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_ONE_BIT_ALPHA)
TEST_WRITE_FMT(BGRA5551,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_ONE_BIT_ALPHA)
TEST_WRITE_FMT(UV88,               VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD)
TEST_WRITE_FMT(UVWQ8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)
TEST_WRITE_FMT(UVLX8888,           VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA)

#endif

TEST(vtfpp, write_non_po2) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/non_po2.png", {
		.widthResizeMethod = ImageConversion::ResizeMethod::NONE,
		.heightResizeMethod = ImageConversion::ResizeMethod::NONE,
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 4);
	EXPECT_EQ(vtf.getWidth(), 136);
	EXPECT_EQ(vtf.getHeight(), 148);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 2);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.10727221f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.06730457f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.38124585f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
}

TEST(vtfpp, read_v70) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v70.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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

TEST(vtfpp, write_v70) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/po2.png", {
		.version = 0,
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 7);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.76578784f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.67778563f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.56479901f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
}

TEST(vtfpp, read_v70_nomip) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v70_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v70_nothumb.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v70_nothumb_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 0);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v71.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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

TEST(vtfpp, write_v71) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/po2.png", {
		.version = 1,
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 7);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.76578784f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.67778563f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.56479901f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
}

TEST(vtfpp, read_v71_nomip) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v71_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v71_nothumb.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v71_nothumb_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 1);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v72.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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

TEST(vtfpp, write_v72) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/po2.png", {
		.version = 2,
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 7);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.76578784f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.67778563f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.56479901f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
}

TEST(vtfpp, read_v72_nomip) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v72_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v72_nothumb.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v72_nothumb_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v75.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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
	EXPECT_EQ(lodControlInfo->flags, Resource::FLAG_LOCAL_DATA);
	auto lodControlInfoData = lodControlInfo->getDataAsLODControlInfo();
	EXPECT_EQ(std::get<0>(lodControlInfoData), 31);
	EXPECT_EQ(std::get<1>(lodControlInfoData), 31);
	EXPECT_EQ(std::get<2>(lodControlInfoData), 0);
	EXPECT_EQ(std::get<3>(lodControlInfoData), 0);

	const auto* keyValues = vtf.getResource(Resource::TYPE_KEYVALUES_DATA);
	ASSERT_TRUE(keyValues);
	EXPECT_EQ(keyValues->flags, Resource::FLAG_NONE);
	auto keyValuesData = keyValues->getDataAsKeyValuesData();
	EXPECT_STREQ(keyValuesData.c_str(), "\"Information\"\r\n{\r\n\t\"Author\" \"craftablescience\"\r\n\t\"Contact\" \"contact\"\r\n\t\"Version\" \"version\"\r\n\t\"Modification\" \"modification\"\r\n\t\"Description\" \"description\"\r\n\t\"Comments\" \"comments\"\r\n}\r\n");
}

TEST(vtfpp, write_v75) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/po2.png", {
		.version = 5,
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 7);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.76578784f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.67778563f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.56479901f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
}

TEST(vtfpp, read_v75_nomip) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v75_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v75_nothumb.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), 0);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v75_nothumb_nomip.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 5);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
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

TEST(vtfpp, read_xbox) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/xbox/dxt1.xtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_XBOX);
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), /*NICE filtered*/ (1 << 24) | VTF::FLAG_XBOX_CACHEABLE);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 9);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.27086672f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.26922473f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.24819961f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::RGB888);
	EXPECT_EQ(vtf.getThumbnailWidth(), 1);
	EXPECT_EQ(vtf.getThumbnailHeight(), 1);
	EXPECT_EQ(vtf.getFallbackWidth(), 8);
	EXPECT_EQ(vtf.getFallbackHeight(), 8);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 3);

	const auto* thumbnail = vtf.getResource(Resource::TYPE_THUMBNAIL_DATA);
	ASSERT_TRUE(thumbnail);
	EXPECT_EQ(thumbnail->flags, Resource::FLAG_NONE);
	EXPECT_EQ(thumbnail->data.size(), ImageFormatDetails::getDataLength(vtf.getThumbnailFormat(), vtf.getThumbnailWidth(), vtf.getThumbnailHeight()));

	const auto* fallback = vtf.getResource(Resource::TYPE_FALLBACK_DATA);
	ASSERT_TRUE(fallback);
	EXPECT_EQ(fallback->flags, Resource::FLAG_NONE);
	EXPECT_EQ(fallback->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getFallbackMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getFallbackWidth(), vtf.getFallbackHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_xbox_envmap) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/xbox/envmap.xtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_XBOX);
	EXPECT_EQ(vtf.getVersion(), 2);
	EXPECT_EQ(vtf.getWidth(), 32);
	EXPECT_EQ(vtf.getHeight(), 32);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_CLAMP_S | VTF::FLAG_CLAMP_T | VTF::FLAG_NO_MIP | VTF::FLAG_ENVMAP | static_cast<VTF::FlagsV0>(VTF::FLAG_XBOX_CACHEABLE));
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(),	1);
	EXPECT_EQ(vtf.getFaceCount(), 6);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.034028269f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.020980936f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.013155934f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);
	EXPECT_EQ(vtf.getFallbackWidth(), 8);
	EXPECT_EQ(vtf.getFallbackHeight(), 8);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 2);

	const auto* fallback = vtf.getResource(Resource::TYPE_FALLBACK_DATA);
	ASSERT_TRUE(fallback);
	EXPECT_EQ(fallback->flags, Resource::FLAG_NONE);
	EXPECT_EQ(fallback->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getFallbackMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getFallbackWidth(), vtf.getFallbackHeight()));

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_ps3_orangebox) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ps3_orangebox/portal.ps3.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_PS3_ORANGEBOX);
	EXPECT_EQ(vtf.getVersion(), 4);
	EXPECT_EQ(vtf.getWidth(), 1024);
	EXPECT_EQ(vtf.getHeight(), 1024);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.037193343f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.020529008f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.016482241f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_ps3_orangebox_swizzled) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ps3_orangebox/bgra8888.ps3.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_PS3_ORANGEBOX);
	EXPECT_EQ(vtf.getVersion(), 4);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::BGRA8888);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.25142393f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.24672441f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.24161555f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_ps3_portal2) {
	{
		VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ps3_portal2/elevator_screen_colour.ps3.vtf")};
		ASSERT_TRUE(vtf);

		// Header
		EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_PS3_PORTAL2);
		EXPECT_EQ(vtf.getVersion(), 5);
		EXPECT_EQ(vtf.getWidth(), 256);
		EXPECT_EQ(vtf.getHeight(), 512);
		EXPECT_EQ(vtf.getFlags(), 0);
		EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1);
		EXPECT_EQ(vtf.getMipCount(), 10);
		EXPECT_EQ(vtf.getFrameCount(), 1);
		EXPECT_EQ(vtf.getFaceCount(), 1);
		EXPECT_EQ(vtf.getSliceCount(), 1);
		EXPECT_EQ(vtf.getStartFrame(), 0);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.050660271f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.050561361f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.048385158f);
		EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
		EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
		EXPECT_EQ(vtf.getThumbnailWidth(), 0);
		EXPECT_EQ(vtf.getThumbnailHeight(), 0);

		// Resources
		EXPECT_EQ(vtf.getResources().size(), 1);

		const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
		ASSERT_TRUE(image);
		EXPECT_EQ(image->flags, Resource::FLAG_NONE);
		EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
	}
	{
		VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ps3_portal2/elevator_screen_normal.ps3.vtf")};
		ASSERT_TRUE(vtf);

		// Header
		EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_PS3_PORTAL2);
		EXPECT_EQ(vtf.getVersion(), 5);
		EXPECT_EQ(vtf.getWidth(), 256);
		EXPECT_EQ(vtf.getHeight(), 512);
		EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NORMAL | VTF::FLAG_MULTI_BIT_ALPHA);
		EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT5);
		EXPECT_EQ(vtf.getMipCount(), 10);
		EXPECT_EQ(vtf.getFrameCount(), 1);
		EXPECT_EQ(vtf.getFaceCount(), 1);
		EXPECT_EQ(vtf.getSliceCount(), 1);
		EXPECT_EQ(vtf.getStartFrame(), 0);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.42064965f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.45285019f);
		EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.78605181f);
		EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
		EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
		EXPECT_EQ(vtf.getThumbnailWidth(), 0);
		EXPECT_EQ(vtf.getThumbnailHeight(), 0);

		// Resources
		EXPECT_EQ(vtf.getResources().size(), 1);

		const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
		ASSERT_TRUE(image);
		EXPECT_EQ(image->flags, Resource::FLAG_NONE);
		EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
	}
}

TEST(vtfpp, read_x360) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/x360/metalwall048b.360.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getPlatform(), VTF::PLATFORM_X360);
	EXPECT_EQ(vtf.getVersion(), 4);
	EXPECT_EQ(vtf.getWidth(), 512);
	EXPECT_EQ(vtf.getHeight(), 512);
	EXPECT_EQ(vtf.getFlags(), static_cast<uint32_t>(VTF::FLAG_V5_PWL_CORRECTED) | VTF::FLAG_MULTI_BIT_ALPHA);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT5);
	EXPECT_EQ(vtf.getMipCount(), 10);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.0389036387f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.0300185774f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.0235184804f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::EMPTY);
	EXPECT_EQ(vtf.getThumbnailWidth(), 0);
	EXPECT_EQ(vtf.getThumbnailHeight(), 0);

	// Resources
	EXPECT_EQ(vtf.getResources().size(), 1);

	const auto* image = vtf.getResource(Resource::TYPE_IMAGE_DATA);
	ASSERT_TRUE(image);
	EXPECT_EQ(image->flags, Resource::FLAG_NONE);
	EXPECT_EQ(image->data.size(), ImageFormatDetails::getDataLength(vtf.getFormat(), vtf.getMipCount(), vtf.getFrameCount(), vtf.getFaceCount(), vtf.getWidth(), vtf.getHeight(), vtf.getSliceCount()));
}

TEST(vtfpp, read_v76_c9) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v76_c9.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 6);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_MULTI_BIT_ALPHA);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1_ONE_BIT_ALPHA);
	EXPECT_EQ(vtf.getMipCount(), 10);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
	EXPECT_EQ(vtf.getCompressionLevel(), 9);
	EXPECT_EQ(vtf.getCompressionMethod(), CompressionMethod::DEFLATE);

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

TEST(vtfpp, write_v76_c6) {
	VTF vtf = VTF::create(ASSET_ROOT "vtfpp/src/po2.png", {
		.version = 6,
		.compressedFormatQuality = 0.f, // save time in testing
	});
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 6);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_MULTI_BIT_ALPHA);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::BC7);
	EXPECT_EQ(vtf.getMipCount(), 7);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.76578784f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.67778563f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.56479901f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
	EXPECT_EQ(vtf.getCompressionLevel(), -1);
	EXPECT_EQ(vtf.getCompressionMethod(), CompressionMethod::ZSTD);
}

TEST(vtfpp, read_v76_nomip_c9) {
	VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/ver/v76_nomip_c9.vtf")};
	ASSERT_TRUE(vtf);

	// Header
	EXPECT_EQ(vtf.getVersion(), 6);
	EXPECT_EQ(vtf.getWidth(), 256);
	EXPECT_EQ(vtf.getHeight(), 256);
	EXPECT_EQ(vtf.getFlags(), VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD | VTF::FLAG_MULTI_BIT_ALPHA);
	EXPECT_EQ(vtf.getFormat(), ImageFormat::DXT1_ONE_BIT_ALPHA);
	EXPECT_EQ(vtf.getMipCount(), 1);
	EXPECT_EQ(vtf.getFrameCount(), 1);
	EXPECT_EQ(vtf.getFaceCount(), 1);
	EXPECT_EQ(vtf.getSliceCount(), 1);
	EXPECT_EQ(vtf.getStartFrame(), 0);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[0], 0.14816631f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[1], 0.03145336f);
	EXPECT_FLOAT_EQ(vtf.getReflectivity()[2], 0.080934197f);
	EXPECT_FLOAT_EQ(vtf.getBumpMapScale(), 1.f);
	EXPECT_EQ(vtf.getThumbnailFormat(), ImageFormat::DXT1);
	EXPECT_EQ(vtf.getThumbnailWidth(), 16);
	EXPECT_EQ(vtf.getThumbnailHeight(), 16);
	EXPECT_EQ(vtf.getCompressionLevel(), 9);
	EXPECT_EQ(vtf.getCompressionMethod(), CompressionMethod::DEFLATE);

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
