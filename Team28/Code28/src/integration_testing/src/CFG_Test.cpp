#include "catch.hpp"

#include "KnowledgeBase/PKB.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/shared/Source.h"
#include "SimpleExtractor/DesignExtractor.h"
#include <memory>
#include <string>

using namespace KnowledgeBase;
using namespace Parser;

std::shared_ptr<SimpleNode> build_temp_ast() {
    std::string source;
    source += "procedure main {";
    source += "    flag = 0;";
    source += "    call computeCentroid;";
    source += "    call printResults;";
    source += "}";

    source += "procedure readPoint {";
    source += "    read x;";
    source += "    read y;";
    source += "}";

    source += "procedure printResults {";
    source += "    print flag;";
    source += "    print cenX;";
    source += "    print cenY;";
    source += "    print normSq;";
    source += "}";

    source += "procedure computeCentroid {";
    source += "    count = 0;";
    source += "    cenX = 0;";
    source += "    cenY = 0;";
    source += "    call printResults;";
    source += "    while ((x != 0) && (y != 0)) {";
    source += "        count = count + 1;";
    source += "        cenX = cenX + x;";
    source += "        cenY = cenY + y;";
    source += "        call readPoint;";
    source += "    }";
    source += "    if (count == 0) then {";
    source += "        flag = 1;";
    source += "    } else {";
    source += "        cenX = cenX / count;";
    source += "        cenY = cenY / count;";
    source += "    }";
    source += "    normSq = cenX * cenX + cenY * cenY;";
    source += "}";

    auto base_source = std::make_shared<Parser::Source>(source);
    Parser::SimpleParser parser(base_source);
    auto ast = parser.parse_program();
    return ast;
}

TEST_CASE("Test for CFG") {
    auto ast = build_temp_ast();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_cfg(pkb);
    std::unordered_map<int, std::unordered_set<int>> cfg = pkb->get_cfg();
    REQUIRE(cfg.size() == 19);

    SECTION("Procedure main") {
        REQUIRE(cfg[1].size() == 1);
        REQUIRE(cfg[1].count(2) == 1);
        REQUIRE(cfg[2].size() == 1);
        REQUIRE(cfg[2].count(3) == 1);
        REQUIRE(cfg.count(3) == 0);
    }

    SECTION("Procedure readPoint") {
        REQUIRE(cfg[4].size() == 1);
        REQUIRE(cfg[4].count(5) == 1);
        REQUIRE(cfg.count(5) == 0);
    }

    SECTION("Procedure printResults") {
        REQUIRE(cfg[6].size() == 1);
        REQUIRE(cfg[6].count(7) == 1);
        REQUIRE(cfg[7].size() == 1);
        REQUIRE(cfg[7].count(8) == 1);
        REQUIRE(cfg[8].size() == 1);
        REQUIRE(cfg[8].count(9) == 1);
        REQUIRE(cfg.count(9) == 0);
    }

    SECTION("Procedure computeCentroid") {
        REQUIRE(cfg[10].size() == 1);
        REQUIRE(cfg[10].count(11) == 1);
        REQUIRE(cfg[11].size() == 1);
        REQUIRE(cfg[11].count(12) == 1);
        REQUIRE(cfg[12].size() == 1);
        REQUIRE(cfg[12].count(13) == 1);
        REQUIRE(cfg[13].size() == 1);
        REQUIRE(cfg[13].count(14) == 1);
        REQUIRE(cfg[14].size() == 2);
        REQUIRE(cfg[14].count(15) == 1);
        REQUIRE(cfg[14].count(19) == 1);
        REQUIRE(cfg[15].size() == 1);
        REQUIRE(cfg[15].count(16) == 1);
        REQUIRE(cfg[16].size() == 1);
        REQUIRE(cfg[16].count(17) == 1);
        REQUIRE(cfg[17].size() == 1);
        REQUIRE(cfg[17].count(18) == 1);
        REQUIRE(cfg[18].size() == 1);
        REQUIRE(cfg[18].count(14) == 1);
        REQUIRE(cfg[19].size() == 2);
        REQUIRE(cfg[19].count(20) == 1);
        REQUIRE(cfg[19].count(21) == 1);
        REQUIRE(cfg[20].size() == 1);
        REQUIRE(cfg[20].count(23) == 1);
        REQUIRE(cfg[21].size() == 1);
        REQUIRE(cfg[21].count(22) == 1);
        REQUIRE(cfg[22].size() == 1);
        REQUIRE(cfg[22].count(23) == 1);
        REQUIRE(cfg.count(23) == 0);
    }
}