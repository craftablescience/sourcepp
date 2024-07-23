#include <gtest/gtest.h>

#include <fgdpp/fgdpp.h>

using namespace fgdpp;

TEST(fgdpp, parseEmpty) {
	FGD fgd{ASSET_ROOT "fgdpp/empty.fgd"};
	EXPECT_EQ(fgd.getVersion(), 0);
	EXPECT_EQ(fgd.getMapSize().x, 0);
	EXPECT_EQ(fgd.getMapSize().y, 0);
	EXPECT_TRUE(fgd.getEntities().empty());
	EXPECT_TRUE(fgd.getMaterialExclusionDirs().empty());
	EXPECT_TRUE(fgd.getAutoVisGroups().empty());
}

TEST(fgdpp, parseIdeal) {
	FGD fgd{ASSET_ROOT "fgdpp/ideal.fgd"};
	EXPECT_EQ(fgd.getVersion(), 8);
	EXPECT_EQ(fgd.getMapSize().x, -16384);
	EXPECT_EQ(fgd.getMapSize().y, 16384);
	EXPECT_EQ(fgd.getEntities().size(), 1);
	EXPECT_EQ(fgd.getMaterialExclusionDirs().size(), 9);
	EXPECT_EQ(fgd.getAutoVisGroups().size(), 3);
}

TEST(fgdpp, parsePortal2) {
    FGD fgd{ASSET_ROOT "fgdpp/game/portal2.fgd"};
	EXPECT_EQ(fgd.getVersion(), 0);
	EXPECT_EQ(fgd.getMapSize().x, -16384);
	EXPECT_EQ(fgd.getMapSize().y, 16384);
	EXPECT_EQ(fgd.getEntities().size(), 494);
	EXPECT_TRUE(fgd.getMaterialExclusionDirs().empty());
	EXPECT_TRUE(fgd.getAutoVisGroups().empty());
}

TEST(fgdpp, write) {
	std::string writeContents = FGDWriter::begin()
		.version(8)
		.mapSize({-16384, 16384})
		.beginEntity(
			"PointClass",
			{
				"base(Targetname, Angles, Origin)",
				"halfgridsnap",
				"studio(\"path/model space dot mdl\")",
			},
			"entity_name",
			"An entity that can be linked to another door and create a passage between them dynamically.")
			.keyValue("portal1", "int", "", "", "", true)
			.keyValue("portal2", "int", "Portal 2")
			.keyValue("portal3", "int", "Portal 3", "0")
			.keyValue("portal4", "int", "Portal 4", "0", "This is the wonderful world of Portal 4", false, true)
			.input("portalInput1", "string")
			.input("portalInput2", "string", "Portal Input 2")
			.output("portalOutput1", "string")
			.output("portalOutput2", "string", "Portal Output 2")
			.beginKeyValueFlags("spawn_flags")
				.flag(1, "something clever", true, "This flag is clever")
				.flag(2, "something else", false, "This flag is something else")
				.flag(4, "you said what now?", false)
				.flag(8, "nothing", true)
			.endKeyValueFlags()
			.beginKeyValueChoices("model", "Model used", "models/something02.mdl", "", true, true)
				.choice("models/something01.mdl", "something")
				.choice("models/something02.mdl", "something else (default)")
				.choice("models/something03.mdl", "something completely different")
			.endKeyValueChoices()
		.endEntity()
		.beginEntity("ExtendClass", {}, "entity_name", "")
			.keyValue("portal4", "void", "Portal 4 but better")
			.output("portalOutput3", "string", "Portal Output 3")
			.beginKeyValueFlags("effects", "Effects", "These sure are effects")
				.flag(0, "None", false)
				.flag(1, "Always, very expensive!", false)
				.flag(2, "Bright, dynamic light at entity origin", false)
				.flag(4, "Dim, dynamic light at entity origin", false)
			.endKeyValueFlags()
		.endEntity()
		.materialExclusionDirs({
			"console",
			"debug",
			"engine",
			"hud",
			"perftest/gman",
			"perftest/loader",
			"vgui",
			"voice",
			"vr",
		})
		.beginAutoVisGroup("Brushes")
			.visGroup("Triggers", {
				"trigger_once",
				"trigger_multiple",
			})
			.visGroup("Tool Brushes", {
				"func_areaportal",
				"func_viscluster",
			})
		.endAutoVisGroup()
		.beginAutoVisGroup("Tool Brushes")
			.visGroup("Vis Clusters", {
				"func_viscluster",
			})
		.endAutoVisGroup()
		.bakeToString();

	std::string expectedContents = R"~(@version(8)

@mapsize(-16384, 16384)

@PointClass
	base(Targetname, Angles, Origin)
	halfgridsnap
	studio("path/model space dot mdl")
= entity_name :
	"An entity that can be linked to another door and create a passage between them dynamically."
[
	portal1(int) readonly
	portal2(int) : "Portal 2"
	portal3(int) : "Portal 3" : "0"
	portal4(int) report : "Portal 4" : "0" : "This is the wonderful world of Portal 4"
	input portalInput1(string)
	input portalInput2(string) : "Portal Input 2"
	output portalOutput1(string)
	output portalOutput2(string) : "Portal Output 2"
	spawn_flags(flags) =
	[
		1 : "something clever" : 1 : "This flag is clever"
		2 : "something else" : 0 : "This flag is something else"
		4 : "you said what now?" : 0
		8 : "nothing" : 1
	]
	model(choices) readonly report : "Model used" : "models/something02.mdl" =
	[
		"models/something01.mdl" : "something"
		"models/something02.mdl" : "something else (default)"
		"models/something03.mdl" : "something completely different"
	]
]

@ExtendClass = entity_name : ""
[
	portal4(void) : "Portal 4 but better"
	output portalOutput3(string) : "Portal Output 3"
	effects(flags) : "Effects" : "These sure are effects" =
	[
		0 : "None" : 0
		1 : "Always, very expensive!" : 0
		2 : "Bright, dynamic light at entity origin" : 0
		4 : "Dim, dynamic light at entity origin" : 0
	]
]

@MaterialExclusion
[
	"console"
	"debug"
	"engine"
	"hud"
	"perftest/gman"
	"perftest/loader"
	"vgui"
	"voice"
	"vr"
]

@AutoVisGroup = "Brushes"
[
	"Triggers"
	[
		"trigger_once"
		"trigger_multiple"
	]
	"Tool Brushes"
	[
		"func_areaportal"
		"func_viscluster"
	]
]

@AutoVisGroup = "Tool Brushes"
[
	"Vis Clusters"
	[
		"func_viscluster"
	]
]
)~";

	EXPECT_STREQ(writeContents.c_str(), expectedContents.c_str());
}
