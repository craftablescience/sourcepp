#include <gtest/gtest.h>

#include <vcdpp/vcdpp.h>

using namespace sourcepp;
using namespace vcdpp;
using namespace vcdpp::literals;

TEST(vcdpp, read_vcd_speak) {
	// todo: move to test assets
	// scenes/escape/ai_escape_speaker01.vcd
	const auto vcd = R"(//Choreo version 1
actor "Aperture_AI"
{
	channel "audio"
	{
		event speak "Portal.escape_00_part1_nag01-1"
		{
			time 0.12 0.952268
			param "Portal.escape_00_part1_nag01-1"
			fixedlength

			cctype "cc_master"
			cctoken ""
			cc_noattentuate
		}
		event speak "Portal.escape_00_part1_nag02-1"
		{
			time 1.433334 2.543016
			param "Portal.escape_00_part1_nag02-1"
			fixedlength

			cctype "cc_master"
			cctoken ""
			cc_noattentuate
		}
		event speak "Portal.escape_00_part1_nag03-1"
		{
			time 2.626667 5.995397
			param "Portal.escape_00_part1_nag03-1"
			fixedlength

			cctype "cc_master"
			cctoken ""
			cc_noattentuate
		}
		event speak "Portal.escape_00_part1_nag01-1"
		{
			time 6.526667 7.358935
			param "Portal.escape_00_part1_nag01-1"
			fixedlength

			cctype "cc_master"
			cctoken ""
			cc_noattentuate
		}
	}
}
"scalesettings"
{
	"CChoreoView" "100"
	"SceneRampTool" "100"
	"ExpressionTool" "100"
	"GestureTool" "100"
	"RampTool" "100"
}
fps 60
snap off
)"_vcd;
	EXPECT_EQ(vcd.getPlatform(), VCD::Platform::VCD);
	EXPECT_EQ(vcd.getVersion(), 1);
	EXPECT_EQ(vcd.getChecksum(), 0);
	EXPECT_TRUE(vcd.getGlobalEvents().empty());
	ASSERT_EQ(vcd.getActors().size(), 1);
	EXPECT_EQ(vcd.getActors()[0].name, "Aperture_AI");
	ASSERT_EQ(vcd.getActors()[0].channels.size(), 1);
	EXPECT_EQ(vcd.getActors()[0].channels[0].name, "audio");
	ASSERT_EQ(vcd.getActors()[0].channels[0].events.size(), 4);
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[0].name, "Portal.escape_00_part1_nag01-1");
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[0].startTime, 0.12);
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[0].endTime, 0.952268);
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[0].param, "Portal.escape_00_part1_nag01-1");
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[1].name, "Portal.escape_00_part1_nag02-1");
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[1].startTime, 1.433334);
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[1].endTime, 2.543016);
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[1].param, "Portal.escape_00_part1_nag02-1");
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[2].name, "Portal.escape_00_part1_nag03-1");
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[2].startTime, 2.626667);
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[2].endTime, 5.995397);
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[2].param, "Portal.escape_00_part1_nag03-1");
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[3].name, "Portal.escape_00_part1_nag01-1");
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[3].startTime, 6.526667);
	EXPECT_FLOAT_EQ(vcd.getActors()[0].channels[0].events[3].endTime, 7.358935);
	EXPECT_EQ(vcd.getActors()[0].channels[0].events[3].param, "Portal.escape_00_part1_nag01-1");
	for (int i = 0; i < 4; i++) {
		EXPECT_EQ(vcd.getActors()[0].channels[0].events[i].type, Event::Type::SPEAK);
		EXPECT_TRUE(vcd.getActors()[0].channels[0].events[i].fixedLength);
		EXPECT_EQ(vcd.getActors()[0].channels[0].events[i].speechData.ccType, Event::SpeechData::CCType::CC_MASTER);
		EXPECT_EQ(vcd.getActors()[0].channels[0].events[i].speechData.ccToken, "");
		EXPECT_EQ(vcd.getActors()[0].channels[0].events[i].speechData.ccFlags, Event::SpeechData::CCFlags::NO_ATTENUATE);
	}
	EXPECT_EQ(vcd.getFaceposerSettings().scaleSettings.choreoView, 100);
	EXPECT_EQ(vcd.getFaceposerSettings().scaleSettings.sceneRampTool, 100);
	EXPECT_EQ(vcd.getFaceposerSettings().scaleSettings.expressionTool, 100);
	EXPECT_EQ(vcd.getFaceposerSettings().scaleSettings.gestureTool, 100);
	EXPECT_EQ(vcd.getFaceposerSettings().scaleSettings.rampTool, 100);
	EXPECT_EQ(vcd.getFaceposerSettings().fps, 60);
	EXPECT_FALSE(vcd.getFaceposerSettings().snap);
}

TEST(vcdpp, read_vce) {
	const auto vce = R"(
// Choreo version 1:  <1> Exported Events
event gesture "Gesture event 1"
{
  time 0.000000 -1.000000
  param "G_shrug"
  absolutetags playback_time
  {
    "apex" 0.160000
    "accent" 0.240000
    "loop" 0.320000
    "end" 0.440000
  }
  absolutetags shifted_time
  {
    "apex" 0.160000
    "accent" 0.240000
    "loop" 0.320000
    "end" 0.440000
  }
  sequenceduration 1.700000
})"_vcd;
	EXPECT_EQ(vce.getPlatform(), VCD::Platform::VCE);
	EXPECT_EQ(vce.getGlobalEvents().size(), 1);
}

#if 0

TEST(vcdpp, blah) {
	std::vector<std::filesystem::path> files;
	for (const auto& it : std::filesystem::recursive_directory_iterator{"/home/lxlewis/Steam/steamapps/common", std::filesystem::directory_options::skip_permission_denied}) {
		if (it.path().extension() == ".vcd") {
			files.push_back(it.path());
		}
	}

	// blacklist corrupted vcds
	std::array<std::filesystem::path, 7> blacklist{
		"/home/lxlewis/Steam/steamapps/common/GarrysMod/garrysmod/scenes/chell/death.vcd",
		"/home/lxlewis/Steam/steamapps/common/GarrysMod/garrysmod/scenes/chell/idle.vcd",
		"/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/update/scenes/gambler/warnsmoker01.vcd",
		"/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/update/scenes/gambler/warnsmoker02.vcd",
		"/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/update/scenes/gambler/warnsmoker03.vcd",
		"/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/update/scenes/gambler/warnsmoker04.vcd",
		"/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/update/scenes/mechanic/warnboomer06.vcd",
	};

	for (const auto& file : files) {
		if (std::ranges::find(blacklist, file) != blacklist.end()) continue;
		EXPECT_NO_THROW(VCD{fs::readFileText(file)}) << file;
	}
}

TEST(vcdpp, parse_event_ramp_extras) {
	const VCD vcd{fs::readFileText("/home/lxlewis/Steam/steamapps/common/Dino D-Day/bin/unittests/dmxtest.vcd")};
	ASSERT_EQ(vcd.getGlobalEvents().size(), 3);
}

TEST(vcdpp, fuzz) {
	const VCD vcd{fs::readFileText("/home/lxlewis/Steam/steamapps/common/Left 4 Dead 2/left4dead2/scenes/coach/laughter01.vcd")};
	ASSERT_EQ(vcd.getGlobalEvents().size(), 1);
}

#endif
