#include "QueryEvaluator/constraints/WithConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/WithRef.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "QueryEvaluator/Assignment.h"
#include "catch.hpp"
#include <memory>

using namespace QueryEvaluator;
using namespace Parser;
using namespace KnowledgeBase;

TEST_CASE("WithConstraintLogic") {
    AssignmentMap assign_mappings;

    auto ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);

    // Name with ref
    auto with_name = Parser::WithRef();
    with_name.set_type(Parser::WithRefType::NAME);
    auto with_name_dup = Parser::WithRef();
    with_name_dup.set_type(Parser::WithRefType::NAME);
    // Synonym with ref
    auto with_synonym = Parser::WithRef();
    with_synonym.set_type(Parser::WithRefType::SYNONYM);
    auto with_synonym_dup = Parser::WithRef();
    with_synonym_dup.set_type(Parser::WithRefType::SYNONYM);
    // Statement ID with ref
    auto with_id = Parser::WithRef();
    with_id.set_type(Parser::WithRefType::STATEMENT_ID);
    auto with_id_dup = Parser::WithRef();
    with_id_dup.set_type(Parser::WithRefType::STATEMENT_ID);
    // Attr ref
    auto with_attr = Parser::WithRef();
    with_attr.set_type(Parser::WithRefType::ATTR_REF);
    auto with_attr_dup = Parser::WithRef();
    with_attr_dup.set_type(Parser::WithRefType::ATTR_REF);

    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    pkb->add_procedure("main");
    pkb->add_procedure("logic");

    assign_mappings["r"] = Assignment(1, "sum");  // read stmt: stmt #1 and varName sum
    assign_mappings["pr"] = Assignment(2, "sum"); // print stmt: stmt #2 and varName sum
    assign_mappings["p1"] = Assignment("main");   // procedure main
    assign_mappings["v"] = Assignment("main");    // variable main
    assign_mappings["a"] = Assignment(3);         // assign stmt#3 in procedure main
    assign_mappings["w1"] = Assignment(4);        // while stmt# 4
    assign_mappings["c1"] = Assignment(3);        // constant value 3
    assign_mappings["c2"] = Assignment(3);        // constant value 3
    assign_mappings["c3"] = Assignment(4);        // constant value 4

    SECTION("both string value") {
        SECTION("both var name") {
            // r.varName = "sum"
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("r");
            attr_ref.set_attr_name("varName");
            with_attr.set_attr_ref(attr_ref);
            with_name.set_name("sum");
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_name);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // r.varName = pr.varName
            auto attr_ref2 = Parser::AttrRef();
            attr_ref2.set_synonym("pr");
            attr_ref2.set_attr_name("varName");
            with_attr_dup.set_attr_ref(attr_ref2);
            with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // "sum" = "sum"
            with_name_dup.set_name("sum");
            with_constraint = WithConstraintLogic(pkb, with_name_dup, with_name);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            //"sum" = "max"
            with_name_dup.set_name("max");
            with_constraint = WithConstraintLogic(pkb, with_name_dup, with_name);
            REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));
        }

        SECTION("both proc name") {
            // p1.procName = "main"
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("p1");
            attr_ref.set_attr_name("procName");
            with_attr.set_attr_ref(attr_ref);
            with_name.set_name("main");
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_name);
            REQUIRE(with_constraint.is_valid(assign_mappings));
        }

        SECTION("var name and proc name") {
            // p1.procName = v.varName
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("p1");
            attr_ref.set_attr_name("procName");
            with_attr.set_attr_ref(attr_ref);
            auto attr_ref2 = Parser::AttrRef();
            attr_ref2.set_synonym("v");
            attr_ref2.set_attr_name("varName");
            with_attr_dup.set_attr_ref(attr_ref2);
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // p1.procName = pr.varName
            attr_ref2.set_synonym("pr");
            with_attr_dup.set_attr_ref(attr_ref2);
            with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("both int value") {
        SECTION("both statement id") {
            // r.stmt# = 1
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("r");
            attr_ref.set_attr_name("stmt#");
            with_attr.set_attr_ref(attr_ref);
            with_id.set_statement_id(1);
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // 1 = 1
            with_id_dup.set_statement_id(1);
            with_constraint = WithConstraintLogic(pkb, with_id_dup, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // 2 = 1
            with_id_dup.set_statement_id(2);
            with_constraint = WithConstraintLogic(pkb, with_id_dup, with_id);
            REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

            // r = 1
            with_synonym.set_synonym("r");
            with_constraint = WithConstraintLogic(pkb, with_synonym, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));
        }

        SECTION("both constant value") {
            // c1.value = 3
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("c1");
            attr_ref.set_attr_name("value");
            with_attr.set_attr_ref(attr_ref);
            with_id.set_statement_id(3);
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // c1.value = c2.value
            auto attr_ref2 = Parser::AttrRef();
            attr_ref2.set_synonym("c2");
            attr_ref2.set_attr_name("value");
            with_attr_dup.set_attr_ref(attr_ref2);
            with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // 3 = 3
            with_id_dup.set_statement_id(3);
            with_constraint = WithConstraintLogic(pkb, with_id_dup, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // 2 = 3
            with_id_dup.set_statement_id(2);
            with_constraint = WithConstraintLogic(pkb, with_id_dup, with_id);
            REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

            // a = 3
            with_synonym.set_synonym("a");
            with_constraint = WithConstraintLogic(pkb, with_synonym, with_id);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // c1.value = a
            with_constraint = WithConstraintLogic(pkb, with_attr, with_synonym);
            REQUIRE(with_constraint.is_valid(assign_mappings));
        }

        SECTION("statement id and constant value") {
            // a.stmt# = c1.value
            auto attr_ref = Parser::AttrRef();
            attr_ref.set_synonym("a");
            attr_ref.set_attr_name("stmt#");
            with_attr.set_attr_ref(attr_ref);
            auto attr_ref2 = Parser::AttrRef();
            attr_ref2.set_synonym("c1");
            attr_ref2.set_attr_name("value");
            with_attr_dup.set_attr_ref(attr_ref2);
            auto with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE(with_constraint.is_valid(assign_mappings));

            // a.stmt# = c3.value
            attr_ref2.set_synonym("c3");
            with_attr_dup.set_attr_ref(attr_ref2);
            with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
            REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));
        }
    }

    SECTION("one int one string") {
        with_id.set_statement_id(1);

        auto attr_ref = Parser::AttrRef();
        attr_ref.set_synonym("r");
        attr_ref.set_attr_name("varName");
        with_attr.set_attr_ref(attr_ref);
        auto with_constraint = WithConstraintLogic(pkb, with_attr, with_id);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        attr_ref.set_synonym("p1");
        attr_ref.set_attr_name("procName");
        with_attr.set_attr_ref(attr_ref);
        with_constraint = WithConstraintLogic(pkb, with_attr, with_id);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_name.set_name("sum");
        with_constraint = WithConstraintLogic(pkb, with_name, with_id);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_synonym.set_synonym("a");
        with_constraint = WithConstraintLogic(pkb, with_attr, with_synonym);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_attr, with_synonym);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_name, with_synonym);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        auto attr_ref2 = Parser::AttrRef();
        attr_ref2.set_synonym("r");
        attr_ref2.set_attr_name("stmt#");
        with_attr_dup.set_attr_ref(attr_ref2);
        with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_name, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        auto attr_ref3 = Parser::AttrRef();
        attr_ref3.set_synonym("c1");
        attr_ref3.set_attr_name("value");
        with_attr_dup.set_attr_ref(attr_ref3);
        with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_attr, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));

        with_constraint = WithConstraintLogic(pkb, with_name, with_attr_dup);
        REQUIRE_FALSE(with_constraint.is_valid(assign_mappings));
    }
}
