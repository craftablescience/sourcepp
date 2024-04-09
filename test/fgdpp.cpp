#include <gtest/gtest.h>

#include <fgdpp/fgdpp.h>

#include "Helpers.h"

using namespace fgdpp;

TEST(fgdpp, parseBasePortal2) {
    FGDParser* fgdFile = new FGDParser(ASSET_ROOT "fgdpp/portal2.fgd", true);
    ASSERT_TRUE(fgdFile);
    ASSERT_TRUE(fgdFile->parseError.err == NO_ERROR);
}

//adds support for Spen's Unified FGD format changes.
#ifdef FGDPP_UNIFIED_FGD
TEST(fgdpp, parseUnifiedFGD) {

    FGDParser* fgdFile = new FGDParser(ASSET_ROOT "fgdpp/unified/game_ui.fgd", true);
    ASSERT_TRUE(fgdFile);
    std::cout << fgdFile->parseError.err << std::endl;
    ASSERT_TRUE(fgdFile->parseError.err == NO_ERROR);
}
#endif