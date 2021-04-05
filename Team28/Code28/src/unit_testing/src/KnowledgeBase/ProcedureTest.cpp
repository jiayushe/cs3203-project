#include "catch.hpp"

#include "KnowledgeBase/Procedure.h"

using namespace KnowledgeBase;

TEST_CASE("KnowledgeBase::Procedure") {
    std::string proc_name = "main";
    Procedure proc(proc_name);

    SECTION("get_name") { REQUIRE(proc.get_name() == proc_name); }

    SECTION("add_get_statements") {
        int stmt_id = 10;
        proc.add_statement(stmt_id);
        auto statements_set = proc.get_statements();
        REQUIRE(statements_set->size() == 1);
        REQUIRE(statements_set->count(stmt_id) == 1);
        REQUIRE(statements_set->count(100) == 0);
    }

    SECTION("add_get_called_by") {
        int stmt_id = 10;
        proc.add_called_by_statement(stmt_id);
        auto called_by_set = proc.get_called_by_statements();
        REQUIRE(called_by_set->size() == 1);
        REQUIRE(called_by_set->count(stmt_id) == 1);
        REQUIRE(called_by_set->count(100) == 0);
    }
}
