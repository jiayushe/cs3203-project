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

std::shared_ptr<SimpleNode> build_multi_proc_ast() {
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
    auto ast = build_multi_proc_ast();
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

TEST_CASE("Test for next relationship") {
    auto ast = build_multi_proc_ast();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_cfg(pkb);
    SimpleExtractor::DesignExtractor::extract_next_relationship(pkb);

    SECTION("Procedure main") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_next()->count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_next()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(1)->get_next()->count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(1)->get_next()->count(3) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(1)->get_direct_previous()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(1)->get_previous()->size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_next()->count(3) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_next()->count(3) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_direct_previous()->count(1) == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(2)->get_previous()->count(1) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_previous()->count(2) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous()->count(1) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_previous()->count(2) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_next()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(3)->get_next()->size() == 0);
        }
    }

    SECTION("Procedure readPoint") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_next()->count(5) == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_next()->count(5) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_previous()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(4)->get_previous()->size() == 0);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_previous()->count(4) == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_previous()->count(4) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_next()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(5)->get_next()->size() == 0);
        }
    }

    SECTION("Procedure printResults") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_next()->count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next()->size() == 3);
            REQUIRE(pkb->get_statement_by_id(6)->get_next()->count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next()->count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(6)->get_next()->count(9) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(6)->get_direct_previous()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(6)->get_previous()->size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_next()->count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_next()->count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_previous()->count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous()->count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_previous()->count(7) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_previous()->count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous()->size() == 3);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous()->count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous()->count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_previous()->count(8) == 1);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_next()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(9)->get_next()->size() == 0);
        }
    }

    SECTION("Procedure computeCentroid") {
        SECTION("First stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_next()->count(11) == 1);
            std::unordered_set<int> next_stmts(
                {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
            REQUIRE(*pkb->get_statement_by_id(10)->get_next() == next_stmts);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(10)->get_direct_previous()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(10)->get_previous()->size() == 0);
        }

        SECTION("Middle stmt of procedure") {
            // WHILE stmt
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next()->count(15) == 1);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_next()->count(19) == 1);
            std::unordered_set<int> while_next_stmts({14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
            REQUIRE(*pkb->get_statement_by_id(14)->get_next() == while_next_stmts);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous()->count(13) == 1);
            REQUIRE(pkb->get_statement_by_id(14)->get_direct_previous()->count(18) == 1);
            std::unordered_set<int> while_prev_stmts({10, 11, 12, 13, 14, 15, 16, 17, 18});
            REQUIRE(*pkb->get_statement_by_id(14)->get_previous() == while_prev_stmts);

            // Last stmt of while block
            REQUIRE(pkb->get_statement_by_id(18)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(18)->get_direct_next()->count(14) == 1);

            // IF stmt
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next()->count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_next()->count(21) == 1);
            std::unordered_set<int> if_next_stmts({20, 21, 22, 23});
            REQUIRE(*pkb->get_statement_by_id(19)->get_next() == if_next_stmts);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_previous()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(19)->get_direct_previous()->count(14) == 1);
            std::unordered_set<int> if_prev_stmts({10, 11, 12, 13, 14, 15, 16, 17, 18});
            REQUIRE(*pkb->get_statement_by_id(19)->get_previous() == if_prev_stmts);

            // Last stmt of then branch
            REQUIRE(pkb->get_statement_by_id(20)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_direct_next()->count(23) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_next()->count(23) == 1);

            // Last stmt of else branch
            REQUIRE(pkb->get_statement_by_id(22)->get_direct_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_direct_next()->count(23) == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_next()->size() == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_next()->count(23) == 1);
        }

        SECTION("Last stmt of procedure") {
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous()->size() == 2);
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous()->count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_previous()->count(22) == 1);
            std::unordered_set<int> prev_stmts(
                {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22});
            REQUIRE(*pkb->get_statement_by_id(23)->get_previous() == prev_stmts);

            // Negative cases
            REQUIRE(pkb->get_statement_by_id(23)->get_direct_next()->size() == 0);
            REQUIRE(pkb->get_statement_by_id(23)->get_next()->size() == 0);
        }

        SECTION("Cross-level edge cases") {
            REQUIRE(pkb->get_statement_by_id(11)->get_next()->count(16) == 1);
            REQUIRE(pkb->get_statement_by_id(11)->get_next()->count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(11)->get_next()->count(22) == 1);
            REQUIRE(pkb->get_statement_by_id(16)->get_previous()->count(11) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_previous()->count(11) == 1);
            REQUIRE(pkb->get_statement_by_id(22)->get_previous()->count(11) == 1);

            REQUIRE(pkb->get_statement_by_id(16)->get_next()->count(20) == 1);
            REQUIRE(pkb->get_statement_by_id(20)->get_previous()->count(16) == 1);
        }
    }
}

TEST_CASE("DEPKBIntegration_Multi_Procedure_Program") {
    auto ast = build_multi_proc_ast();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_call_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    REQUIRE(pkb->get_procedures().size() == 4);

    SECTION("Valid record of multiple procedures") {
        REQUIRE(pkb->get_procedure_by_name("main")->get_statements()->size() == 3);
        REQUIRE(pkb->get_procedure_by_name("readPoint")->get_statements()->size() == 2);
        REQUIRE(pkb->get_procedure_by_name("printResults")->get_statements()->size() == 4);
        REQUIRE(pkb->get_procedure_by_name("computeCentroid")->get_statements()->size() == 14);
    }

    SECTION("Modifies and Uses for Procedures") {
        REQUIRE(pkb->get_procedures().size() == 4);
        REQUIRE(pkb->get_variables().size() == 7);
        REQUIRE(pkb->get_procedure_by_name("main")->get_modifies()->size() == 7);
        REQUIRE(pkb->get_procedure_by_name("readPoint")->get_modifies()->size() == 2);
        REQUIRE(pkb->get_procedure_by_name("printResults")->get_modifies()->size() == 0);
        REQUIRE(pkb->get_procedure_by_name("computeCentroid")->get_modifies()->size() == 7);
        REQUIRE(pkb->get_variable_by_name("x")->get_modified_by_procedures()->size() == 3);
        REQUIRE(pkb->get_variable_by_name("y")->get_modified_by_procedures()->size() == 3);
        REQUIRE(pkb->get_variable_by_name("cenX")->get_modified_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("cenY")->get_modified_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("flag")->get_modified_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("count")->get_modified_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("normSq")->get_modified_by_procedures()->size() == 2);
        REQUIRE(pkb->get_procedure_by_name("main")->get_uses()->size() == 7);
        REQUIRE(pkb->get_procedure_by_name("readPoint")->get_uses()->size() == 0);
        REQUIRE(pkb->get_procedure_by_name("printResults")->get_uses()->size() == 4);
        REQUIRE(pkb->get_procedure_by_name("computeCentroid")->get_uses()->size() == 7);
        REQUIRE(pkb->get_variable_by_name("x")->get_used_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("y")->get_used_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("cenX")->get_used_by_procedures()->size() == 3);
        REQUIRE(pkb->get_variable_by_name("cenY")->get_used_by_procedures()->size() == 3);
        REQUIRE(pkb->get_variable_by_name("flag")->get_used_by_procedures()->size() == 3);
        REQUIRE(pkb->get_variable_by_name("count")->get_used_by_procedures()->size() == 2);
        REQUIRE(pkb->get_variable_by_name("normSq")->get_used_by_procedures()->size() == 3);
    }

    SECTION("Call") {
        SECTION("Call relationship between procedure") {
            SECTION("Direct call") {
                REQUIRE(pkb->get_procedure_by_name("main")->get_direct_callees()->size() == 2);
                REQUIRE(pkb->get_procedure_by_name("main")->get_direct_callees()->count(
                            "computeCentroid") == 1);
                REQUIRE(pkb->get_procedure_by_name("main")->get_direct_callees()->count(
                            "printResults") == 1);
                REQUIRE(pkb->get_procedure_by_name("main")->get_direct_callers()->size() == 0);

                REQUIRE(
                    pkb->get_procedure_by_name("computeCentroid")->get_direct_callees()->size() ==
                    2);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_direct_callees()
                            ->count("readPoint") == 1);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_direct_callees()
                            ->count("printResults") == 1);
                REQUIRE(
                    pkb->get_procedure_by_name("computeCentroid")->get_direct_callers()->size() ==
                    1);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_direct_callers()
                            ->count("main") == 1);

                REQUIRE(pkb->get_procedure_by_name("printResults")->get_direct_callers()->size() ==
                        2);
                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_direct_callers()
                            ->count("main") == 1);
                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_direct_callers()
                            ->count("computeCentroid") == 1);

                REQUIRE(pkb->get_procedure_by_name("readPoint")->get_direct_callers()->size() == 1);
                REQUIRE(pkb->get_procedure_by_name("readPoint")
                            ->get_direct_callers()
                            ->count("computeCentroid") == 1);
            }

            SECTION("Indirect call") {
                REQUIRE(pkb->get_procedure_by_name("main")->get_callees()->size() == 3);
                REQUIRE(pkb->get_procedure_by_name("main")->get_callees()->count(
                            "computeCentroid") == 1);
                REQUIRE(pkb->get_procedure_by_name("main")->get_callees()->count("printResults") ==
                        1);
                REQUIRE(pkb->get_procedure_by_name("main")->get_callees()->count("readPoint") == 1);
                REQUIRE(pkb->get_procedure_by_name("main")->get_callers()->size() == 0);

                REQUIRE(pkb->get_procedure_by_name("computeCentroid")->get_callees()->size() == 2);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_callees()
                            ->count("printResults") == 1);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_callees()
                            ->count("readPoint") == 1);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")->get_callers()->size() == 1);
                REQUIRE(
                    pkb->get_procedure_by_name("computeCentroid")->get_callers()->count("main") ==
                    1);

                REQUIRE(pkb->get_procedure_by_name("printResults")->get_callers()->size() == 2);
                REQUIRE(pkb->get_procedure_by_name("printResults")->get_callers()->count("main") ==
                        1);
                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_callers()
                            ->count("computeCentroid") == 1);

                REQUIRE(pkb->get_procedure_by_name("readPoint")->get_callers()->size() == 2);
                REQUIRE(pkb->get_procedure_by_name("readPoint")
                            ->get_callers()
                            ->count("computeCentroid") == 1);
                REQUIRE(pkb->get_procedure_by_name("readPoint")->get_callers()->count("main") == 1);
            }

            SECTION("Negative test") {
                REQUIRE(pkb->get_procedure_by_name("main")->get_direct_callees()->count(
                            "readPoint") == 0);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_direct_callees()
                            ->count("main") == 0);
                REQUIRE(
                    pkb->get_procedure_by_name("computeCentroid")->get_callees()->count("main") ==
                    0);
            }

            SECTION("Call by statement") {
                REQUIRE(pkb->get_procedure_by_name("main")->get_called_by_statements()->size() ==
                        0);

                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_called_by_statements()
                            ->size() == 1);
                REQUIRE(pkb->get_procedure_by_name("computeCentroid")
                            ->get_called_by_statements()
                            ->count(2) == 1);
                REQUIRE(pkb->get_statement_by_id(2)->get_procedure_called() == "computeCentroid");

                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_called_by_statements()
                            ->size() == 2);
                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_called_by_statements()
                            ->count(3) == 1);
                REQUIRE(pkb->get_procedure_by_name("printResults")
                            ->get_called_by_statements()
                            ->count(13) == 1);
                REQUIRE(pkb->get_statement_by_id(3)->get_procedure_called() == "printResults");
                REQUIRE(pkb->get_statement_by_id(13)->get_procedure_called() == "printResults");

                REQUIRE(
                    pkb->get_procedure_by_name("readPoint")->get_called_by_statements()->size() ==
                    1);
                REQUIRE(pkb->get_procedure_by_name("readPoint")
                            ->get_called_by_statements()
                            ->count(18) == 1);
                REQUIRE(pkb->get_statement_by_id(18)->get_procedure_called() == "readPoint");
            }
        }
    }
}

std::shared_ptr<SimpleNode> build_ast_for_next_and_affects() {
    std::string source;

    source += "procedure First {";
    source += "    a = 0;";
    source += "    b = 5;";
    source += "    while (c < 10) {";
    source += "        if (d != 2) then {";
    source += "            d = a + 1;";
    source += "        } else {";
    source += "            e = a + d;";
    source += "            h = b;";
    source += "        }";
    source += "        e = b + a;";
    source += "        z = 1;";
    source += "        call Second;";
    source += "        f = z * 10;";
    source += "        c = c + 1;";
    source += "        while (d < 10) {";
    source += "            d = a * 2;";
    source += "            k = h + 2;";
    source += "        }";
    source += "    }";
    source += "    e = e + 1;";
    source += "    if (i == 2) then {";
    source += "        read e;";
    source += "        if (j != 1) then {";
    source += "            g = a / 3 + e;";
    source += "        } else {";
    source += "            m = k * 10;";
    source += "        }";
    source += "    } else {";
    source += "        a = 5;";
    source += "        while (j < 3) {";
    source += "            g = a + 1;";
    source += "            j = j - 1;";
    source += "        }";
    source += "    }";
    source += "    n = m - 1;";
    source += "    print a;";
    source += "    a = a * b;";
    source += "}";

    source += "procedure Second {";
    source += "    if (c != 1) then {";
    source += "        z = 5 + b;";
    source += "    } else {";
    source += "        c = c + 1;";
    source += "    }";
    source += "    v = z;";
    source += "    print v;";
    source += "}";

    auto base_source = std::make_shared<Parser::Source>(source);
    Parser::SimpleParser parser(base_source);
    auto ast = parser.parse_program();
    return ast;
}

TEST_CASE("Test for next and affects relationship with multi-nested source program") {
    auto ast = build_ast_for_next_and_affects();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_cfg(pkb);
    SimpleExtractor::DesignExtractor::extract_next_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_call_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_affect_relationship(pkb);
}
