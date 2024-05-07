#include <gtest/gtest.h>

#include <fgdpp/fgdpp.h>
#include <fstream>

#include "Helpers.h"

#define quoted(str) #str

using namespace fgdpp;

TEST(fgdpp, parseBasePortal2) {
    FGD fgd{ASSET_ROOT "fgdpp/portal2.fgd", true};
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);
}

TEST(fgdpp, parseBasePortal2NoInclude) {
    FGD fgd{ASSET_ROOT "fgdpp/portal2.fgd", false};
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);
}

TEST(fgdpp, parseWriteP2CE)
{
    FGD fgd{ASSET_ROOT "fgdpp/p2ce.fgd", false}; //P2CE doesn't have includes
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);

    unsigned long currentLength = fgd.FGDFileContents.entities.size();

    Entity& ent = fgd.FGDFileContents.entities.emplace_back();
    ent.entityName = "func_windigo";
    auto& property = ent.classProperties.emplace_back();
    property.name = "base";
    auto& pp = property.classProperties.emplace_back();
    pp.properties.emplace_back("func_brush" );

    ent.entityDescription.emplace_back( quoted("An entity designed for having too many bitches.") );

    auto& io = ent.inputOutput.emplace_back();
    io.name = "cosume_flesh";
    io.putType = fgdpp::IO::INPUT;
    io.type = fgdpp::EntityIOPropertyType::t_bool;
    io.stringType = "boolean";

    auto& entProps = ent.entityProperties.emplace_back();
    entProps.propertyName = "has_bitches";
    entProps.type = "boolean";
    entProps.defaultValue = "1";
    entProps.displayName = quoted("Has Bitches?");

    ent.type = "@SolidClass";

    unsigned long afterLength = fgd.FGDFileContents.entities.size();

    auto str = fgd.Write();

    auto fl = std::ofstream(ASSET_ROOT "test_results/test.fgd");
    ASSERT_TRUE(fl.is_open());
    fl.write(str.data(), str.length());
    fl.close();
    ASSERT_GT(afterLength, currentLength);

    FGD fgd2{ASSET_ROOT "test_results/test.fgd", false}; //P2CE doesn't have includes
    ASSERT_EQ(fgd2.parseError.err, ParseError::NO_ERROR);

}


#ifdef FGDPP_UNIFIED_FGD
TEST(fgdpp, parseUnifiedFGD) {
    FGD fgd{ASSET_ROOT "fgdpp/unified/game_ui.fgd", true};
    ASSERT_EQ(fgd.parseError.err, ParseError::NO_ERROR);
}
#endif
