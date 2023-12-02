#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include <studiomodelpp/studiomodelpp.h>

#include "Config.h"

using namespace studiomodelpp;

static std::vector<std::byte> readFileToBuffer(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	file >> std::skipws;
	auto size = std::filesystem::file_size(path);
	std::vector<std::byte> out(size);
	file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
	return out;
}

TEST(Reader, v44_readMDL_clipboard) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(TEST_MODEL_ROOT_PATH "clipboard.mdl"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "clipboard.vtx"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "clipboard.vvd"));
	ASSERT_TRUE(opened);
}

TEST(Reader, v44_readMDL_radio) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(TEST_MODEL_ROOT_PATH "radio.mdl"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "radio.vtx"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "radio.vvd"));
	ASSERT_TRUE(opened);
}

TEST(Reader, v47_readMDL_airboat) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(TEST_MODEL_ROOT_PATH "airboat.mdl"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "airboat.vtx"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "airboat.vvd"));
	ASSERT_TRUE(opened);
}

TEST(Reader, v49_readMDL_life_preserver) {
	StudioModel model;
	bool opened = model.open(readFileToBuffer(TEST_MODEL_ROOT_PATH "life_preserver.mdl"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "life_preserver.vtx"),
	                         readFileToBuffer(TEST_MODEL_ROOT_PATH "life_preserver.vvd"));
	ASSERT_TRUE(opened);
}
