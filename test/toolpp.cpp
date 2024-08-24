#include <gtest/gtest.h>

#include <toolpp/toolpp.h>

using namespace toolpp;

TEST(toolpp, openBinary) {
	CmdSeq cmdSeq{ASSET_ROOT "toolpp/binary.wc"};
	ASSERT_EQ(cmdSeq.getSequences().size(), 8);
}

TEST(toolpp, openKeyValues) {
	CmdSeq cmdSeq{ASSET_ROOT "toolpp/keyvalues.wc"};
	ASSERT_EQ(cmdSeq.getSequences().size(), 4);
}
