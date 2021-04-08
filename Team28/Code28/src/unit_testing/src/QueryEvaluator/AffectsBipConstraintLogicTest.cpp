#include "QueryEvaluator/constraints/AffectsBipConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/AffectsBipTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("AffectsBipConstraintLogic and AffectsBipTConstraintLogic") {
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

    // procedure Main {
    // 1   x = 0;
    // 2   if () {
    // 3      call Logic;
    //     else {
    // 4      y = 2;}}

    // procedure Logic {
    // 5   z = x;
    // 6   call Evaluator;
    // 7   y = z;

    // procedure Evaluator {
    // 8   if () then {
    // 9      y = z;}
    //     else {
    // 10     x = x * z;}}

    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    auto assign_stmt_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 1);
    assign_stmt_node_1->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
    auto assign_rhs_1 = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, 0);
    assign_stmt_node_1->add_child(assign_rhs_1);
    auto if_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::IF, 2);
    auto call_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::CALL, 3);
    call_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "Logic"));
    auto assign_stmt_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 4);
    assign_stmt_node_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_2 = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, 2);
    assign_stmt_node_2->add_child(assign_rhs_2);

    auto assign_stmt_node_3 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 5);
    assign_stmt_node_3->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z"));
    auto assign_rhs_3 = std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x");
    assign_stmt_node_3->add_child(assign_rhs_3);
    auto call_stmt_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::CALL, 6);
    call_stmt_node_2->add_child(
        std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "Evaluator"));
    auto assign_stmt_node_4 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 7);
    assign_stmt_node_4->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_4 = std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z");
    assign_stmt_node_4->add_child(assign_rhs_4);

    auto if_stmt_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::IF, 8);
    auto assign_stmt_node_5 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 9);
    assign_stmt_node_5->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_5 = std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z");
    assign_stmt_node_5->add_child(assign_rhs_5);
    auto assign_stmt_node_6 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 10);
    assign_stmt_node_6->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
    auto assign_rhs_6 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
    assign_rhs_6->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
    assign_rhs_6->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z"));
    assign_stmt_node_6->add_child(assign_rhs_6);

    pkb->add_procedure("Main");
    pkb->add_procedure("Logic");
    pkb->add_procedure("Evaluator");
    pkb->add_statement(StatementType::ASSIGN, 1, "Main", assign_stmt_node_1);
    pkb->add_statement(StatementType::IF, 2, "Main", if_stmt_node);
    pkb->add_statement(StatementType::CALL, 3, "Main", call_stmt_node);
    pkb->add_statement(StatementType::ASSIGN, 4, "Main", assign_stmt_node_2);
    pkb->add_statement(StatementType::ASSIGN, 5, "Logic", assign_stmt_node_3);
    pkb->add_statement(StatementType::CALL, 6, "Logic", call_stmt_node_2);
    pkb->add_statement(StatementType::ASSIGN, 7, "Logic", assign_stmt_node_4);
    pkb->add_statement(StatementType::IF, 8, "Evaluator", if_stmt_node_2);
    pkb->add_statement(StatementType::ASSIGN, 9, "Evaluator", assign_stmt_node_5);
    pkb->add_statement(StatementType::ASSIGN, 10, "Evaluator", assign_stmt_node_6);
    pkb->add_direct_affect_bip_relationship(1, 5);
    pkb->add_direct_affect_bip_relationship(5, 7);
    pkb->add_direct_affect_bip_relationship(1, 10);
    pkb->add_indirect_affect_bip_relationship(1, 5);
    pkb->add_indirect_affect_bip_relationship(1, 7);
    pkb->add_indirect_affect_bip_relationship(1, 10);
    pkb->add_indirect_affect_bip_relationship(5, 7);

    SECTION("both any") {
        auto affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        auto pkb2 = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb2->add_procedure("Main");
        affectsBip_constraint = AffectsBipConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        auto affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));
    }

    SECTION("rhs any") {
        statement_id.set_statement_id(1);
        auto affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        auto affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(10);
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));

        assign_mappings["a1"] = Assignment(1);
        statement_synonym.set_synonym("a1");
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(10);
        statement_synonym.set_synonym("a2");
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));
    }

    SECTION("lhs_any") {
        statement_id.set_statement_id(5);
        auto affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        auto affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(5);
        statement_synonym.set_synonym("a2");
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        assign_mappings["a1"] = Assignment(1);
        statement_synonym.set_synonym("a1");
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));
    }

    SECTION("both statement id") {
        statement_id.set_statement_id(1);
        statement_id_dup.set_statement_id(5);
        auto affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        auto affectsBipT_constraint =
            AffectsBipTConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        statement_id_dup.set_statement_id(7);
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        statement_id_dup.set_statement_id(10);
        affectsBip_constraint = AffectsBipConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint = AffectsBipTConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        SECTION("indirect affectsBip") {
            statement_id.set_statement_id(1);
            statement_id_dup.set_statement_id(7);
            affectsBipT_constraint =
                AffectsBipTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

            affectsBipT_constraint =
                AffectsBipTConstraintLogic(pkb, statement_id_dup, statement_id);
            REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("both synonym") {
        assign_mappings["a1"] = Assignment(1);
        assign_mappings["a2"] = Assignment(5);
        statement_synonym.set_synonym("a1");
        statement_synonym_dup.set_synonym("a2");
        auto affectsBip_constraint =
            AffectsBipConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        auto affectsBipT_constraint =
            AffectsBipTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(7);
        affectsBip_constraint =
            AffectsBipConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(affectsBip_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(10);
        affectsBip_constraint =
            AffectsBipConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affectsBip_constraint.is_valid(assign_mappings));

        affectsBipT_constraint =
            AffectsBipTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

        SECTION("indirect affectsBip") {
            assign_mappings["a2"] = Assignment(7);
            affectsBipT_constraint =
                AffectsBipTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(affectsBipT_constraint.is_valid(assign_mappings));

            affectsBipT_constraint =
                AffectsBipTConstraintLogic(pkb, statement_synonym_dup, statement_synonym);
            REQUIRE_FALSE(affectsBipT_constraint.is_valid(assign_mappings));
        }
    }
}
