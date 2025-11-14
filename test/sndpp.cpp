#include <gtest/gtest.h>

#include <sndpp/sndpp.h>

using namespace sndpp;

TEST(sndpp, read_wav) {
	WAV wav{ASSET_ROOT "sndpp/cinemasins.wav"};
	ASSERT_TRUE(wav);
	EXPECT_EQ(wav.getSignature(), RIFF_SIGNATURE);
	EXPECT_EQ(wav.getChunks().size(), 9);

	const auto fmt = wav.getFirstWAVChunk<WAV::CHUNK_FMT>();
	ASSERT_TRUE(fmt);
	EXPECT_EQ(fmt->format, 1);
	EXPECT_EQ(fmt->channels, 2);
	EXPECT_EQ(fmt->samplesPerSecond, 96000);
	EXPECT_EQ(fmt->averageBytesPerSecond, 576000);
	EXPECT_EQ(fmt->blockAlign, 6);
	EXPECT_EQ(fmt->bitsPerSample, 24);
	EXPECT_TRUE(fmt->extraCompressionInfo.empty());

	const auto data = wav.getFirstWAVChunk<WAV::CHUNK_DATA>();
	ASSERT_TRUE(data);
	EXPECT_EQ(data->size(), 6156822);
}

TEST(sndpp, read_xwv_x360) {
	XWV xwv{ASSET_ROOT "sndpp/hl1_stinger_song27.360.wav"};
	ASSERT_TRUE(xwv);
	EXPECT_EQ(xwv.getPlatform(), XWV::Platform::X360_AND_PS3);
	EXPECT_EQ(xwv.getData().size(), 585728);
	EXPECT_EQ(xwv.getDecodedSampleCount(), 772096);
	EXPECT_EQ(xwv.getLoopStart(), -1);
	EXPECT_EQ(xwv.getLoopBlock(), 0);
	EXPECT_EQ(xwv.getLeadingSampleCount(), 0);
	EXPECT_EQ(xwv.getTrailingSampleCount(), 0);
	EXPECT_EQ(xwv.getFormat(), XWV::Format::XMA);
	EXPECT_EQ(xwv.getBitsPerSample(), 16);
	EXPECT_EQ(xwv.getFrequency(), XWV::Frequency::HZ_44100);
	EXPECT_EQ(xwv.getChannelCount(), 2);
	EXPECT_EQ(xwv.getQuality(), 75);
}
