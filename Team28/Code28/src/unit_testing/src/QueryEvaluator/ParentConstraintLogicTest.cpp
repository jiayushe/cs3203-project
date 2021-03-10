#include "QueryEvaluator/constraints/ParentConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/ParentTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("ParentConstraintLogic and ParentTConstraintLogic") {
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

    SECTION("no children") {
        auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 1);
        assign_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
        auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
        assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
        assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
        assign_stmt_node->add_child(assign_rhs);

        auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 2);
        auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 3);

        pkb->add_statement(StatementType::ASSIGN, 1, "main", assign_stmt_node);
        pkb->add_statement(StatementType::READ, 2, "main", read_stmt_node);
        pkb->add_statement(StatementType::PRINT, 3, "main", print_stmt_node);

        assign_mappings["a"] = Assignment(1);
        assign_mappings["r"] = Assignment(2);
        assign_mappings["p"] = Assignment(3);

        SECTION("both any") {
            auto parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            auto parentT_constraint = ParentConstraintLogic(pkb, statement_any, statement_any);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));
        }

        SECTION("rhs any") {
            statement_synonym.set_synonym("a");
            auto parent_constraint = ParentConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            auto parentT_constraint = ParentTConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));
        }

        SECTION("lhs any") {
            statement_synonym.set_synonym("a");
            auto parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            auto parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));
        }

        SECTION("synonym or id not found") {
            statement_synonym.set_synonym("ifs");
            auto parent_constraint = ParentConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_THROWS(parent_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(4);
            parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_THROWS(parent_constraint.is_valid(assign_mappings));

            auto parentT_constraint = ParentTConstraintLogic(pkb, statement_synonym, statement_any);
            REQUIRE_THROWS(parentT_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_any);
            REQUIRE_THROWS(parentT_constraint.is_valid(assign_mappings));

            parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_THROWS(parent_constraint.is_valid(assign_mappings));

            parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_THROWS(parent_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_synonym);
            REQUIRE_THROWS(parentT_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_id);
            REQUIRE_THROWS(parentT_constraint.is_valid(assign_mappings));
        }

        SECTION("both synonym or id") {
            statement_synonym.set_synonym("a");
            statement_synonym_dup.set_synonym("r");
            auto parent_constraint =
                ParentConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            statement_id_dup.set_statement_id(2);
            parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));

            auto parentT_constraint =
                ParentTConstraintLogic(pkb, statement_any, statement_synonym_dup);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));

            parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_id_dup);
            REQUIRE_FALSE(parentT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("has children") {
        SECTION("while stmt") {
            auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);

            auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 2);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            assign_stmt_node->add_child(assign_rhs);

            pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
            pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);
            pkb->add_parent_relationship(1, 2);

            // Assignment mappings
            assign_mappings["w"] = Assignment(1);
            assign_mappings["a"] = Assignment(2);

            SECTION("both any") {
                auto parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_any);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                auto parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_any);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }

            SECTION("rhs any") {
                statement_synonym.set_synonym("w");
                auto parent_constraint =
                    ParentConstraintLogic(pkb, statement_synonym, statement_any);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_any);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                auto parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_synonym, statement_any);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_any);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }

            SECTION("lhs any") {
                statement_synonym.set_synonym("a");
                auto parent_constraint =
                    ParentConstraintLogic(pkb, statement_any, statement_synonym);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(2);
                parent_constraint = ParentConstraintLogic(pkb, statement_any, statement_id);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                auto parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_any, statement_synonym);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                parentT_constraint = ParentTConstraintLogic(pkb, statement_any, statement_id);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }

            SECTION("both synonym or both id") {
                statement_synonym.set_synonym("w");
                statement_synonym_dup.set_synonym("a");
                auto parent_constraint =
                    ParentConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                statement_id_dup.set_statement_id(2);
                parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_id_dup);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                auto parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_id_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }
        }

        SECTION("if stmt") {
            auto if_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::IF, 1);

            auto assign_stmt_node =
                std::make_shared<Parser::SimpleNode>(Parser::SimpleNodeType::ASSIGN, 2);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            assign_stmt_node->add_child(assign_rhs);

            auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 3);

            pkb->add_statement(StatementType::IF, 1, "main", if_stmt_node);
            pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);
            pkb->add_statement(StatementType::PRINT, 3, "main", print_stmt_node);
            pkb->add_parent_relationship(1, 2);
            pkb->add_parent_relationship(1, 3);

            // Assignment mappings
            assign_mappings["ifs"] = Assignment(1);
            assign_mappings["a"] = Assignment(2);
            assign_mappings["p"] = Assignment(3);

            SECTION("rhs any") {
                statement_synonym.set_synonym("ifs");
                auto parent_constraint =
                    ParentConstraintLogic(pkb, statement_synonym, statement_any);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_any);
                REQUIRE(parent_constraint.is_valid(assign_mappings));

                auto parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_synonym, statement_any);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_any);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }
        }

        SECTION("while-if-assign stmt") {
            auto while_stmt_node =
                std::make_shared<Parser::SimpleNode>(Parser::SimpleNodeType::WHILE, 1);
            auto if_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::IF, 2);

            auto assign_stmt_node =
                std::make_shared<Parser::SimpleNode>(Parser::SimpleNodeType::ASSIGN, 3);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            assign_stmt_node->add_child(assign_rhs);

            auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 4);

            pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
            pkb->add_statement(StatementType::IF, 2, "main", if_stmt_node);
            pkb->add_statement(StatementType::ASSIGN, 3, "main", assign_stmt_node);
            pkb->add_statement(StatementType::PRINT, 4, "main", print_stmt_node);
            pkb->add_parent_relationship(1, 2);
            pkb->add_parent_relationship(2, 3);
            pkb->add_parent_relationship(2, 4);

            // Assignment mappings
            assign_mappings["w"] = Assignment(1);
            assign_mappings["ifs"] = Assignment(2);
            assign_mappings["a"] = Assignment(3);
            assign_mappings["p"] = Assignment(4);

            SECTION("both synonym or id") {
                statement_synonym.set_synonym("w");
                statement_synonym_dup.set_synonym("a");

                auto parent_constraint =
                    ParentConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));
                auto parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                statement_id_dup.set_statement_id(3);

                parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_synonym_dup);
                REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));
                parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_id_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                statement_synonym_dup.set_synonym("p");

                parent_constraint =
                    ParentConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));
                parentT_constraint =
                    ParentTConstraintLogic(pkb, statement_synonym, statement_synonym_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));

                statement_id_dup.set_statement_id(4);
                parent_constraint = ParentConstraintLogic(pkb, statement_id, statement_synonym_dup);
                REQUIRE_FALSE(parent_constraint.is_valid(assign_mappings));
                parentT_constraint = ParentTConstraintLogic(pkb, statement_id, statement_id_dup);
                REQUIRE(parentT_constraint.is_valid(assign_mappings));
            }
        }
    }
}
