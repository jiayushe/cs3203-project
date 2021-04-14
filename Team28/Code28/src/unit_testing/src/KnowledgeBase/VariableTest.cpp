#include "catch.hpp"

#include "KnowledgeBase/Variable.h"
#include <KnowledgeBase/Procedure.h>
#include <string>

using namespace KnowledgeBase;

TEST_CASE("KnowledgeBase::Variable") {
    std::string var_name = "var";
    Variable var(var_name);

    SECTION("get_name") { REQUIRE(var.get_name() == var_name); }

    SECTION("add_get_direct_modified_by") {
        int stmt_id = 10;
        var.add_direct_modified_by(stmt_id);
        auto direct_modified_by_set = var.get_direct_modified_by();
        REQUIRE(direct_modified_by_set->size() == 1);
        REQUIRE(direct_modified_by_set->count(stmt_id) == 1);
        REQUIRE(direct_modified_by_set->count(100) == 0);
    }

    SECTION("add_get_modified_by") {
        int stmt_id = 10;
        var.add_modified_by(stmt_id);
        auto modified_by_set = var.get_modified_by();
        REQUIRE(modified_by_set->size() == 1);
        REQUIRE(modified_by_set->count(stmt_id) == 1);
        REQUIRE(modified_by_set->count(100) == 0);
    }

    SECTION("add_get_direct_used_by") {
        int stmt_id = 10;
        var.add_direct_used_by(stmt_id);
        auto direct_used_by_set = var.get_direct_used_by();
        REQUIRE(direct_used_by_set->size() == 1);
        REQUIRE(direct_used_by_set->count(stmt_id) == 1);
        REQUIRE(direct_used_by_set->count(100) == 0);
    }

    SECTION("add_get_used_by") {
        int stmt_id = 10;
        var.add_used_by(stmt_id);
        auto used_by_set = var.get_used_by();
        REQUIRE(used_by_set->size() == 1);
        REQUIRE(used_by_set->count(stmt_id) == 1);
        REQUIRE(used_by_set->count(100) == 0);
    }

    SECTION("add_get_modified_by_procedures") {
        std::string first_proc_name = "main";
        std::string second_proc_name = "compute";
        Procedure first_proc(first_proc_name);
        Procedure second_proc(second_proc_name);
        var.add_modified_by_procedure(first_proc_name);
        var.add_modified_by_procedure(second_proc_name);
        auto modified_by_set = var.get_modified_by_procedures();
        REQUIRE(modified_by_set->size() == 2);
        REQUIRE(modified_by_set->count(first_proc_name) == 1);
        REQUIRE(modified_by_set->count(second_proc_name) == 1);
        REQUIRE(modified_by_set->count("hello") == 0);
    }

    SECTION("add_get_used_by_procedures") {
        std::string first_proc_name = "main";
        std::string second_proc_name = "compute";
        Procedure first_proc(first_proc_name);
        Procedure second_proc(second_proc_name);
        var.add_used_by_procedure(first_proc_name);
        var.add_used_by_procedure(second_proc_name);
        auto used_by_set = var.get_used_by_procedures();
        REQUIRE(used_by_set->size() == 2);
        REQUIRE(used_by_set->count(first_proc_name) == 1);
        REQUIRE(used_by_set->count(second_proc_name) == 1);
        REQUIRE(used_by_set->count("hello") == 0);
    }
}
