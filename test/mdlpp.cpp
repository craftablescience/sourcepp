#include <gtest/gtest.h>

#include <mdlpp/mdlpp.h>

#include "Helpers.h"

using namespace mdlpp;

TEST(mdlpp, v44_read) {
	StudioModel model;
	bool opened = model.open(::readFile(ASSET_ROOT "studiomodelpp/v44.mdl"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v44.vtx"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v44.vvd"));
	ASSERT_TRUE(opened);
}

TEST(mdlpp, v47_read) {
	StudioModel model;
	bool opened = model.open(::readFile(ASSET_ROOT "studiomodelpp/v47.mdl"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v47.vtx"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v47.vvd"));
	ASSERT_TRUE(opened);
}

TEST(mdlpp, v49_read) {
	StudioModel model;
	bool opened = model.open(::readFile(ASSET_ROOT "studiomodelpp/v49.mdl"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v49.vtx"),
	                         ::readFile(ASSET_ROOT "studiomodelpp/v49.vvd"));
	ASSERT_TRUE(opened);
}
