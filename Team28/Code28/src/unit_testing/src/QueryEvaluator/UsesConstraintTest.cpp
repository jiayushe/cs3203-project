#include "QueryEvaluator/constraints/UsesConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "SimpleExtractor/DesignExtractor.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("UsesConstraint") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);

    // Any statement ref
    auto statement_any = Parser::StatementRef();
    statement_any.set_type(Parser::StatementRefType::ANY);
    // Any Entity ref
    auto entity_any = Parser::EntityRef();
    entity_any.set_type(Parser::EntityRefType::ANY);
    auto entity_synonym = Parser::EntityRef();
    entity_synonym.set_type(Parser::EntityRefType::SYNONYM);
    auto entity_name = Parser::EntityRef();
    entity_name.set_type(Parser::EntityRefType::NAME);
    // Synonym statement ref
    auto statement_synonym = Parser::StatementRef();
    statement_synonym.set_type(Parser::StatementRefType::SYNONYM);
    auto statement_synonym_dup = Parser::StatementRef();
    statement_synonym_dup.set_type(Parser::StatementRefType::SYNONYM);
    // Statement ID statement ref
    auto statement_id = Parser::StatementRef();
    statement_id.set_type(Parser::StatementRefType::STATEMENT_ID);

    SECTION("lhs any") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb->add_procedure("main");

        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);

        assign_mappings["w"] = Assignment(1);
        auto uses_constraint = UsesConstraint(pkb, statement_any, entity_any);
        REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
    }

    SECTION("while const value stmt with read child stmt") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb->add_procedure("main");

        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);

        auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 2);
        pkb->add_statement(StatementType::READ, 2, "main", read_stmt_node);
        pkb->add_variable("a"); // read stmt

        // Assignment mappings
        assign_mappings["w"] = Assignment(1);
        assign_mappings["r"] = Assignment(1);
        assign_mappings["v"] = Assignment("a");

        SECTION("while const value stmt") {
            statement_synonym.set_synonym("w");
            auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_any);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            entity_synonym.set_synonym("v");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            entity_name.set_name("a");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
        }

        SECTION("read stmt") {
            statement_synonym.set_synonym("r");
            auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_any);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            entity_synonym.set_synonym("v");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(2);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("while cond stmt with print child stmt") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb->add_procedure("main");

        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);

        auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 2);
        pkb->add_statement(StatementType::PRINT, 2, "main", print_stmt_node);
        pkb->add_variable("x"); // while stmt cond
        pkb->add_variable("b"); // print stmt
        pkb->add_use_relationship(1, "b");
        pkb->add_use_relationship(1, "x");
        pkb->add_use_relationship(2, "b");

        assign_mappings["w"] = Assignment(1);
        assign_mappings["p"] = Assignment(2);

        SECTION("while stmt condition variable") {
            assign_mappings["v"] = Assignment("x");

            statement_synonym.set_synonym("w");
            entity_synonym.set_synonym("v");
            auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            entity_name.set_name("x");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
            REQUIRE(uses_constraint.is_valid(assign_mappings));
        }

        SECTION("while stmt with print child stmt variable") {
            assign_mappings["v"] = Assignment("b");

            statement_synonym.set_synonym("w");
            entity_synonym.set_synonym("v");
            auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            entity_name.set_name("b");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
            REQUIRE(uses_constraint.is_valid(assign_mappings));
        }

        SECTION("print stmt variable") {
            assign_mappings["v"] = Assignment("b");

            statement_synonym.set_synonym("p");
            entity_synonym.set_synonym("v");
            auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            entity_name.set_name("b");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(2);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
            REQUIRE(uses_constraint.is_valid(assign_mappings));

            assign_mappings["p"] = Assignment(2);
            assign_mappings["v"] = Assignment("x");

            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            entity_name.set_name("x");
            uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(2);
            uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

            uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
            REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
        }

        SECTION("with inner if cond, child assign and read stmt") {
            auto if_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::IF, 3);
            pkb->add_statement(StatementType::IF, 3, "main", if_stmt_node);

            auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 4);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            assign_stmt_node->add_child(assign_rhs);
            pkb->add_statement(StatementType::ASSIGN, 4, "main", assign_stmt_node);

            auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 5);
            pkb->add_statement(StatementType::READ, 5, "main", read_stmt_node);

            pkb->add_variable("y");      // if cond
            pkb->add_variable("result"); // lhs of assign
            pkb->add_variable("z");      // rhs of assign
            pkb->add_variable("a");      // read variable
            pkb->add_constant(2);
            pkb->add_use_relationship(1, "y");
            pkb->add_use_relationship(1, "z");
            pkb->add_use_relationship(3, "y");
            pkb->add_use_relationship(3, "z");
            pkb->add_use_relationship(4, "z");

            assign_mappings["ifs"] = Assignment(3);
            assign_mappings["a"] = Assignment(4);
            assign_mappings["r"] = Assignment(5);
            assign_mappings["v"] = Assignment("y");

            SECTION("while stmt and if cond variable") {
                statement_synonym.set_synonym("w");
                entity_synonym.set_synonym("v");
                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("y");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));
            }

            SECTION("while stmt and assign child stmt variable") {
                statement_synonym.set_synonym("w");
                entity_synonym.set_synonym("v");
                assign_mappings["v"] = Assignment("z");
                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("z");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));
            }

            SECTION("while stmt and read child stmt variable") {
                statement_synonym.set_synonym("w");
                entity_synonym.set_synonym("v");
                assign_mappings["v"] = Assignment("a");
                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("a");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
            }

            SECTION("if stmt cond variable") {
                statement_synonym.set_synonym("ifs");
                entity_synonym.set_synonym("v");
                assign_mappings["v"] = Assignment("y");

                statement_synonym.set_synonym("ifs");
                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("y");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(3);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));
            }

            SECTION("if stmt and assign child stmt variable") {
                statement_synonym.set_synonym("ifs");
                entity_synonym.set_synonym("v");

                assign_mappings["v"] = Assignment("z");
                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("z");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(3);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));
            }

            SECTION("assign stmt variable") {
                statement_synonym.set_synonym("a");
                entity_synonym.set_synonym("v");

                assign_mappings["v"] = Assignment("z");

                auto uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("z");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(4);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_any);
                REQUIRE(uses_constraint.is_valid(assign_mappings));

                assign_mappings["v"] = Assignment("result");

                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_synonym);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                entity_name.set_name("result");
                uses_constraint = UsesConstraint(pkb, statement_synonym, entity_name);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(4);
                uses_constraint = UsesConstraint(pkb, statement_id, entity_synonym);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));

                uses_constraint = UsesConstraint(pkb, statement_id, entity_name);
                REQUIRE_FALSE(uses_constraint.is_valid(assign_mappings));
            }
        }
    }
}
