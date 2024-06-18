#include <gtest/gtest.h>

#include <kvpp/kvpp.h>

#include "Helpers.h"

using namespace kvpp;

TEST(kvpp, read_empty) {
	KV1 kv1{""};
	EXPECT_TRUE(kv1.getChildren().empty());
}

TEST(kvpp, read_block) {
	KV1 kv1{R"(
"block"
{
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "block");
	EXPECT_STREQ(kv1[0].getValue().data(), "");
	EXPECT_TRUE(kv1[0].getChildren().empty());
}

TEST(kvpp, read_blocks) {
	KV1 kv1{R"(
"block1"
{
}
"block2"
{
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 2);
	EXPECT_STREQ(kv1[0].getKey().data(), "block1");
	EXPECT_STREQ(kv1[0].getValue().data(), "");
	EXPECT_TRUE(kv1[0].getChildren().empty());
	EXPECT_STREQ(kv1[1].getKey().data(), "block2");
	EXPECT_STREQ(kv1[1].getValue().data(), "");
	EXPECT_TRUE(kv1[1].getChildren().empty());
}

TEST(kvpp, read_keys) {
	KV1 kv1{R"(
"keys"
{
    "test"   "1"
    "test 2"  0
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "keys");
	EXPECT_STREQ(kv1["keys"].getValue().data(), "");
	ASSERT_EQ(kv1["keys"].getChildren().size(), 2);
	EXPECT_STREQ(kv1["keys"][0].getKey().data(), "test");
	EXPECT_STREQ(kv1["keys"]["test"].getValue().data(), "1");
	EXPECT_TRUE(kv1["keys"]["test"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][1].getKey().data(), "test 2");
	EXPECT_STREQ(kv1["keys"]["test 2"].getValue().data(), "0");
	EXPECT_TRUE(kv1["keys"]["test 2"].getChildren().empty());
}

TEST(kvpp, read_escaped) {
	KV1 kv1{R"(
"\"keys\""
{
    "te\"st"  "\\1\in"
    "test\t2"  0\n
}
)", true};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "\"keys\"");
	EXPECT_STREQ(kv1["\"keys\""].getValue().data(), "");
	ASSERT_EQ(kv1["\"keys\""].getChildren().size(), 2);
	EXPECT_STREQ(kv1["\"keys\""][0].getKey().data(), "te\"st");
	EXPECT_STREQ(kv1["\"keys\""]["te\"st"].getValue().data(), "\\1\\in");
	EXPECT_TRUE(kv1["\"keys\""]["te\"st"].getChildren().empty());
	EXPECT_STREQ(kv1["\"keys\""][1].getKey().data(), "test\t2");
	EXPECT_STREQ(kv1["\"keys\""]["test\t2"].getValue().data(), "0\n");
	EXPECT_TRUE(kv1["\"keys\""]["test\t2"].getChildren().empty());
}

TEST(kvpp, read_comments) {
	KV1 kv1{R"(
"keys"
{   // cool
    "test"   "1" // so nice
    "test 2"  0  // here's another one
}
// Wowie
)"};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "keys");
	EXPECT_STREQ(kv1["keys"].getValue().data(), "");
	ASSERT_EQ(kv1["keys"].getChildren().size(), 2);
	EXPECT_STREQ(kv1["keys"][0].getKey().data(), "test");
	EXPECT_STREQ(kv1["keys"]["test"].getValue().data(), "1");
	EXPECT_TRUE(kv1["keys"]["test"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][1].getKey().data(), "test 2");
	EXPECT_STREQ(kv1["keys"]["test 2"].getValue().data(), "0");
	EXPECT_TRUE(kv1["keys"]["test 2"].getChildren().empty());
}

TEST(kvpp, read_subkeys) {
	KV1 kv1{R"(
"keys"
{
    "test"   "1"
	{
		"sub test" whatever
		$ignore_z  1
	}
    "test 2"
	{
	}
	"test 3" "1"
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "keys");
	EXPECT_STREQ(kv1["keys"].getValue().data(), "");
	ASSERT_EQ(kv1["keys"].getChildren().size(), 3);
	EXPECT_STREQ(kv1["keys"][0].getKey().data(), "test");
	EXPECT_STREQ(kv1["keys"]["test"].getValue().data(), "1");
	ASSERT_EQ(kv1["keys"]["test"].getChildren().size(), 2);
	EXPECT_STREQ(kv1["keys"]["test"][0].getKey().data(), "sub test");
	EXPECT_STREQ(kv1["keys"]["test"]["sub test"].getValue().data(), "whatever");
	EXPECT_TRUE(kv1["keys"]["test"]["sub test"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"]["test"][1].getKey().data(), "$ignore_z");
	EXPECT_STREQ(kv1["keys"]["test"]["$ignore_z"].getValue().data(), "1");
	EXPECT_TRUE(kv1["keys"]["test"]["$ignore_z"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][1].getKey().data(), "test 2");
	EXPECT_STREQ(kv1["keys"]["test 2"].getValue().data(), "");
	EXPECT_TRUE(kv1["keys"]["test 2"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][2].getKey().data(), "test 3");
	EXPECT_STREQ(kv1["keys"]["test 3"].getValue().data(), "1");
	EXPECT_TRUE(kv1["keys"]["test 3"].getChildren().empty());
}

TEST(kvpp, read_conditionals) {
	KV1 kv1{R"(
"keys"
{
    "test"   "1"    [$WIN32]
	{
		"sub test" whatever
		$ignore_z  1
	}
    "test 2"
	{
	}
	"test 3" "1"  [$GAME_CONSOLE && !$WIN32]
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 1);
	EXPECT_STREQ(kv1[0].getKey().data(), "keys");
	EXPECT_STREQ(kv1["keys"].getValue().data(), "");
	ASSERT_EQ(kv1["keys"].getChildren().size(), 3);
	EXPECT_STREQ(kv1["keys"][0].getKey().data(), "test");
	EXPECT_STREQ(kv1["keys"]["test"].getValue().data(), "1");
	EXPECT_STREQ(kv1["keys"]["test"].getConditional().data(), "$WIN32");
	ASSERT_EQ(kv1["keys"]["test"].getChildren().size(), 2);
	EXPECT_STREQ(kv1["keys"]["test"][0].getKey().data(), "sub test");
	EXPECT_STREQ(kv1["keys"]["test"]["sub test"].getValue().data(), "whatever");
	EXPECT_TRUE(kv1["keys"]["test"]["sub test"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"]["test"][1].getKey().data(), "$ignore_z");
	EXPECT_STREQ(kv1["keys"]["test"]["$ignore_z"].getValue().data(), "1");
	EXPECT_TRUE(kv1["keys"]["test"]["$ignore_z"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][1].getKey().data(), "test 2");
	EXPECT_STREQ(kv1["keys"]["test 2"].getValue().data(), "");
	EXPECT_TRUE(kv1["keys"]["test 2"].getChildren().empty());
	EXPECT_STREQ(kv1["keys"][2].getKey().data(), "test 3");
	EXPECT_STREQ(kv1["keys"]["test 3"].getValue().data(), "1");
	EXPECT_STREQ(kv1["keys"]["test 3"].getConditional().data(), "$GAME_CONSOLE && !$WIN32");
	EXPECT_TRUE(kv1["keys"]["test 3"].getChildren().empty());
}

TEST(kvpp, read_multiple) {
	KV1 kv1{R"(
entity 0
entity 1
entity 2
)"};
	ASSERT_EQ(kv1.getChildren().size(), 3);
	for (int i = 0; i < kv1.getChildCount("entity"); i++) {
		EXPECT_STREQ(kv1[i].getKey().data(), "entity");
		EXPECT_EQ(kv1("entity", i).getValue<int>(), i);
		EXPECT_TRUE(kv1("entity", i).getChildren().empty());
	}
}

TEST(kvpp, read_includes) {
	KV1 kv1{R"(
#base "resources\parent.res"

resources\child.res
{
}
)"};
	ASSERT_EQ(kv1.getChildren().size(), 2);
	EXPECT_STREQ(kv1[0].getKey().data(), "#base");
	EXPECT_STREQ(kv1["#base"].getValue().data(), "resources\\parent.res");
	EXPECT_TRUE(kv1["#base"].getChildren().empty());
	EXPECT_STREQ(kv1[1].getKey().data(), "resources\\child.res");
	EXPECT_STREQ(kv1["resources\\child.res"].getValue().data(), "");
	EXPECT_TRUE(kv1["resources\\child.res"].getChildren().empty());
}
