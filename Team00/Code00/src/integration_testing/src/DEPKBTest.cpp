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
    REQUIRE(pkb->get_procedure_by_name("other")->get_called_by().size() == 1);
    REQUIRE(pkb->get_procedure_by_name("other")->get_called_by().count(2) == 1);
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
