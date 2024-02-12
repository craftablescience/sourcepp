#include <gtest/gtest.h>

#include <studiomodelpp/studiomodelpp.h>

#include "Helpers.h"

using namespace studiomodelpp;

TEST(studiomodelpp, v44_read) {
	StudioModel model;
	bool opened = model.open(::readFileToBuffer(ASSET_ROOT "studiomodelpp/v44.mdl"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v44.vtx"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v44.vvd"));
	ASSERT_TRUE(opened);
}

TEST(studiomodelpp, v47_read) {
	StudioModel model;
	bool opened = model.open(::readFileToBuffer(ASSET_ROOT "studiomodelpp/v47.mdl"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v47.vtx"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v47.vvd"));
	ASSERT_TRUE(opened);
}

TEST(studiomodelpp, v49_read) {
	StudioModel model;
	bool opened = model.open(::readFileToBuffer(ASSET_ROOT "studiomodelpp/v49.mdl"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v49.vtx"),
	                         ::readFileToBuffer(ASSET_ROOT "studiomodelpp/v49.vvd"));
	ASSERT_TRUE(opened);
}
