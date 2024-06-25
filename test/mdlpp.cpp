#include <gtest/gtest.h>

#include <mdlpp/mdlpp.h>
#include <sourcepp/fs/FS.h>

using namespace mdlpp;
using namespace sourcepp;

TEST(mdlpp, v44_read) {
	StudioModel model;
	bool opened = model.open(fs::readFileBuffer(ASSET_ROOT "mdlpp/v44.mdl"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v44.vtx"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v44.vvd"));
	ASSERT_TRUE(opened);
}

TEST(mdlpp, v47_read) {
	StudioModel model;
	bool opened = model.open(fs::readFileBuffer(ASSET_ROOT "mdlpp/v47.mdl"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v47.vtx"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v47.vvd"));
	ASSERT_TRUE(opened);
}

TEST(mdlpp, v49_read) {
	StudioModel model;
	bool opened = model.open(fs::readFileBuffer(ASSET_ROOT "mdlpp/v49.mdl"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v49.vtx"),
	                         fs::readFileBuffer(ASSET_ROOT "mdlpp/v49.vvd"));
	ASSERT_TRUE(opened);
}
