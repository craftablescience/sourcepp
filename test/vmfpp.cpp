#include <gtest/gtest.h>

#include <vmfpp/vmfpp.h>

#include "Helpers.h"

using namespace vmfpp;

TEST(vmfpp, rootEmpty) {
    constexpr std::string_view data = R"(
node
{
}
)";
	auto vmf = VMF::openMem(data);
	ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 0);
}

TEST(vmfpp, rootMulti) {
    constexpr std::string_view data = R"(
node1
{
}
node2
{
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 2);
    ASSERT_TRUE(vmf->hasSection("node1"));
    ASSERT_TRUE(vmf->hasSection("node2"));
    ASSERT_EQ(vmf->getSections().at("node1").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node1").at(0).getNumItems(), 0);
    ASSERT_EQ(vmf->getSections().at("node2").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node2").at(0).getNumItems(), 0);
}

TEST(vmfpp, valueBasic) {
    constexpr std::string_view data = R"(
node
{
    "key" "value"
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 1);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasValue("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getValue("key").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("node").at(0).getValue("key").at(0).data(), "value");
}

TEST(vmfpp, valueMultiline) {
    constexpr std::string_view data = R"(
node
{
    "key" "value
on
several lines"
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 1);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasValue("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getValue("key").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("node").at(0).getValue("key").at(0).data(), "value\non\nseveral lines");
}

TEST(vmfpp, childEmpty) {
    constexpr std::string_view data = R"(
node
{
    key
    {
    }
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 1);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasChild("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key").at(0).getNumItems(), 0);
}

TEST(vmfpp, childBasic) {
    constexpr std::string_view data = R"(
node
{
    key
    {
        "inner" "value"
    }
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 1);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasChild("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key").at(0).getNumItems(), 1);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).getChild("key").at(0).hasValue("inner"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key").at(0).getValue("inner").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("node").at(0).getChild("key").at(0).getValue("inner").at(0).data(), "value");
}

TEST(vmfpp, valueBasicAndChild) {
    constexpr std::string_view data = R"(
node
{
    "key" "value"
    key2
    {
    }
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 2);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasValue("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getValue("key").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("node").at(0).getValue("key").at(0).data(), "value");
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasChild("key2"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key2").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key2").at(0).getNumItems(), 0);
}

TEST(vmfpp, valueMultilineAndChild) {
    constexpr std::string_view data = R"(
node
{
    "key" "value
on
several lines"
    key2
    {
    }
}
)";
	auto vmf = VMF::openMem(data);
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 1);
    ASSERT_TRUE(vmf->hasSection("node"));
    ASSERT_EQ(vmf->getSections().at("node").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getNumItems(), 2);
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasValue("key"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getValue("key").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("node").at(0).getValue("key").at(0).data(), "value\non\nseveral lines");
    ASSERT_TRUE(vmf->getSections().at("node").at(0).hasChild("key2"));
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key2").size(), 1);
    ASSERT_EQ(vmf->getSections().at("node").at(0).getChild("key2").at(0).getNumItems(), 0);
}

TEST(vmfpp, file) {
	auto vmf = VMF::openFile(ASSET_ROOT "vmfpp/test.vmf");
    ASSERT_TRUE(vmf);
    ASSERT_EQ(vmf->getSections().size(), 9);
    ASSERT_TRUE(vmf->hasSection("entity"));
    ASSERT_EQ(vmf->getSections().at("entity").size(), 3);
    ASSERT_EQ(vmf->getSections().at("entity").at(0).getNumItems(), 5);
    ASSERT_TRUE(vmf->getSections().at("entity").at(0).hasValue("classname"));
    ASSERT_EQ(vmf->getSections().at("entity").at(0).getValue("classname").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("entity").at(0).getValue("classname").at(0).data(), "info_player_start");
    ASSERT_TRUE(vmf->getSections().at("entity").at(0).hasChild("editor"));
    ASSERT_EQ(vmf->getSections().at("entity").at(0).getChild("editor").size(), 1);
    ASSERT_EQ(vmf->getSections().at("entity").at(0).getChild("editor").at(0).getNumItems(), 4);
    ASSERT_TRUE(vmf->getSections().at("entity").at(0).getChild("editor").at(0).hasValue("color"));
    ASSERT_EQ(vmf->getSections().at("entity").at(0).getChild("editor").at(0).getValue("color").size(), 1);
    ASSERT_STREQ(vmf->getSections().at("entity").at(0).getChild("editor").at(0).getValue("color").at(0).data(), "0 255 0");
}
