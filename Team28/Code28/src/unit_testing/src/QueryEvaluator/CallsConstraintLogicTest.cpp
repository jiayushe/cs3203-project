#include "QueryEvaluator/constraints/CallsConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/CallsTConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("CallsConstraintLogic and CallsTConstraintLogic") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);

    // Any Entity ref
    auto entity_any = Parser::EntityRef();
    entity_any.set_type(Parser::EntityRefType::ANY);
    // Synonym Entity ref
    auto entity_synonym = Parser::EntityRef();
    entity_synonym.set_type(Parser::EntityRefType::SYNONYM);
    auto entity_synonym_dup = Parser::EntityRef();
    entity_synonym_dup.set_type(Parser::EntityRefType::SYNONYM);
    // Name Entity ref
    auto entity_name = Parser::EntityRef();
    entity_name.set_type(Parser::EntityRefType::NAME);
    auto entity_name_dup = Parser::EntityRef();
    entity_name_dup.set_type(Parser::EntityRefType::NAME);

    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    pkb->add_procedure("main");

    SECTION("One procedure, no Calls/Calls* relationship") {
        // Assignment mappings
        assign_mappings["p1"] = Assignment("main");

        SECTION("both any") {
            auto calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_any);
            REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

            auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_any);
            REQUIRE_FALSE(callsT_constraint.is_valid(assign_mappings));
        }

        SECTION("rhs any") {
            entity_synonym.set_synonym("p1");
            auto calls_constraint = CallsConstraintLogic(pkb, entity_synonym, entity_any);
            REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

            entity_name.set_name("main");
            calls_constraint = CallsConstraintLogic(pkb, entity_name, entity_any);
            REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

            auto callsT_constraint = CallsTConstraintLogic(pkb, entity_synonym, entity_any);
            REQUIRE_FALSE(callsT_constraint.is_valid(assign_mappings));

            callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_any);
            REQUIRE_FALSE(callsT_constraint.is_valid(assign_mappings));
        }

        SECTION("lhs any") {
            entity_synonym.set_synonym("p1");
            auto calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_synonym);
            REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

            entity_name.set_name("main");
            calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_name);
            REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

            auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_synonym);
            REQUIRE_FALSE(callsT_constraint.is_valid(assign_mappings));

            callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_name);
            REQUIRE_FALSE(callsT_constraint.is_valid(assign_mappings));
        }

        SECTION("synonym or name not found") {
            entity_synonym.set_synonym("p1");
            entity_synonym_dup.set_synonym("p2");
            auto calls_constraint = CallsConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
            REQUIRE_THROWS(calls_constraint.is_valid(assign_mappings));

            entity_name.set_synonym("main");
            entity_name_dup.set_synonym("logic");
            calls_constraint = CallsConstraintLogic(pkb, entity_name, entity_name_dup);
            REQUIRE_THROWS(calls_constraint.is_valid(assign_mappings));

            auto callsT_constraint = CallsTConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
            REQUIRE_THROWS(callsT_constraint.is_valid(assign_mappings));

            callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_name_dup);
            REQUIRE_THROWS(callsT_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("More than one procedures") {
        auto call_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::CALL, 1);
        call_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "logic"));
        pkb->add_statement(StatementType::CALL, 1, "main", call_stmt_node);
        pkb->add_procedure("logic");
        pkb->add_call_relationship("main", "logic");

        // Assignment mappings
        assign_mappings["p1"] = Assignment("main");
        assign_mappings["p2"] = Assignment("logic");

        SECTION("Two direct call procedures") {
            SECTION("both any") {
                auto calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_any);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("rhs any") {
                entity_synonym.set_synonym("p1");
                auto calls_constraint = CallsConstraintLogic(pkb, entity_synonym, entity_any);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                entity_name.set_name("main");
                calls_constraint = CallsConstraintLogic(pkb, entity_name, entity_any);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_synonym, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("lhs any") {
                entity_synonym.set_synonym("p2");
                auto calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_synonym);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                entity_name.set_name("logic");
                calls_constraint = CallsConstraintLogic(pkb, entity_any, entity_name);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_synonym);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_name);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("both synonym or name") {
                entity_synonym.set_synonym("p1");
                entity_synonym_dup.set_synonym("p2");
                auto calls_constraint =
                    CallsConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                entity_name.set_name("main");
                entity_name_dup.set_name("logic");
                calls_constraint = CallsConstraintLogic(pkb, entity_name, entity_name_dup);
                REQUIRE(calls_constraint.is_valid(assign_mappings));

                auto callsT_constraint =
                    CallsTConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_name_dup);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }
        }

        SECTION("Two indirect call procedures") {
            auto call_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::CALL, 2);
            call_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "execute"));
            pkb->add_statement(StatementType::CALL, 2, "logic", call_stmt_node);
            pkb->add_procedure("execute");
            pkb->add_call_relationship("logic", "execute");

            // Assignment mappings
            assign_mappings["p3"] = Assignment("execute");

            SECTION("both any") {
                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("rhs any") {
                entity_synonym.set_synonym("p1");
                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_synonym, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                entity_name.set_name("main");
                callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_any);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("lhs any") {
                entity_synonym.set_synonym("p3");
                auto callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_synonym);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                entity_name.set_name("execute");
                callsT_constraint = CallsTConstraintLogic(pkb, entity_any, entity_name);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }

            SECTION("both synonym or name") {
                entity_synonym.set_synonym("p1");
                entity_synonym_dup.set_synonym("p3");
                auto calls_constraint =
                    CallsConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
                REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

                entity_name.set_name("main");
                entity_name_dup.set_name("execute");
                calls_constraint = CallsConstraintLogic(pkb, entity_name, entity_name_dup);
                REQUIRE_FALSE(calls_constraint.is_valid(assign_mappings));

                auto callsT_constraint =
                    CallsTConstraintLogic(pkb, entity_synonym, entity_synonym_dup);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));

                callsT_constraint = CallsTConstraintLogic(pkb, entity_name, entity_name_dup);
                REQUIRE(callsT_constraint.is_valid(assign_mappings));
            }
        }
    }
}
