#include "QueryEvaluator/constraints/FollowsConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/FollowsTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("FollowsConstraintLogic and FollowsTConstraintLogic") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    pkb->add_procedure("main");

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

    auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
    auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 2);
    assign_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    assign_stmt_node->add_child(assign_rhs);

    auto assign_stmt_node_two =
        std::make_shared<Parser::SimpleNode>(Parser::SimpleNodeType::ASSIGN, 3);
    assign_stmt_node_two->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs_two = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs_two->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
    assign_rhs_two->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    assign_stmt_node_two->add_child(assign_rhs);

    SECTION("no follow") {
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);

        assign_mappings["w"] = Assignment(1);
        assign_mappings["a"] = Assignment(2);

        SECTION("both any") {
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("rhs any") {
            statement_synonym.set_synonym("w");
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("lhs any") {
            statement_synonym.set_synonym("w");
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("synonym or id not found") {
            statement_synonym.set_synonym("ifs");
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_THROWS(follows_constraint.is_valid(assign_mappings));

            follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_THROWS(follows_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(3);
            follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_THROWS(follows_constraint.is_valid(assign_mappings));

            follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_THROWS(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_THROWS(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_THROWS(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_THROWS(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_THROWS(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("both synonym or id") {
            statement_synonym.set_synonym("w");
            statement_synonym_dup.set_synonym("a");
            auto follows_constraint =
                FollowsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            statement_id_dup.set_statement_id(2);
            follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("one-level follow") {
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 3, "main", assign_stmt_node_two);
        pkb->add_follow_relationship(1, 3);

        // Assignment mappings
        assign_mappings["w"] = Assignment(1);
        assign_mappings["a"] = Assignment(3);

        SECTION("both any") {
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("rhs any") {
            statement_synonym.set_synonym("w");
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("lhs any") {
            statement_synonym.set_synonym("a");
            auto follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(3);
            follows_constraint = FollowsConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));
        }

        SECTION("both synonym or id") {
            statement_synonym.set_synonym("w");
            statement_synonym_dup.set_synonym("a");
            auto follows_constraint =
                FollowsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            statement_id_dup.set_statement_id(3);
            follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(follows_constraint.is_valid(assign_mappings));

            auto followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));

            followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE(followsT_constraint.is_valid(assign_mappings));

            statement_synonym.set_synonym("a");
            statement_synonym_dup.set_synonym("w");
            follows_constraint =
                FollowsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));

            followsT_constraint =
                FollowsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE_FALSE(followsT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("multi-level follow") {
        auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 4);
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 3, "main", assign_stmt_node_two);
        pkb->add_statement(StatementType::READ, 4, "main", read_stmt_node);

        pkb->add_follow_relationship(1, 3);
        pkb->add_follow_relationship(3, 4);

        // Assignment mappings
        assign_mappings["w"] = Assignment(1);
        assign_mappings["a"] = Assignment(3);
        assign_mappings["r"] = Assignment(4);

        statement_synonym.set_synonym("w");
        statement_synonym_dup.set_synonym("r");

        auto follows_constraint =
            FollowsConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));
        auto followsT_constraint =
            FollowsTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
        REQUIRE(followsT_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        statement_id_dup.set_statement_id(4);

        follows_constraint = FollowsConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE_FALSE(follows_constraint.is_valid(assign_mappings));
        followsT_constraint = FollowsTConstraintLogic(pkb, statement_id, statement_id_dup);
        REQUIRE(followsT_constraint.is_valid(assign_mappings));
    }
}
