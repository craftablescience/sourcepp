#include <gtest/gtest.h>

#include <vcdpp/vcdpp.h>

using namespace sourcepp;
using namespace vcdpp;
using namespace vcdpp::literals;

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
	ASSERT_EQ(vce.getPlatform(), VCD::Platform::VCE);
	ASSERT_EQ(vce.getGlobalEvents().size(), 1);
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
