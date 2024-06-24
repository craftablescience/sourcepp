#include <gtest/gtest.h>

#include <fgdpp/fgdpp.h>

using namespace fgdpp;

TEST(fgdpp, parseBasePortal2) {
    FGD fgd{ASSET_ROOT "fgdpp/portal2.fgd", true};
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);
}

#ifdef FGDPP_UNIFIED_FGD
TEST(fgdpp, parseUnifiedFGD) {
    FGD fgd{ASSET_ROOT "fgdpp/unified/game_ui.fgd", true};
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);
}
#endif
