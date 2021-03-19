#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/ModifiesPConstraintLogic.h"
#include "QueryEvaluator/constraints/ModifiesSConstraintLogic.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("ModifiesSConstraintLogicLogic and ModifiesPConstraintLogic") {
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

    // Any Entity ref
    auto entity_any = Parser::EntityRef();
    entity_any.set_type(Parser::EntityRefType::ANY);
    // Synonym entity ref
    auto entity_synonym = Parser::EntityRef();
    entity_synonym.set_type(Parser::EntityRefType::SYNONYM);
    auto entity_synonym_dup = Parser::EntityRef();
    entity_synonym_dup.set_type(Parser::EntityRefType::SYNONYM);
    // Name entity ref
    auto entity_name = Parser::EntityRef();
    entity_name.set_type(Parser::EntityRefType::NAME);

    SECTION("lhs any") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        pkb->add_procedure("main");

        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);

        assign_mappings["w"] = Assignment(1);
        auto modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_any, entity_any);
        auto modifiesP_constraint = ModifiesPConstraintLogic(pkb, entity_any, entity_any);
        REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));
        REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));
    }

    SECTION("while with print child stmt") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        auto print_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 2);

        pkb->add_procedure("main");
        pkb->add_variable("x"); // while stmt cond
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
        pkb->add_statement(StatementType::PRINT, 2, "main", print_stmt_node);
        pkb->add_parent_relationship(1, 2);
        pkb->add_variable("a"); // print stmt

        // Assignment mappings
        assign_mappings["w"] = Assignment(1);
        assign_mappings["pr"] = Assignment(2);
        assign_mappings["v"] = Assignment("a");
        assign_mappings["p"] = Assignment("main");

        SECTION("while cond") {
            statement_synonym.set_synonym("w");
            auto modifiesS_constraint =
                ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            entity_synonym.set_synonym("v");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            entity_name.set_name("x");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            entity_synonym_dup.set_synonym("p");
            auto modifiesP_constraint =
                ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_name);
            REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));

            modifiesP_constraint =
                ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_synonym);
            REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));

            modifiesP_constraint = ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_any);
            REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));
        }

        SECTION("print statement") {
            statement_synonym.set_synonym("pr");
            auto modifiesS_constraint =
                ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            entity_synonym.set_synonym("v");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            entity_name.set_name("a");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(2);
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
            REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("while with read child stmt") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        auto while_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::WHILE, 1);
        auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 2);

        pkb->add_procedure("main");
        pkb->add_variable("x"); // while stmt cond
        pkb->add_statement(StatementType::WHILE, 1, "main", while_stmt_node);
        pkb->add_statement(StatementType::READ, 2, "main", read_stmt_node);
        pkb->add_parent_relationship(1, 2);
        pkb->add_variable("a"); // read stmt
        pkb->add_modify_relationship(2, "a");

        assign_mappings["w"] = Assignment(1);
        assign_mappings["r"] = Assignment(2);
        assign_mappings["p"] = Assignment("main");
        assign_mappings["v"] = Assignment("a");

        SECTION("while stmt") {
            statement_synonym.set_synonym("w");
            entity_synonym.set_synonym("v");
            auto modifiesS_constraint =
                ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            entity_name.set_name("a");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(1);
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            entity_synonym_dup.set_synonym("p");
            auto modifiesP_constraint =
                ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_name);
            REQUIRE(modifiesP_constraint.is_valid(assign_mappings));

            modifiesP_constraint =
                ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_synonym);
            REQUIRE(modifiesP_constraint.is_valid(assign_mappings));

            modifiesP_constraint = ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_any);
            REQUIRE(modifiesP_constraint.is_valid(assign_mappings));
        }

        SECTION("read stmt") {
            statement_synonym.set_synonym("r");
            entity_synonym.set_synonym("v");
            auto modifiesS_constraint =
                ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            entity_name.set_name("a");
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            statement_id.set_statement_id(2);
            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

            modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
            REQUIRE(modifiesS_constraint.is_valid(assign_mappings));
        }

        SECTION("with inner if cond, child assign and read stmt") {
            auto if_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::IF, 3);
            auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 4);
            assign_stmt_node->add_child(
                std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "result"));
            auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
            assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "z"));
            assign_stmt_node->add_child(assign_rhs);
            auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 5);

            pkb->add_statement(StatementType::IF, 3, "main", if_stmt_node);
            pkb->add_statement(StatementType::ASSIGN, 4, "main", assign_stmt_node);
            pkb->add_statement(StatementType::READ, 5, "main", read_stmt_node);
            pkb->add_variable("y");      // if cond
            pkb->add_variable("result"); // lhs of assign
            pkb->add_variable("z");      // rhs of assign
            pkb->add_variable("a");      // read variable
            pkb->add_constant(2);
            pkb->add_modify_relationship(1, "result");
            pkb->add_modify_relationship(1, "a");
            pkb->add_modify_relationship(2, "a");
            pkb->add_modify_relationship(3, "result");
            pkb->add_modify_relationship(4, "result");

            assign_mappings["ifs"] = Assignment(3);
            assign_mappings["a"] = Assignment(4);
            assign_mappings["r"] = Assignment(5);
            assign_mappings["v"] = Assignment("y");

            SECTION("while stmt and if cond variable") {
                statement_synonym.set_synonym("w");
                entity_synonym.set_synonym("v");
                auto modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                entity_name.set_name("y");
                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                entity_synonym_dup.set_synonym("p");
                auto modifiesP_constraint =
                    ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_synonym);
                REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));

                modifiesP_constraint =
                    ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_name);
                REQUIRE_FALSE(modifiesP_constraint.is_valid(assign_mappings));
            }

            SECTION("while stmt and assign child stmt variable") {
                statement_synonym.set_synonym("w");
                entity_synonym.set_synonym("v");
                assign_mappings["v"] = Assignment("result");
                auto modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                entity_name.set_name("result");
                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(1);
                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                entity_synonym_dup.set_synonym("p");
                auto modifiesP_constraint =
                    ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_synonym);
                REQUIRE(modifiesP_constraint.is_valid(assign_mappings));

                modifiesP_constraint =
                    ModifiesPConstraintLogic(pkb, entity_synonym_dup, entity_name);
                REQUIRE(modifiesP_constraint.is_valid(assign_mappings));
            }

            SECTION("if stmt and assign child stmt variable") {
                statement_synonym.set_synonym("ifs");
                entity_synonym.set_synonym("v");

                assign_mappings["v"] = Assignment("result");
                auto modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                entity_name.set_name("result");
                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(3);
                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));
            }

            SECTION("assign stmt variable") {
                statement_synonym.set_synonym("a");
                entity_synonym.set_synonym("v");

                assign_mappings["v"] = Assignment("z");

                auto modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                entity_name.set_name("z");
                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_any);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(4);
                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
                REQUIRE_FALSE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_any);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                assign_mappings["v"] = Assignment("result");

                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                entity_name.set_name("result");
                modifiesS_constraint =
                    ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                statement_id.set_statement_id(4);
                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

                modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
                REQUIRE(modifiesS_constraint.is_valid(assign_mappings));
            }
        }
    }

    SECTION("call stmt") {
        auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
        auto call_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::CALL, 1);
        call_stmt_node->add_child(
            std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "logic")); // main calls logic
        auto read_stmt_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, 1);
        auto assign_stmt_node = std::make_shared<Parser::SimpleNode>(SimpleNodeType::ASSIGN, 2);
        assign_stmt_node->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
        auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
        assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
        assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
        assign_stmt_node->add_child(assign_rhs);

        pkb->add_procedure("main");
        pkb->add_procedure("logic");
        pkb->add_statement(StatementType::CALL, 1, "main", call_stmt_node);
        pkb->add_call_relationship("main", "logic");
        pkb->add_statement(StatementType::READ, 1, "main", read_stmt_node);
        pkb->add_statement(StatementType::ASSIGN, 2, "logic",
                           assign_stmt_node); // assign stmt in logic
        pkb->add_variable("y");               // assign stmt y = y + 1
        pkb->add_modify_relationship(1, "y");
        pkb->add_modify_relationship(2, "y"); // assign stmt in logic uses y

        // Assignment mappings
        assign_mappings["c"] = Assignment(1);
        assign_mappings["v"] = Assignment("y");

        statement_synonym.set_synonym("c");
        entity_synonym.set_synonym("v");
        auto modifiesS_constraint =
            ModifiesSConstraintLogic(pkb, statement_synonym, entity_synonym);
        REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

        entity_name.set_name("y");
        modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_synonym, entity_name);
        REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

        statement_id.set_statement_id(1);
        modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_synonym);
        REQUIRE(modifiesS_constraint.is_valid(assign_mappings));

        modifiesS_constraint = ModifiesSConstraintLogic(pkb, statement_id, entity_name);
        REQUIRE(modifiesS_constraint.is_valid(assign_mappings));
    }
}