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

std::shared_ptr<SimpleNode> build_temp_ast_for_next() {
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

TEST_CASE("Test for next relationship") {
    auto ast = build_temp_ast_for_next();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_cfg(pkb);
    SimpleExtractor::DesignExtractor::extract_next_relationship(pkb);

    SECTION("Procedure main") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_next().count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_next().size() == 2);
            REQUIRE(pkb->get_statement_by_id(1)->get_next().count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_next().count(3) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_previous().size() == 0);
            REQUIRE(pkb->get_statement_by_id(1)->get_previous().size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_next().count(3) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_next().count(3) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_previous().count(1) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_previous().count(1) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_previous().count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous().size() == 2);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous().count(1) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous().count(2) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_next().size() == 0);
            REQUIRE(pkb->get_statement_by_id(3)->get_next().size() == 0);
        }
    }

    SECTION("Procedure readPoint") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_next().count(5) == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_next().count(5) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_previous().size() == 0);
            REQUIRE(pkb->get_statement_by_id(4)->get_previous().size() == 0);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_previous().count(4) == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_previous().count(4) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_next().size() == 0);
            REQUIRE(pkb->get_statement_by_id(5)->get_next().size() == 0);
        }
    }

    SECTION("Procedure printResults") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_next().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next().size() == 3);
            REQUIRE(pkb->get_statement_by_id(6)->get_next().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next().count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next().count(9) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_previous().size() == 0);
            REQUIRE(pkb->get_statement_by_id(6)->get_previous().size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_next().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_next().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_previous().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous().size() == 2);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous().count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous().count(7) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_previous().count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous().size() == 3);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous().count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous().count(8) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_next().size() == 0);
            REQUIRE(pkb->get_statement_by_id(9)->get_next().size() == 0);
        }
    }

    SECTION("Procedure computeCentroid") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_next().count(11) == 1);
            std::unordered_set<int> next_stmts(
                {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
            REQUIRE(pkb->get_statement_by_id(10)->get_next() == next_stmts);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_previous().size() == 0);
            REQUIRE(pkb->get_statement_by_id(10)->get_previous().size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            // WHILE stmt
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next().size() == 2);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next().count(15) == 1);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next().count(19) == 1);
            std::unordered_set<int> while_next_stmts({14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
            REQUIRE(pkb->get_statement_by_id(14)->get_next() == while_next_stmts);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous().size() == 2);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous().count(13) == 1);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous().count(18) == 1);
            std::unordered_set<int> while_prev_stmts({10, 11, 12, 13, 14, 15, 16, 17, 18});
            REQUIRE(pkb->get_statement_by_id(14)->get_previous() == while_prev_stmts);

            // Last stmt of while block
            REQUIRE(pkb->get_statement_by_id(18)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(18)->get_direct_next().count(14) == 1);

            // IF stmt
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next().size() == 2);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next().count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next().count(21) == 1);
            std::unordered_set<int> if_next_stmts({20, 21, 22, 23});
            REQUIRE(pkb->get_statement_by_id(19)->get_next() == if_next_stmts);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_previous().size() == 1);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_previous().count(14) == 1);
            std::unordered_set<int> if_prev_stmts({10, 11, 12, 13, 14, 15, 16, 17, 18});
            REQUIRE(pkb->get_statement_by_id(19)->get_previous() == if_prev_stmts);

            // Last stmt of then branch
            REQUIRE(pkb->get_statement_by_id(20)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_direct_next().count(23) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_next().count(23) == 1);

            // Last stmt of else branch
            REQUIRE(pkb->get_statement_by_id(22)->get_direct_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_direct_next().count(23) == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_next().size() == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_next().count(23) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous().size() == 2);
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous().count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous().count(22) == 1);
            std::unordered_set<int> prev_stmts(
                {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22});
            REQUIRE(pkb->get_statement_by_id(23)->get_previous() == prev_stmts);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_next().size() == 0);
            REQUIRE(pkb->get_statement_by_id(23)->get_next().size() == 0);
        }

        SECTION("Cross-level edge cases") {
            REQUIRE(pkb->get_statement_by_id(11)->get_next().count(16) == 1);
            REQUIRE(pkb->get_statement_by_id(11)->get_next().count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(11)->get_next().count(22) == 1);
            REQUIRE(pkb->get_statement_by_id(16)->get_previous().count(11) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_previous().count(11) == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_previous().count(11) == 1);

            REQUIRE(pkb->get_statement_by_id(16)->get_next().count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_previous().count(16) == 1);
        }
    }
}