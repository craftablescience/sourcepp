#include <gtest/gtest.h>

#include <studiomodelpp/studiomodelpp.h>

#include "Helpers.h"

using namespace studiomodelpp;

TEST(Reader, v44_read) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(ASSET_ROOT "v44.mdl"),
	                         readFileToBuffer(ASSET_ROOT "v44.vtx"),
	                         readFileToBuffer(ASSET_ROOT "v44.vvd"));
	ASSERT_TRUE(opened);
}

TEST(Reader, v47_read) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(ASSET_ROOT "v47.mdl"),
	                         readFileToBuffer(ASSET_ROOT "v47.vtx"),
	                         readFileToBuffer(ASSET_ROOT "v47.vvd"));
	ASSERT_TRUE(opened);
}

TEST(Reader, v49_read) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(ASSET_ROOT "v49.mdl"),
	                         readFileToBuffer(ASSET_ROOT "v49.vtx"),
	                         readFileToBuffer(ASSET_ROOT "v49.vvd"));
	ASSERT_TRUE(opened);
}
