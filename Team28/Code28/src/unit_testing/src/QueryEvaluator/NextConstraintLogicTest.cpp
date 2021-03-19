#include "QueryEvaluator/constraints/NextConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/NextTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("NextConstraintLogic and NextTConstraintLogic") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);

    // Any statement ref
    auto statement_any = Parser::StatementRef();
    statement_any.set_type(Parser::StatementRefType::ANY);
    // Synonym statement ref
    auto statement_synonym = Parser::StatementRef();
    statement_synonym.set_type(Parser::StatementRefType::SYNONYM);
    auto statement_synonym_dup = Parser::StatementRef();
    statement_synonym_dup.set_type(Parser::StatementRefType::SYNONYM);
    // Statement ID statement ref
    auto statement_id = Parser::StatementRef();
    statement_id.set_type(Parser::StatementRefType::STATEMENT_ID);
    auto statement_id_dup = Parser::StatementRef();
    statement_id_dup.set_type(Parser::StatementRefType::STATEMENT_ID);

    // procedure main {
    // 1   print;
    // 2   while () {
    // 3      read;
    // 4      call procedure;}
    // 5   if () then {
    // 6      y = y + 10; }
    //    else {
    // 7      read;}
    // 8   print;}

    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 1);
    auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 2);
    auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 3);
    auto call_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::CALL, 4);
    call_stmt_node->add_child(
        std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "logic")); // main calls logic

    pkb->add_procedure("main");
    pkb->add_statement(StatementType::PRINT, 1, "main", print_stmt_node);
    pkb->add_statement(StatementType::WHILE, 2, "main", while_stmt_node);
    pkb->add_statement(StatementType::READ, 3, "main", read_stmt_node);
    pkb->add_statement(StatementType::CALL, 4, "main", call_stmt_node);
    pkb->add_next_relationship(1, 2);
    pkb->add_next_relationship(2, 3);
    pkb->add_next_relationship(3, 4);

    auto if_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::IF, 5);
    auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 6);
    assign_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    assign_stmt_node->add_child(assign_rhs);
    auto read_stmt_node2 = std::make_shared<SimpleNode>(SimpleNodeType::READ, 7);

    pkb->add_statement(StatementType::IF, 5, "main", if_stmt_node);
    pkb->add_statement(StatementType::ASSIGN, 6, "main", assign_stmt_node);
    pkb->add_statement(StatementType::READ, 7, "main", read_stmt_node2);
    pkb->add_next_relationship(2, 5);
    pkb->add_next_relationship(5, 6);
    pkb->add_next_relationship(5, 7);

    auto print_stmt_node2 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 8);
    pkb->add_statement(StatementType::PRINT, 8, "main", print_stmt_node2);
    pkb->add_next_relationship(6, 8);
    pkb->add_next_relationship(7, 8);

    SECTION("both any") {
        auto next_constraint = NextConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        auto pkb2 = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb2->add_procedure("main");
        next_constraint = NextConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        auto nextT_constraint = NextTConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(nextT_constraint.is_valid(assign_mappings));

        nextT_constraint = NextTConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(nextT_constraint.is_valid(assign_mappings));
    }

    SECTION("rhs any") {
        statement_id.set_statement_id(1);
        auto next_constraint = NextConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        auto nextT_constraint = NextTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(nextT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(8);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        nextT_constraint = NextTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(nextT_constraint.is_valid(assign_mappings));

        assign_mappings["pr"] = Assignment(1);
        statement_synonym.set_synonym("pr");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        nextT_constraint = NextTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(nextT_constraint.is_valid(assign_mappings));

        assign_mappings["pr"] = Assignment(8);
        statement_synonym.set_synonym("pr");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        nextT_constraint = NextTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(nextT_constraint.is_valid(assign_mappings));
    }

    SECTION("lhs_any") {
        statement_id.set_statement_id(2);
        auto next_constraint = NextConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        next_constraint = NextConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        assign_mappings["w"] = Assignment(2);
        statement_synonym.set_synonym("w");
        next_constraint = NextConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["pr"] = Assignment(1);
        statement_synonym.set_synonym("pr");
        next_constraint = NextConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));
    }

    SECTION("both statement id") {
        statement_id.set_statement_id(1);
        statement_id_dup.set_statement_id(2);
        auto next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(2);
        statement_id_dup.set_statement_id(3);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id_dup.set_statement_id(4);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(2);
        statement_id_dup.set_statement_id(5);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(5);
        statement_id_dup.set_statement_id(6);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(5);
        statement_id_dup.set_statement_id(7);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(6);
        statement_id_dup.set_statement_id(8);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(7);
        statement_id_dup.set_statement_id(8);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(4);
        statement_id_dup.set_statement_id(3);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(5);
        statement_id_dup.set_statement_id(8);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(6);
        statement_id_dup.set_statement_id(7);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(4);
        statement_id_dup.set_statement_id(2);
        next_constraint = NextConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        SECTION("while stmt indirect next") {
            pkb->add_next_relationship(4, 2);
            statement_id.set_statement_id(3);
            statement_id_dup.set_statement_id(2);
            auto nextT_constraint = NextTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(nextT_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(4);
            statement_id_dup.set_statement_id(2);
            nextT_constraint = NextTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(nextT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("both synonym") {
        assign_mappings["pr"] = Assignment(1);
        assign_mappings["w"] = Assignment(2);
        statement_synonym.set_synonym("pr");
        statement_synonym_dup.set_synonym("w");
        auto next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["w"] = Assignment(2);
        assign_mappings["r"] = Assignment(3);
        statement_synonym.set_synonym("w");
        statement_synonym_dup.set_synonym("r");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["c"] = Assignment(4);
        statement_synonym_dup.set_synonym("c");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        assign_mappings["w"] = Assignment(2);
        assign_mappings["ifs"] = Assignment(5);
        statement_synonym.set_synonym("w");
        statement_synonym_dup.set_synonym("ifs");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["ifs"] = Assignment(5);
        assign_mappings["a"] = Assignment(6);
        statement_synonym.set_synonym("ifs");
        statement_synonym_dup.set_synonym("a");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["ifs"] = Assignment(5);
        assign_mappings["r"] = Assignment(7);
        statement_synonym.set_synonym("ifs");
        statement_synonym_dup.set_synonym("r");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["a"] = Assignment(6);
        assign_mappings["pr"] = Assignment(8);
        statement_synonym.set_synonym("a");
        statement_synonym_dup.set_synonym("pr");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["r"] = Assignment(7);
        assign_mappings["pr"] = Assignment(8);
        statement_synonym.set_synonym("r");
        statement_synonym_dup.set_synonym("pr");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(next_constraint.is_valid(assign_mappings));

        assign_mappings["c"] = Assignment(4);
        assign_mappings["r"] = Assignment(3);
        statement_synonym.set_synonym("c");
        statement_synonym_dup.set_synonym("r");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        assign_mappings["ifs"] = Assignment(5);
        assign_mappings["p"] = Assignment(8);
        statement_synonym.set_synonym("ifs");
        statement_synonym_dup.set_synonym("p");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        assign_mappings["a"] = Assignment(6);
        assign_mappings["r"] = Assignment(7);
        statement_synonym.set_synonym("a");
        statement_synonym_dup.set_synonym("r");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        assign_mappings["r"] = Assignment(3);
        assign_mappings["w"] = Assignment(2);
        statement_synonym.set_synonym("r");
        statement_synonym_dup.set_synonym("w");
        next_constraint = NextConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(next_constraint.is_valid(assign_mappings));

        SECTION("while stmt indirect next") {
            pkb->add_next_relationship(3, 2);
            pkb->add_next_relationship(4, 2);
            auto nextT_constraint =
                NextTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(nextT_constraint.is_valid(assign_mappings));

            assign_mappings["c"] = Assignment(4);
            statement_synonym.set_synonym("c");
            nextT_constraint = NextTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(nextT_constraint.is_valid(assign_mappings));
        }
    }
}
