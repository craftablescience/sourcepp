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

TEST(sndpp, read_wav_vdat) {
	WAV wav{ASSET_ROOT "sndpp/gman_riseshine.wav"};
	ASSERT_TRUE(wav);
	EXPECT_EQ(wav.getSignature(), RIFF_SIGNATURE);
	EXPECT_EQ(wav.getChunks().size(), 5);

	const auto fmt = wav.getFirstWAVChunk<WAV::CHUNK_FMT>();
	ASSERT_TRUE(fmt);
	EXPECT_EQ(fmt->format, 2);
	EXPECT_EQ(fmt->channels, 1);
	EXPECT_EQ(fmt->samplesPerSecond, 22050);
	EXPECT_EQ(fmt->averageBytesPerSecond, 11155);
	EXPECT_EQ(fmt->blockAlign, 512);
	EXPECT_EQ(fmt->bitsPerSample, 4);
	EXPECT_EQ(fmt->extraCompressionInfo.size(), 34);

	const auto data = wav.getFirstWAVChunk<WAV::CHUNK_DATA>();
	ASSERT_TRUE(data);
	EXPECT_EQ(data->size(), 74240);

	const auto vdat = wav.getFirstWAVChunk<WAV::CHUNK_VDAT>();
	ASSERT_TRUE(vdat);
	EXPECT_EQ(vdat->version, 1.0);
	EXPECT_STREQ(vdat->plaintext.c_str(), "Rise and shine, Mr. Freeman. Rise and shine.");
	EXPECT_STREQ(vdat->words[0].word.c_str(), "Rise");
	EXPECT_EQ(vdat->words[0].startTime, 0.0);
	EXPECT_EQ(vdat->words[0].phonemes[0].code, 633);
	EXPECT_STREQ(vdat->words[0].phonemes[0].name.c_str(), "r");
	EXPECT_EQ(vdat->words[0].phonemes[0].volume, 1.0);
	EXPECT_STREQ(vdat->words[1].word.c_str(), "and");
	EXPECT_STREQ(vdat->words[2].word.c_str(), "shine,");
	EXPECT_STREQ(vdat->words[3].word.c_str(), "Mr.");
	EXPECT_STREQ(vdat->words[4].word.c_str(), "Freeman.");
	EXPECT_STREQ(vdat->words[5].word.c_str(), "shine");
	EXPECT_STREQ(vdat->words[6].word.c_str(), "Mr");
	EXPECT_STREQ(vdat->words[7].word.c_str(), "Freeman");
	EXPECT_STREQ(vdat->words[8].word.c_str(), "Rise");
	EXPECT_STREQ(vdat->words[9].word.c_str(), "and");
	EXPECT_STREQ(vdat->words[10].word.c_str(), "shine.");
	EXPECT_STREQ(vdat->words[11].word.c_str(), "shine");
	EXPECT_EQ(vdat->emphasis.size(), 0);
	EXPECT_STREQ(vdat->options.at("voice_duck").c_str(), "0");
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
