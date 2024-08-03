#if 0

#include <gtest/gtest.h>

#include <gamepp/gamepp.h>

using namespace gamepp;

TEST(gamepp, commandsBlocking) {
	auto game = GameInstance::find();
	ASSERT_TRUE(game);

	// For Portal 2 - it can *almost* play through the whole map
	(*game)
		.command("developer 0").command("sv_cheats 1").command("cl_showpos 1").command("cl_yawspeed 180")
		.command("map sp_a2_catapult_intro")
		.wait(8)
		.inputHold("forward", 15)
		.inputOnce("use")
		.inputHold("left", 0.99)
		.wait(0.6)
		.inputBegin("forward")
		.wait(2.3)
		.inputOnce("use")
		.wait(1)
		.inputEnd("forward")
		.inputHold("left", 0.99)
		.inputBegin("forward")
		.wait(3)
		.inputHold("right", 0.5)
		.wait(0.86)
		.inputHold("left", 0.5)
		.wait(1.6)
		.inputEnd("forward")
		.wait(0.2)
		.inputOnce("use")
		.inputBegin("forward")
		.inputHold("left", 0.3)
		.wait(0.3)
		.inputHold("right", 0.3)
		.wait(8)
		.inputEnd("forward");
}

#endif
