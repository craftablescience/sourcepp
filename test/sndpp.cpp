#include <gtest/gtest.h>

#include <sndpp/sndpp.h>

using namespace sndpp;

TEST(sndpp, read_wav) {
	WAV wav{ASSET_ROOT "sndpp/biohazard_detected.wav"};
	ASSERT_TRUE(wav);
	EXPECT_EQ(wav.getSignature(), RIFF_SIGNATURE);
	EXPECT_EQ(wav.getChunks().size(), 3);

	const auto fmt = wav.getFirstWAVChunk<WAV::CHUNK_FMT>();
	ASSERT_TRUE(fmt);
	EXPECT_EQ(fmt->format, 1);
	EXPECT_EQ(fmt->channels, 1);
	EXPECT_EQ(fmt->samplesPerSecond, 11025);
	EXPECT_EQ(fmt->averageBytesPerSecond, 11025);
	EXPECT_EQ(fmt->blockAlign, 1);
	EXPECT_EQ(fmt->bitsPerSample, 8);
	EXPECT_EQ(fmt->extraCompressionInfo.size(), 2);

	const auto data = wav.getFirstWAVChunk<WAV::CHUNK_DATA>();
	ASSERT_TRUE(data);
	EXPECT_EQ(data->size(), 34061);
}

TEST(sndpp, read_xwv_v0) {
	XWV xwv{ASSET_ROOT "sndpp/biohazard_detected.xbox.wav"};
	ASSERT_TRUE(xwv);
	EXPECT_EQ(xwv.getVersion(), XWV::Version::V0);
	EXPECT_EQ(xwv.getAudioDataRaw().size(), 19152);
	EXPECT_EQ(xwv.getStaticData().size(), 0);
	EXPECT_EQ(xwv.getValveData().size(), 0);
	EXPECT_EQ(xwv.getSeekTableData().size(), 0);
	EXPECT_EQ(xwv.getDecodedSampleCount(), 34048);
	EXPECT_EQ(xwv.getLoopStart(), -1);
	EXPECT_EQ(xwv.getLoopBlock(), 0);
	EXPECT_EQ(xwv.getLeadingSampleCount(), 0);
	EXPECT_EQ(xwv.getTrailingSampleCount(), 0);
	EXPECT_EQ(xwv.getFormat(), XWV::Format::XMA);
	EXPECT_EQ(xwv.getBitsPerSample(), 4);
	EXPECT_EQ(xwv.getFrequency(), XWV::Frequency::HZ_11025);
	EXPECT_EQ(xwv.getChannelCount(), 1);
	EXPECT_EQ(xwv.getQuality(), 63);
}

TEST(sndpp, read_xwv_x360) {
	XWV xwv{ASSET_ROOT "sndpp/biohazard_detected.360.wav"};
	ASSERT_TRUE(xwv);
	EXPECT_EQ(xwv.getVersion(), XWV::Version::V4);
	EXPECT_EQ(xwv.getAudioDataRaw().size(), 16384);
	EXPECT_EQ(xwv.getStaticData().size(), 0);
	EXPECT_EQ(xwv.getValveData().size(), 0);
	EXPECT_EQ(xwv.getSeekTableData().size(), 0);
	EXPECT_EQ(xwv.getDecodedSampleCount(), 34816);
	EXPECT_EQ(xwv.getLoopStart(), -1);
	EXPECT_EQ(xwv.getLoopBlock(), 0);
	EXPECT_EQ(xwv.getLeadingSampleCount(), 0);
	EXPECT_EQ(xwv.getTrailingSampleCount(), 0);
	EXPECT_EQ(xwv.getFormat(), XWV::Format::XMA2);
	EXPECT_EQ(xwv.getBitsPerSample(), 16);
	EXPECT_EQ(xwv.getFrequency(), XWV::Frequency::HZ_11025);
	EXPECT_EQ(xwv.getChannelCount(), 1);
	EXPECT_EQ(xwv.getQuality(), 75);
}
