#include "catch.hpp"

#include "KnowledgeBase/Constant.h"

using namespace KnowledgeBase;

TEST_CASE("KnowledgeBase::Constant") {
    Constant cst(100);

    SECTION("get_value") { REQUIRE(cst.get_value() == 100); }
}
