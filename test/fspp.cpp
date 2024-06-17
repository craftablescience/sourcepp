#include <gtest/gtest.h>

#include <fspp/fspp.h>

using namespace fspp;
using namespace sourcepp;

#if 0

TEST(fspp, open_portal2) {
	auto fs = FileSystem::load(620, "portal2");
	ASSERT_TRUE(fs);
}

TEST(fspp, open_p2ce) {
	auto fs = FileSystem::load(440000, "p2ce");
	ASSERT_TRUE(fs);
}

#endif
