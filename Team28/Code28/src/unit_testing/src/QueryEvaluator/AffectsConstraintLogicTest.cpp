#include "QueryEvaluator/constraints/AffectsConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/AffectsTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("AffectsConstraintLogic and AffectsTConstraintLogic") {
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
    // 1   x = 0;
    // 2   if () {
    // 3      x = y + 2;
    //    else {
    // 4      y = 2;}
    // 5   z = x;
    // 6   y = z;}

    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    auto assign_stmt_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 1);
    assign_stmt_node_1->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
    auto assign_rhs_1 = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, 0);
    assign_stmt_node_1->add_child(assign_rhs_1);
    auto if_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::IF, 2);
    auto assign_stmt_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 3);
    assign_stmt_node_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
    auto assign_rhs_2 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    assign_rhs_2->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "2"));
    assign_stmt_node_2->add_child(assign_rhs_2);
    auto assign_stmt_node_3 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 4);
    assign_stmt_node_3->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_3 = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, 2);
    assign_stmt_node_3->add_child(assign_rhs_3);
    auto assign_stmt_node_4 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 5);
    assign_stmt_node_4->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z"));
    auto assign_rhs_4 = std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x");
    assign_stmt_node_4->add_child(assign_rhs_4);
    auto assign_stmt_node_5 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 6);
    assign_stmt_node_5->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_5 = std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z");
    assign_stmt_node_5->add_child(assign_rhs_5);

    pkb->add_procedure("main");
    pkb->add_statement(StatementType::ASSIGN, 1, "main", assign_stmt_node_1);
    pkb->add_statement(StatementType::IF, 2, "main", if_stmt_node);
    pkb->add_statement(StatementType::ASSIGN, 3, "main", assign_stmt_node_2);
    pkb->add_statement(StatementType::ASSIGN, 4, "main", assign_stmt_node_3);
    pkb->add_statement(StatementType::ASSIGN, 5, "main", assign_stmt_node_4);
    pkb->add_statement(StatementType::ASSIGN, 6, "main", assign_stmt_node_5);
    pkb->add_affect_relationship(5, 6);
    pkb->add_affect_relationship(1, 5);

    SECTION("both any") {
        auto affects_constraint = AffectsConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        auto pkb2 = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb2->add_procedure("main");
        affects_constraint = AffectsConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        auto affectsT_constraint = AffectsTConstraintLogic(pkb, statement_any, statement_any);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb2, statement_any, statement_any);
        REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));
    }

    SECTION("rhs any") {
        statement_id.set_statement_id(1);
        auto affects_constraint = AffectsConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        auto affectsT_constraint = AffectsTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(6);
        affects_constraint = AffectsConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_id, statement_any);
        REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));

        assign_mappings["a1"] = Assignment(1);
        statement_synonym.set_synonym("a1");
        affects_constraint = AffectsConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(6);
        statement_synonym.set_synonym("a2");
        affects_constraint = AffectsConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_synonym, statement_any);
        REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));
    }

    SECTION("lhs_any") {
        statement_id.set_statement_id(5);
        auto affects_constraint = AffectsConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        auto affectsT_constraint = AffectsTConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        affects_constraint = AffectsConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_any, statement_id);
        REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(5);
        statement_synonym.set_synonym("a2");
        affects_constraint = AffectsConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        assign_mappings["a1"] = Assignment(1);
        statement_synonym.set_synonym("a1");
        affects_constraint = AffectsConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        affectsT_constraint = AffectsTConstraintLogic(pkb, statement_any, statement_synonym);
        REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));
    }

    SECTION("both statement id") {
        statement_id.set_statement_id(1);
        statement_id_dup.set_statement_id(5);
        auto affects_constraint = AffectsConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        auto affectsT_constraint = AffectsTConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        statement_id_dup.set_statement_id(3);
        affects_constraint = AffectsConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(5);
        statement_id_dup.set_statement_id(1);
        affects_constraint = AffectsConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        SECTION("indirect affects") {
            statement_id.set_statement_id(1);
            statement_id_dup.set_statement_id(6);
            affectsT_constraint = AffectsTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(affectsT_constraint.is_valid(assign_mappings));

            affectsT_constraint = AffectsTConstraintLogic(pkb, statement_id_dup, statement_id);
            REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("both synonym") {
        assign_mappings["a1"] = Assignment(1);
        assign_mappings["a2"] = Assignment(5);
        statement_synonym.set_synonym("a1");
        statement_synonym_dup.set_synonym("a2");
        auto affects_constraint =
            AffectsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affects_constraint.is_valid(assign_mappings));

        auto affectsT_constraint =
            AffectsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(affectsT_constraint.is_valid(assign_mappings));

        affects_constraint = AffectsConstraintLogic(pkb, statement_synonym_dup, statement_synonym);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        assign_mappings["a2"] = Assignment(3);
        statement_synonym_dup.set_synonym("a2");
        affects_constraint = AffectsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(affects_constraint.is_valid(assign_mappings));

        SECTION("indirect affects") {
            assign_mappings["a2"] = Assignment(6);
            affectsT_constraint =
                AffectsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(affectsT_constraint.is_valid(assign_mappings));

            affectsT_constraint =
                AffectsTConstraintLogic(pkb, statement_synonym_dup, statement_synonym);
            REQUIRE_FALSE(affectsT_constraint.is_valid(assign_mappings));
        }
    }
}
