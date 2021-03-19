#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/WithRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/PatternLConstraintLogic.h"
#include "QueryEvaluator/constraints/PatternRConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("PatternLConstraintLogic and PatternRConstraintLogic") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    pkb->add_procedure("main");
    auto pattern = Parser::Pattern();
    auto entity_ref = Parser::EntityRef();

    SECTION("while stmt") {
        auto pattern_while = Parser::PatternWhile();
        assign_mappings["w"] = Assignment(1);

        SECTION("while stmt with constant value") {
            entity_ref.set_type(EntityRefType::ANY);
            pattern_while.set_entity_ref(entity_ref);
            pattern_while.set_while_synonym("w");
            pattern.set_type(PatternType::WHILE);
            pattern.set_pattern_while(pattern_while);

            // while stmt with constant value
            auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
            auto constant = std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, 20);
            while_stmt_node->add_child(constant);
            pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
            auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
            REQUIRE_FALSE(pattern_constraint.is_valid(assign_mappings));
        }

        SECTION("while stmt with condition") {
            auto pattern = Parser::Pattern();
            auto pattern_while = Parser::PatternWhile();
            auto entity_ref = Parser::EntityRef();

            auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
            auto cond = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
            cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
            cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "20"));
            while_stmt_node->add_child(cond);
            pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
            pkb->add_variable("x");
            pkb->add_use_relationship(1, "x");

            SECTION("any entity ref") {
                entity_ref.set_type(EntityRefType::ANY);
                pattern_while.set_entity_ref(entity_ref);
                pattern_while.set_while_synonym("w");
                pattern.set_type(PatternType::WHILE);
                pattern.set_pattern_while(pattern_while);

                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }

            SECTION("synonym entity ref") {
                entity_ref.set_type(EntityRefType::SYNONYM);
                entity_ref.set_synonym("v");
                pattern_while.set_entity_ref(entity_ref);
                pattern_while.set_while_synonym("w");
                pattern.set_type(PatternType::WHILE);
                pattern.set_pattern_while(pattern_while);
                pkb->add_variable("x");
                pkb->add_use_relationship(1, "x");

                assign_mappings["v"] = Assignment("x");
                assign_mappings["w"] = Assignment(1);
                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }

            SECTION("name entity ref") {
                entity_ref.set_type(EntityRefType::NAME);
                entity_ref.set_name("count");
                pattern_while.set_entity_ref(entity_ref);
                pattern_while.set_while_synonym("w");
                pattern.set_type(PatternType::WHILE);
                pattern.set_pattern_while(pattern_while);
                pkb->add_variable("count");
                pkb->add_use_relationship(1, "count");

                assign_mappings["v"] = Assignment("count");
                assign_mappings["w"] = Assignment(1);
                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }
        }
    }

    SECTION("if stmt") {
        auto pattern_if = Parser::PatternIf();
        assign_mappings["ifs"] = Assignment(1);

        SECTION("if stmt exists") {
            auto pattern = Parser::Pattern();
            auto pattern_if = Parser::PatternIf();
            auto entity_ref = Parser::EntityRef();

            auto if_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::IF, 1);
            auto cond = std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, "==");
            cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
            cond->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "20"));
            if_stmt_node->add_child(cond);
            pkb->add_statement(StatementType::IF, 1, "main", if_stmt_node);
            pkb->add_variable("x");
            pkb->add_use_relationship(1, "x");

            SECTION("any entity ref") {
                entity_ref.set_type(EntityRefType::ANY);
                pattern_if.set_entity_ref(entity_ref);
                pattern_if.set_if_synonym("ifs");
                pattern.set_type(PatternType::IF);
                pattern.set_pattern_if(pattern_if);

                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }

            SECTION("synonym entity ref") {
                entity_ref.set_type(EntityRefType::SYNONYM);
                entity_ref.set_synonym("v");
                pattern_if.set_entity_ref(entity_ref);
                pattern_if.set_if_synonym("ifs");
                pattern.set_type(PatternType::IF);
                pattern.set_pattern_if(pattern_if);
                pkb->add_variable("x");
                pkb->add_use_relationship(1, "x");

                assign_mappings["v"] = Assignment("x");
                assign_mappings["ifs"] = Assignment(1);
                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }

            SECTION("name entity ref") {
                entity_ref.set_type(EntityRefType::NAME);
                entity_ref.set_name("count");
                pattern_if.set_entity_ref(entity_ref);
                pattern_if.set_if_synonym("ifs");
                pattern.set_type(PatternType::IF);
                pattern.set_pattern_if(pattern_if);
                pkb->add_variable("count");
                pkb->add_use_relationship(1, "count");

                assign_mappings["v"] = Assignment("count");
                assign_mappings["ifs"] = Assignment(1);
                auto pattern_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(pattern_constraint.is_valid(assign_mappings));
            }
        }
    }

    SECTION("assign stmt") {
        auto pattern_assign = Parser::PatternAssign();
        assign_mappings["a"] = Assignment(1);

        SECTION("assign stmt exists") {
            auto pattern = Parser::Pattern();
            auto pattern_assign = Parser::PatternAssign();
            auto entity_ref = Parser::EntityRef();

            auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 1);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
            assign_stmt_node->add_child(assign_rhs);
            pkb->add_statement(StatementType::ASSIGN, 1, "main", assign_stmt_node);
            pkb->add_variable("y");
            pkb->add_use_relationship(1, "y");

            SECTION("both any entity ref") {
                entity_ref.set_type(EntityRefType::ANY);
                pattern_assign.set_entity_ref(entity_ref);
                pattern_assign.set_assign_synonym("a");
                pattern.set_type(PatternType::ASSIGN);
                pattern.set_pattern_assign(pattern_assign);

                auto patternL_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(patternL_constraint.is_valid(assign_mappings));

                auto patternR_assign = Parser::PatternAssign();
                auto expr_spec = Parser::ExpressionSpec();
                expr_spec.set_type(ExpressionSpecType::ANY);
                patternR_assign.set_expression_spec(expr_spec);
                auto patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                REQUIRE(patternR_constraint.is_valid(assign_mappings));
            }

            SECTION("rhs any entity ref") {
                entity_ref.set_type(EntityRefType::SYNONYM);
                entity_ref.set_synonym("v");
                pattern_assign.set_entity_ref(entity_ref);
                pattern_assign.set_assign_synonym("a");
                pattern.set_type(PatternType::ASSIGN);
                pattern.set_pattern_assign(pattern_assign);

                pkb->add_variable("y");
                pkb->add_modify_relationship(1, "y");
                assign_mappings["v"] = Assignment("y");
                assign_mappings["a"] = Assignment(1);
                auto patternL_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(patternL_constraint.is_valid(assign_mappings));

                entity_ref.set_type(EntityRefType::NAME);
                entity_ref.set_name("y");
                pattern_assign.set_entity_ref(entity_ref);
                pattern_assign.set_assign_synonym("a");
                pattern.set_type(PatternType::ASSIGN);
                pattern.set_pattern_assign(pattern_assign);
                patternL_constraint = PatternLConstraintLogic(pkb, pattern);
                REQUIRE(patternL_constraint.is_valid(assign_mappings));
            }

            SECTION("lhs any entity ref") {
                SECTION("exact pattern") {
                    auto patternR_assign = Parser::PatternAssign();
                    patternR_assign.set_assign_synonym("a");
                    auto expr_spec = Parser::ExpressionSpec();
                    expr_spec.set_type(ExpressionSpecType::EXACT_PATTERN);
                    expr_spec.set_exact_pattern(assign_rhs);
                    patternR_assign.set_expression_spec(expr_spec);

                    pkb->add_variable("y");
                    pkb->add_modify_relationship(1, "y");

                    assign_mappings["v"] = Assignment("y");
                    assign_mappings["a"] = Assignment(1);
                    auto patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                    REQUIRE(patternR_constraint.is_valid(assign_mappings));
                }

                SECTION("pattern with one level ast") {
                    auto patternR_assign = Parser::PatternAssign();
                    patternR_assign.set_assign_synonym("a");
                    auto expr_spec = Parser::ExpressionSpec();
                    expr_spec.set_type(ExpressionSpecType::PATTERN);
                    expr_spec.set_pattern(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
                    patternR_assign.set_expression_spec(expr_spec);

                    assign_mappings["v"] = Assignment("y");
                    assign_mappings["a"] = Assignment(1);
                    auto patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                    REQUIRE(patternR_constraint.is_valid(assign_mappings));

                    expr_spec.set_pattern(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
                    patternR_assign.set_expression_spec(expr_spec);

                    patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                    REQUIRE_FALSE(patternR_constraint.is_valid(assign_mappings));
                }

                SECTION("pattern with multi-level ast") {
                    // z = t + 10 - x * y
                    assign_stmt_node =
                        std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 2);
                    assign_stmt_node->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z"));
                    auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "-");
                    auto assign_rhs_child =
                        std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
                    assign_rhs_child->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
                    assign_rhs_child->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "t"));
                    assign_rhs->add_child(assign_rhs_child);
                    auto assign_lhs_child =
                        std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
                    assign_lhs_child->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
                    assign_lhs_child->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
                    assign_rhs->add_child(assign_lhs_child);
                    assign_stmt_node->add_child(assign_rhs);
                    pkb->add_statement(StatementType::ASSIGN, 2, "main", assign_stmt_node);

                    pkb->add_variable("y");
                    pkb->add_variable("z");
                    pkb->add_variable("t");
                    pkb->add_variable("x");
                    pkb->add_modify_relationship(2, "y");
                    pkb->add_modify_relationship(2, "z");
                    pkb->add_modify_relationship(2, "t");
                    pkb->add_modify_relationship(2, "x");

                    assign_mappings["a"] = Assignment(2);
                    // x * t
                    auto pattern_expr =
                        std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
                    pattern_expr->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
                    pattern_expr->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "t"));

                    auto patternR_assign = Parser::PatternAssign();
                    patternR_assign.set_assign_synonym("a");
                    auto expr_spec = Parser::ExpressionSpec();
                    expr_spec.set_type(ExpressionSpecType::PATTERN);
                    expr_spec.set_pattern(pattern_expr);
                    patternR_assign.set_expression_spec(expr_spec);

                    auto patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                    REQUIRE_FALSE(patternR_constraint.is_valid(assign_mappings));

                    // x * y
                    pattern_expr = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
                    pattern_expr->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "x"));
                    pattern_expr->add_child(
                        std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));

                    patternR_assign = Parser::PatternAssign();
                    patternR_assign.set_assign_synonym("a");
                    expr_spec = Parser::ExpressionSpec();
                    expr_spec.set_type(ExpressionSpecType::PATTERN);
                    expr_spec.set_pattern(pattern_expr);
                    patternR_assign.set_expression_spec(expr_spec);

                    patternR_constraint = PatternRConstraintLogic(pkb, patternR_assign);
                    REQUIRE(patternR_constraint.is_valid(assign_mappings));
                }
            }
        }
    }
}
