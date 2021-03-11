#include "catch.hpp"

#include "KnowledgeBase/PKB.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "SimpleExtractor/DesignExtractor.h"
#include <memory>
#include <string>

using namespace KnowledgeBase;
using namespace Parser;

std::shared_ptr<SimpleNode> build_ast() {
    // PROGRAM
    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    // PROCEDURE
    auto proc_other = std::make_shared<SimpleNode>(SimpleNodeType::PROCEDURE);
    proc_other->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "other"));
    proc_other->add_child(std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST));
    ast->add_child(proc_other);
    // PROCEDURE
    auto proc_main = std::make_shared<SimpleNode>(SimpleNodeType::PROCEDURE);
    proc_main->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "main"));
    auto stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    // READ
    auto read_stmt = std::make_shared<SimpleNode>(SimpleNodeType::READ, 1);
    read_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    // CALL
    auto call_stmt = std::make_shared<SimpleNode>(SimpleNodeType::CALL, 2);
    call_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "other"));
    // PRINT
    auto print_stmt = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 3);
    print_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
    // WHILE
    auto while_stmt = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, 4);
    auto cond = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
    cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
    cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "0"));
    while_stmt->add_child(cond);
    auto while_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    // ASSIGN
    auto assign_stmt = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 5);
    assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "d"));
    auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "e"));
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
    assign_stmt->add_child(assign_rhs);
    while_stmt_lst->add_child(assign_stmt);
    while_stmt->add_child(while_stmt_lst);
    // IF
    auto if_stmt = std::make_shared<SimpleNode>(SimpleNodeType::IF, 6);
    auto cond2 = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
    cond2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "f"));
    cond2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "20"));
    if_stmt->add_child(cond2);
    // THEN
    auto if_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    // READ
    auto read_stmt2 = std::make_shared<SimpleNode>(SimpleNodeType::READ, 7);
    read_stmt2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "g"));
    if_stmt_lst->add_child(read_stmt2);
    // ELSE
    auto if_stmt_lst2 = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    // PRINT
    auto print_stmt2 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 8);
    print_stmt2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "h"));
    if_stmt_lst2->add_child(print_stmt2);
    if_stmt->add_child(if_stmt_lst);
    if_stmt->add_child(if_stmt_lst2);
    stmt_lst->add_child(read_stmt);
    stmt_lst->add_child(call_stmt);
    stmt_lst->add_child(print_stmt);
    stmt_lst->add_child(while_stmt);
    stmt_lst->add_child(if_stmt);
    proc_main->add_child(stmt_lst);
    ast->add_child(proc_main);
    return ast;
}

std::shared_ptr<SimpleNode> build_nested_ast() {
    // PROGRAM
    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);

    // PROCEDURE
    auto proc_main = std::make_shared<SimpleNode>(SimpleNodeType::PROCEDURE);
    proc_main->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "main"));
    auto stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);

    // ASSIGN
    auto assign_stmt_1 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 1);
    assign_stmt_1->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    auto assign_rhs_1 = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "0");
    assign_stmt_1->add_child(assign_rhs_1);

    auto assign_stmt_2 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 2);
    assign_stmt_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
    auto assign_rhs_2 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    assign_rhs_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "1"));
    assign_stmt_2->add_child(assign_rhs_2);

    auto assign_stmt_4 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 4);
    assign_stmt_4->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    auto assign_rhs_4 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs_4->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
    assign_rhs_4->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "5"));
    assign_stmt_4->add_child(assign_rhs_4);

    auto assign_stmt_8 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 8);
    assign_stmt_8->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "d"));
    auto assign_rhs_8 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
    assign_rhs_8->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "2"));
    assign_rhs_8->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "2"));
    assign_stmt_8->add_child(assign_rhs_8);

    auto assign_stmt_9 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 9);
    assign_stmt_9->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    auto assign_rhs_9 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs_9->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));
    assign_rhs_9->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "b"));
    assign_stmt_9->add_child(assign_rhs_9);

    // READ
    auto read_stmt_6 = std::make_shared<SimpleNode>(SimpleNodeType::READ, 6);
    read_stmt_6->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));

    // PRINT
    auto print_stmt_5 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 5);
    print_stmt_5->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "a"));

    auto print_stmt_10 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 10);
    print_stmt_10->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "e"));

    // WHILE
    auto while_stmt_3 = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, 3);
    auto cond_3 = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "!=");
    cond_3->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
    cond_3->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "1"));
    while_stmt_3->add_child(cond_3);
    auto while_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    while_stmt_3->add_child(while_stmt_lst);

    // IF
    auto if_stmt_7 = std::make_shared<SimpleNode>(SimpleNodeType::IF, 7);
    auto cond_7 = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
    cond_7->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "c"));
    cond_7->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "2"));
    if_stmt_7->add_child(cond_7);
    // THEN
    auto then_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    then_stmt_lst->add_child(assign_stmt_8);
    then_stmt_lst->add_child(assign_stmt_9);
    // ELSE
    auto else_stmt_lst = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    else_stmt_lst->add_child(print_stmt_10);

    if_stmt_7->add_child(then_stmt_lst);
    if_stmt_7->add_child(else_stmt_lst);

    // Populates while loop
    while_stmt_lst->add_child(assign_stmt_4);
    while_stmt_lst->add_child(print_stmt_5);
    while_stmt_lst->add_child(read_stmt_6);
    while_stmt_lst->add_child(if_stmt_7);

    // Populates top-level statements to procedure
    stmt_lst->add_child(assign_stmt_1);
    stmt_lst->add_child(assign_stmt_2);
    stmt_lst->add_child(while_stmt_3);

    proc_main->add_child(stmt_lst);
    ast->add_child(proc_main);
    return ast;
}

TEST_CASE("DEPKBIntegration") {
    auto ast = build_ast();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    REQUIRE(pkb->get_procedures().size() == 2);
    REQUIRE(pkb->get_statements().size() == 8);
    REQUIRE(pkb->get_variables().size() == 8);
    REQUIRE(pkb->get_constants().size() == 3);
    // Call
    REQUIRE(pkb->get_procedure_by_name("main")->get_statements().size() == 8);
    REQUIRE(pkb->get_procedure_by_name("other")->get_called_by_statements().size() == 1);
    REQUIRE(pkb->get_procedure_by_name("other")->get_called_by_statements().count(2) == 1);
    REQUIRE(pkb->get_statement_by_id(2)->get_procedure_called() == "other");
    // Uses
    REQUIRE(pkb->get_statement_by_id(3)->get_direct_uses().size() == 1);
    REQUIRE(pkb->get_statement_by_id(3)->get_direct_uses().count("b") == 1);
    REQUIRE(pkb->get_statement_by_id(3)->get_uses().size() == 1);
    REQUIRE(pkb->get_statement_by_id(3)->get_uses().count("b") == 1);
    REQUIRE(pkb->get_variable_by_name("b")->get_direct_used_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("b")->get_direct_used_by().count(3) == 1);
    REQUIRE(pkb->get_variable_by_name("b")->get_used_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("b")->get_used_by().count(3) == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_direct_uses().size() == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_direct_uses().count("c") == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_uses().size() == 2);
    REQUIRE(pkb->get_statement_by_id(4)->get_uses().count("c") == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_uses().count("e") == 1);
    REQUIRE(pkb->get_statement_by_id(5)->get_uses().size() == 1);
    REQUIRE(pkb->get_statement_by_id(5)->get_uses().count("e") == 1);
    REQUIRE(pkb->get_variable_by_name("c")->get_direct_used_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("c")->get_direct_used_by().count(4) == 1);
    REQUIRE(pkb->get_variable_by_name("c")->get_used_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("c")->get_used_by().count(4) == 1);
    REQUIRE(pkb->get_variable_by_name("e")->get_direct_used_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("e")->get_direct_used_by().count(5) == 1);
    REQUIRE(pkb->get_variable_by_name("e")->get_used_by().size() == 2);
    REQUIRE(pkb->get_variable_by_name("e")->get_used_by().count(4) == 1);
    REQUIRE(pkb->get_variable_by_name("e")->get_used_by().count(5) == 1);
    // Modifies
    REQUIRE(pkb->get_statement_by_id(1)->get_direct_modifies().size() == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_direct_modifies().count("a") == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_modifies().size() == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_modifies().count("a") == 1);
    REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().count(1) == 1);
    REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(1) == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_direct_modifies().size() == 0);
    REQUIRE(pkb->get_statement_by_id(4)->get_direct_modifies().count("d") == 0);
    REQUIRE(pkb->get_statement_by_id(4)->get_modifies().size() == 1);
    REQUIRE(pkb->get_statement_by_id(4)->get_modifies().count("d") == 1);
    REQUIRE(pkb->get_statement_by_id(5)->get_modifies().size() == 1);
    REQUIRE(pkb->get_statement_by_id(5)->get_modifies().count("d") == 1);
    REQUIRE(pkb->get_variable_by_name("d")->get_direct_modified_by().size() == 1);
    REQUIRE(pkb->get_variable_by_name("d")->get_direct_modified_by().count(5) == 1);
    REQUIRE(pkb->get_variable_by_name("d")->get_modified_by().size() == 2);
    REQUIRE(pkb->get_variable_by_name("d")->get_modified_by().count(4) == 1);
    REQUIRE(pkb->get_variable_by_name("d")->get_modified_by().count(5) == 1);
    // Follows
    REQUIRE(pkb->get_statement_by_id(1)->get_direct_follower() == 2);
    REQUIRE(pkb->get_statement_by_id(1)->get_followers().size() == 4);
    REQUIRE(pkb->get_statement_by_id(1)->get_followers().count(2) == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_followers().count(3) == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_followers().count(4) == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_followers().count(6) == 1);
    REQUIRE(pkb->get_statement_by_id(1)->get_direct_following() == -1);
    REQUIRE(pkb->get_statement_by_id(1)->get_followings().size() == 0);
    // Parent
    REQUIRE(pkb->get_statement_by_id(6)->get_children().size() == 2);
    REQUIRE(pkb->get_statement_by_id(6)->get_children().count(7) == 1);
    REQUIRE(pkb->get_statement_by_id(6)->get_children().count(8) == 1);
    REQUIRE(pkb->get_statement_by_id(6)->get_descendants().size() == 2);
    REQUIRE(pkb->get_statement_by_id(6)->get_descendants().count(7) == 1);
    REQUIRE(pkb->get_statement_by_id(6)->get_descendants().count(8) == 1);
    REQUIRE(pkb->get_statement_by_id(8)->get_parent() == 6);
    REQUIRE(pkb->get_statement_by_id(8)->get_ancestors().size() == 1);
    REQUIRE(pkb->get_statement_by_id(8)->get_ancestors().count(6) == 1);
}

TEST_CASE("DEPKBIntegration_Nested_AST") {
    auto ast = build_nested_ast();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    REQUIRE(pkb->get_procedures().size() == 1);
    REQUIRE(pkb->get_statements().size() == 10);
    REQUIRE(pkb->get_variables().size() == 5);
    REQUIRE(pkb->get_constants().size() == 4);

    SECTION("Follows") {
        SECTION("inside while stmt") {
            REQUIRE(pkb->get_statement_by_id(4)->get_direct_follower() == 5);
            REQUIRE(pkb->get_statement_by_id(4)->get_followers().size() == 3);
            REQUIRE(pkb->get_statement_by_id(4)->get_followers().count(5) == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_followers().count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(4)->get_followers().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(5)->get_direct_following() == 4);
            REQUIRE(pkb->get_statement_by_id(5)->get_followings().size() == 1);
        }

        SECTION("inside if stmt") {
            REQUIRE(pkb->get_statement_by_id(8)->get_direct_follower() == 9);
            REQUIRE(pkb->get_statement_by_id(8)->get_followers().size() == 1);
            REQUIRE(pkb->get_statement_by_id(8)->get_followers().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_direct_following() == 8);
            REQUIRE(pkb->get_statement_by_id(9)->get_followings().size() == 1);
        }

        SECTION("negative edge cases") {
            SECTION("first of stmt list") {
                REQUIRE(pkb->get_statement_by_id(1)->get_direct_following() == -1);
                REQUIRE(pkb->get_statement_by_id(1)->get_followings().size() == 0);
                REQUIRE(pkb->get_statement_by_id(4)->get_direct_following() == -1);
                REQUIRE(pkb->get_statement_by_id(4)->get_followings().size() == 0);
                REQUIRE(pkb->get_statement_by_id(8)->get_direct_following() == -1);
                REQUIRE(pkb->get_statement_by_id(8)->get_followings().size() == 0);
            }

            SECTION("end of stmt list") {
                REQUIRE(pkb->get_statement_by_id(7)->get_direct_follower() == -1);
                REQUIRE(pkb->get_statement_by_id(7)->get_followers().size() == 0);
                REQUIRE(pkb->get_statement_by_id(9)->get_direct_follower() == -1);
                REQUIRE(pkb->get_statement_by_id(9)->get_followers().size() == 0);
                REQUIRE(pkb->get_statement_by_id(10)->get_direct_follower() == -1);
                REQUIRE(pkb->get_statement_by_id(10)->get_followers().size() == 0);
            }

            SECTION("cross-level stmts") {
                REQUIRE(pkb->get_statement_by_id(5)->get_followers().count(9) == 0);
                REQUIRE(pkb->get_statement_by_id(9)->get_followings().count(5) == 0);
            }
        }
    }

    SECTION("Parent") {
        SECTION("while stmt") {
            REQUIRE(pkb->get_statement_by_id(3)->get_children().size() == 4);
            REQUIRE(pkb->get_statement_by_id(3)->get_children().count(4) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_children().count(5) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_children().count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_children().count(7) == 1);

            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().size() == 7);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(4) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(5) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(6) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(7) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_descendants().count(10) == 1);
        }

        SECTION("if stmt") {
            REQUIRE(pkb->get_statement_by_id(7)->get_children().size() == 3);
            REQUIRE(pkb->get_statement_by_id(7)->get_children().count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(7)->get_children().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(7)->get_children().count(10) == 1);

            REQUIRE(pkb->get_statement_by_id(7)->get_descendants().size() == 3);
            REQUIRE(pkb->get_statement_by_id(7)->get_descendants().count(8) == 1);
            REQUIRE(pkb->get_statement_by_id(7)->get_descendants().count(9) == 1);
            REQUIRE(pkb->get_statement_by_id(7)->get_descendants().count(10) == 1);

            REQUIRE(pkb->get_statement_by_id(9)->get_parent() == 7);
            REQUIRE(pkb->get_statement_by_id(9)->get_ancestors().size() == 2);
            REQUIRE(pkb->get_statement_by_id(9)->get_ancestors().count(3) == 1);
            REQUIRE(pkb->get_statement_by_id(9)->get_ancestors().count(7) == 1);
        }

        SECTION("non if/while stmt") {
            REQUIRE(pkb->get_statement_by_id(6)->get_children().size() == 0);
            REQUIRE(pkb->get_statement_by_id(6)->get_descendants().size() == 0);
        }
    }

    SECTION("Uses with if/while stmt") {
        SECTION("Uses by statement") {
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_uses().size() == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_uses().count("c") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_uses().size() == 4);
            REQUIRE(pkb->get_statement_by_id(3)->get_uses().count("a") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_uses().count("b") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_uses().count("c") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_uses().count("e") == 1);
        }

        SECTION("Uses on variable") {
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_used_by().size() == 3);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_used_by().count(2) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_used_by().count(5) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_used_by().count(9) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().size() == 5);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().count(2) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().count(3) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().count(5) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().count(7) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_used_by().count(9) == 1);
        }
    }

    SECTION("Modifies with if/while stmt") {
        SECTION("Modifies by statement") {
            REQUIRE(pkb->get_statement_by_id(3)->get_direct_modifies().size() == 0);
            REQUIRE(pkb->get_statement_by_id(3)->get_modifies().size() == 3);
            REQUIRE(pkb->get_statement_by_id(3)->get_modifies().count("a") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_modifies().count("c") == 1);
            REQUIRE(pkb->get_statement_by_id(3)->get_modifies().count("d") == 1);
        }

        SECTION("Modifies on variable") {
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().size() == 3);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().count(1) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().count(4) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_direct_modified_by().count(9) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().size() == 5);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(1) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(3) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(4) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(7) == 1);
            REQUIRE(pkb->get_variable_by_name("a")->get_modified_by().count(9) == 1);
        }
    }
}
