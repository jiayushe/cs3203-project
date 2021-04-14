#include "catch.hpp"

#include "KnowledgeBase/PKB.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include <memory>
#include <string>

using namespace KnowledgeBase;
using namespace Parser;

TEST_CASE("KnowledgeBase::PKB") {
    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    PKB pkb(ast);

    SECTION("get_ast") { REQUIRE(pkb.get_ast()->is_equal(ast)); }

    SECTION("add_get_procedures") {
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        REQUIRE(pkb.get_procedure_by_name(proc_name)->get_name() == proc_name);
        auto proc_list = pkb.get_procedures();
        REQUIRE(proc_list.size() == 1);
        REQUIRE(proc_list[0]->get_name() == proc_name);
    }

    SECTION("add_get_statements") {
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType stmt_type = StatementType::READ;
        int stmt_id = 2;
        auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, stmt_id);
        read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(stmt_type, stmt_id, proc_name, read_stmt);
        REQUIRE(pkb.get_statement_by_id(stmt_id)->get_id() == stmt_id);
        auto stmt_list = pkb.get_statements();
        REQUIRE(stmt_list.size() == 1);
        REQUIRE(stmt_list[0]->get_id() == stmt_id);
    }

    SECTION("add_get_variables") {
        std::string var_name = "x";
        pkb.add_variable(var_name);
        REQUIRE(pkb.get_variable_by_name(var_name)->get_name() == var_name);
        auto var_list = pkb.get_variables();
        REQUIRE(var_list.size() == 1);
        REQUIRE(var_list[0]->get_name() == var_name);
    }

    SECTION("add_get_constants") {
        int val = 10;
        pkb.add_constant(val);
        REQUIRE(pkb.get_constant_by_value(val)->get_value() == val);
        auto cst_list = pkb.get_constants();
        REQUIRE(cst_list.size() == 1);
        REQUIRE(cst_list[0]->get_value() == val);
    }

    SECTION("add_modify_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType read_stmt_type = StatementType::READ;
        int read_stmt_id = 2;
        auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, read_stmt_id);
        std::string var_name = "x";
        read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, var_name));
        pkb.add_variable(var_name);
        pkb.add_statement(read_stmt_type, read_stmt_id, proc_name, read_stmt);
        pkb.add_modify_relationship(read_stmt_id, var_name);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_direct_modifies()->count(var_name) == 1);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_modifies()->count(var_name) == 1);
        REQUIRE(pkb.get_variable_by_name(var_name)->get_direct_modified_by()->count(read_stmt_id) ==
                1);
        REQUIRE(pkb.get_variable_by_name(var_name)->get_modified_by()->count(read_stmt_id) == 1);
    }

    SECTION("add_use_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType print_stmt_type = StatementType::PRINT;
        int print_stmt_id = 3;
        auto print_stmt = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, print_stmt_id);
        std::string var_name = "x";
        print_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, var_name));
        pkb.add_variable(var_name);
        pkb.add_statement(print_stmt_type, print_stmt_id, proc_name, print_stmt);
        pkb.add_use_relationship(print_stmt_id, var_name);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_direct_uses()->count(var_name) == 1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_uses()->count(var_name) == 1);
        REQUIRE(pkb.get_variable_by_name(var_name)->get_direct_used_by()->count(print_stmt_id) ==
                1);
        REQUIRE(pkb.get_variable_by_name(var_name)->get_used_by()->count(print_stmt_id) == 1);
    }

    SECTION("add_follow_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType read_stmt_type = StatementType::READ;
        int read_stmt_id = 2;
        auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, read_stmt_id);
        read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(read_stmt_type, read_stmt_id, proc_name, read_stmt);
        StatementType print_stmt_type = StatementType::PRINT;
        int print_stmt_id = 3;
        auto print_stmt = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, print_stmt_id);
        print_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(print_stmt_type, print_stmt_id, proc_name, print_stmt);
        pkb.add_follow_relationship(read_stmt_id, print_stmt_id);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_direct_follower() == print_stmt_id);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_followers()->count(print_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_direct_following() == read_stmt_id);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_followings()->count(read_stmt_id) == 1);
    }

    SECTION("add_parent_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType while_stmt_type = StatementType::WHILE;
        int while_stmt_id = 5;
        auto while_stmt = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, while_stmt_id);
        auto cond = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
        cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "20"));
        while_stmt->add_child(cond);
        auto while_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
        StatementType call_stmt_type = StatementType::CALL;
        int call_stmt_id = 6;
        auto call_stmt = std::make_shared<SimpleNode>(SimpleNodeType::CALL, call_stmt_id);
        call_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "main"));
        pkb.add_statement(call_stmt_type, call_stmt_id, proc_name, call_stmt);
        while_stmt_lst->add_child(call_stmt);
        while_stmt->add_child(while_stmt_lst);
        pkb.add_statement(while_stmt_type, while_stmt_id, proc_name, while_stmt);
        pkb.add_parent_relationship(while_stmt_id, call_stmt_id);
        REQUIRE(pkb.get_statement_by_id(while_stmt_id)->get_children()->count(call_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(while_stmt_id)->get_descendants()->count(call_stmt_id) ==
                1);
        REQUIRE(pkb.get_statement_by_id(call_stmt_id)->get_parent() == while_stmt_id);
        REQUIRE(pkb.get_statement_by_id(call_stmt_id)->get_ancestors()->count(while_stmt_id) == 1);
    }

    SECTION("add_call_relationship") {
        PKB pkb(ast);
        std::string first_proc_name = "first";
        std::string second_proc_name = "second";
        std::string third_proc_name = "third";
        pkb.add_procedure(first_proc_name);
        pkb.add_procedure(second_proc_name);
        pkb.add_procedure(third_proc_name);

        pkb.add_call_relationship(first_proc_name, second_proc_name);
        pkb.add_call_relationship(second_proc_name, third_proc_name);
        REQUIRE(pkb.get_procedure_by_name(first_proc_name)
                    ->get_direct_callees()
                    ->count(second_proc_name) == 1);
        REQUIRE(
            pkb.get_procedure_by_name(first_proc_name)->get_callees()->count(second_proc_name) ==
            1);
        REQUIRE(pkb.get_procedure_by_name(second_proc_name)
                    ->get_direct_callers()
                    ->count(first_proc_name) == 1);
        REQUIRE(
            pkb.get_procedure_by_name(second_proc_name)->get_callers()->count(first_proc_name) ==
            1);
        REQUIRE(pkb.get_procedure_by_name(second_proc_name)
                    ->get_direct_callees()
                    ->count(third_proc_name) == 1);
        REQUIRE(
            pkb.get_procedure_by_name(second_proc_name)->get_callees()->count(third_proc_name) ==
            1);
        REQUIRE(pkb.get_procedure_by_name(third_proc_name)
                    ->get_direct_callers()
                    ->count(second_proc_name) == 1);
        REQUIRE(
            pkb.get_procedure_by_name(third_proc_name)->get_callers()->count(second_proc_name) ==
            1);

        REQUIRE(pkb.get_procedure_by_name(first_proc_name)
                    ->get_direct_callees()
                    ->count(third_proc_name) == 0);
        REQUIRE(pkb.get_procedure_by_name(first_proc_name)->get_callees()->count(third_proc_name) ==
                1);
        REQUIRE(pkb.get_procedure_by_name(third_proc_name)
                    ->get_direct_callers()
                    ->count(first_proc_name) == 0);
        REQUIRE(pkb.get_procedure_by_name(third_proc_name)->get_callers()->count(first_proc_name) ==
                1);
    }

    SECTION("add_next_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType read_stmt_type = StatementType::READ;
        int read_stmt_id = 2;
        auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, read_stmt_id);
        read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(read_stmt_type, read_stmt_id, proc_name, read_stmt);
        StatementType print_stmt_type = StatementType::PRINT;
        int print_stmt_id = 3;
        auto print_stmt = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, print_stmt_id);
        print_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(print_stmt_type, print_stmt_id, proc_name, print_stmt);
        pkb.add_next_relationship(read_stmt_id, print_stmt_id);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_direct_next()->count(print_stmt_id) ==
                1);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_next()->count(print_stmt_id) == 1);
        REQUIRE(
            pkb.get_statement_by_id(print_stmt_id)->get_direct_previous()->count(read_stmt_id) ==
            1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_previous()->count(read_stmt_id) == 1);
    }

    SECTION("add_next_bip_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        StatementType read_stmt_type = StatementType::READ;
        int read_stmt_id = 2;
        auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, read_stmt_id);
        read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(read_stmt_type, read_stmt_id, proc_name, read_stmt);
        StatementType print_stmt_type = StatementType::PRINT;
        StatementType while_stmt_type = StatementType::WHILE;
        int while_stmt_id = 3;
        auto while_stmt = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, while_stmt_id);
        auto cond = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
        cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "20"));
        while_stmt->add_child(cond);
        auto while_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
        int print_stmt_id = 4;
        auto print_stmt = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, print_stmt_id);
        print_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
        pkb.add_statement(print_stmt_type, print_stmt_id, proc_name, print_stmt);
        while_stmt_lst->add_child(print_stmt);
        while_stmt->add_child(while_stmt_lst);
        pkb.add_statement(while_stmt_type, while_stmt_id, proc_name, while_stmt);

        pkb.add_direct_next_bip_relationship(read_stmt_id, while_stmt_id);
        REQUIRE(
            pkb.get_statement_by_id(read_stmt_id)->get_direct_next_bip()->count(while_stmt_id) ==
            1);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_next_bip()->count(while_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(while_stmt_id)
                    ->get_direct_previous_bip()
                    ->count(read_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(while_stmt_id)->get_previous_bip()->count(read_stmt_id) ==
                1);

        pkb.add_direct_next_bip_relationship(while_stmt_id, print_stmt_id);
        REQUIRE(
            pkb.get_statement_by_id(while_stmt_id)->get_direct_next_bip()->count(print_stmt_id) ==
            1);
        REQUIRE(pkb.get_statement_by_id(while_stmt_id)->get_next_bip()->count(print_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)
                    ->get_direct_previous_bip()
                    ->count(while_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_previous_bip()->count(while_stmt_id) ==
                1);

        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_next_bip()->count(print_stmt_id) == 0);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_previous_bip()->count(read_stmt_id) ==
                0);
        pkb.add_indirect_next_bip_relationship(read_stmt_id, print_stmt_id);
        REQUIRE(pkb.get_statement_by_id(read_stmt_id)->get_next_bip()->count(print_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(print_stmt_id)->get_previous_bip()->count(read_stmt_id) ==
                1);
    }

    SECTION("add_affect_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        int first_assign_stmt_id = 1;
        int second_assign_stmt_id = 2;
        int third_assign_stmt_id = 3;
        auto first_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, first_assign_stmt_id);
        first_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
        first_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
        auto second_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, second_assign_stmt_id);
        second_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
        second_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
        auto third_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, third_assign_stmt_id);
        third_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "d"));
        third_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
        pkb.add_statement(StatementType::ASSIGN, first_assign_stmt_id, proc_name,
                          first_assign_stmt);
        pkb.add_statement(StatementType::ASSIGN, second_assign_stmt_id, proc_name,
                          second_assign_stmt);
        pkb.add_statement(StatementType::ASSIGN, third_assign_stmt_id, proc_name,
                          third_assign_stmt);

        pkb.add_affect_relationship(first_assign_stmt_id, second_assign_stmt_id);
        pkb.add_affect_relationship(second_assign_stmt_id, third_assign_stmt_id);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_direct_affects()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_affects()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_direct_affected_by()
                    ->count(first_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_affected_by()
                    ->count(first_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_direct_affects()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_affects()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_direct_affected_by()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_affected_by()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_affects()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_affected_by()
                    ->count(first_assign_stmt_id) == 1);
    }

    SECTION("add_affect_bip_relationship") {
        PKB pkb(ast);
        std::string proc_name = "main";
        pkb.add_procedure(proc_name);
        int first_assign_stmt_id = 1;
        int second_assign_stmt_id = 2;
        int third_assign_stmt_id = 3;
        auto first_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, first_assign_stmt_id);
        first_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
        first_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
        auto second_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, second_assign_stmt_id);
        second_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
        second_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
        auto third_assign_stmt =
            std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, third_assign_stmt_id);
        third_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "d"));
        third_assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
        pkb.add_statement(StatementType::ASSIGN, first_assign_stmt_id, proc_name,
                          first_assign_stmt);
        pkb.add_statement(StatementType::ASSIGN, second_assign_stmt_id, proc_name,
                          second_assign_stmt);
        pkb.add_statement(StatementType::ASSIGN, third_assign_stmt_id, proc_name,
                          third_assign_stmt);

        pkb.add_direct_affect_bip_relationship(first_assign_stmt_id, second_assign_stmt_id);
        pkb.add_direct_affect_bip_relationship(second_assign_stmt_id, third_assign_stmt_id);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_direct_affects_bip()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_affects_bip()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_direct_affected_bip_by()
                    ->count(first_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_affected_bip_by()
                    ->count(first_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_direct_affects_bip()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(second_assign_stmt_id)
                    ->get_affects_bip()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_direct_affected_bip_by()
                    ->count(second_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_affected_bip_by()
                    ->count(second_assign_stmt_id) == 1);

        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_affects_bip()
                    ->count(third_assign_stmt_id) == 0);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_affected_bip_by()
                    ->count(first_assign_stmt_id) == 0);
        pkb.add_indirect_affect_bip_relationship(first_assign_stmt_id, third_assign_stmt_id);
        REQUIRE(pkb.get_statement_by_id(first_assign_stmt_id)
                    ->get_affects_bip()
                    ->count(third_assign_stmt_id) == 1);
        REQUIRE(pkb.get_statement_by_id(third_assign_stmt_id)
                    ->get_affected_bip_by()
                    ->count(first_assign_stmt_id) == 1);
    }
}
