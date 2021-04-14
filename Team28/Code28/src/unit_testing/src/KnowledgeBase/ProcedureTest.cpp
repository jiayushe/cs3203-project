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

    SECTION("add_get_uses") {
        std::string var_name = "v";
        proc.add_uses(var_name);
        auto uses_set = proc.get_uses();
        REQUIRE(uses_set->size() == 1);
        REQUIRE(uses_set->count(var_name) == 1);
        REQUIRE(uses_set->count("a") == 0);
    }

    SECTION("add_get_modifies") {
        std::string var_name = "v";
        proc.add_modifies(var_name);
        auto modifies_set = proc.get_modifies();
        REQUIRE(modifies_set->size() == 1);
        REQUIRE(modifies_set->count(var_name) == 1);
        REQUIRE(modifies_set->count("a") == 0);
    }

    SECTION("add_get_called_by_procedures") {
        std::string callee_name = "compute";
        Procedure callee_proc(callee_name);
        proc.add_direct_callee(callee_name);
        proc.add_callee(callee_name);
        callee_proc.add_direct_caller(proc_name);
        callee_proc.add_caller(proc_name);
        REQUIRE(proc.get_direct_callees()->count(callee_name) == 1);
        REQUIRE(proc.get_callees()->count(callee_name) == 1);
        REQUIRE(proc.get_callees()->count("hello") == 0);
        REQUIRE(callee_proc.get_direct_callers()->count(proc_name) == 1);
        REQUIRE(callee_proc.get_callers()->count(proc_name) == 1);
        REQUIRE(callee_proc.get_callers()->count("hello") == 0);
    }
}
