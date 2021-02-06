#include "catch.hpp"

#include "Parser/PQLParser/PQLParser.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/shared/StringLexer.h"
#include <iterator>
#include <map>
#include <string>

using namespace Parser;

TEST_CASE("Parser::PQLParser") {
    SECTION("Without clause") {
        SECTION("One design entity, one variable") {
            std::map<std::string, DesignEntityType> design_entity_map{
                {"stmt", DesignEntityType::STMT},
                {"read", DesignEntityType::READ},
                {"print", DesignEntityType::PRINT},
                {"while", DesignEntityType::WHILE},
                {"if", DesignEntityType::IF},
                {"assign", DesignEntityType::ASSIGN},
                {"variable", DesignEntityType::VARIABLE},
                {"constant", DesignEntityType::CONSTANT},
                {"procedure", DesignEntityType::PROCEDURE}};

            std::string op = GENERATE("stmt", "read", "print", "while", "if", "assign", "variable",
                                      "constant", "procedure");

            std::string query = op + " s; Select s";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.find("s") != declarations.end());
            REQUIRE(declarations["s"].get_synonym() == "s");
            REQUIRE(declarations["s"].get_type() == design_entity_map[op]);

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            delete query_object;
        }

        SECTION("One design entity, multiple variable") {
            std::string query = "stmt s,u,v; Select s";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 3);
            REQUIRE(declarations.find("s") != declarations.end());
            REQUIRE(declarations.find("u") != declarations.end());
            REQUIRE(declarations.find("v") != declarations.end());
            REQUIRE(declarations["s"].get_synonym() == "s");
            REQUIRE(declarations["u"].get_synonym() == "u");
            REQUIRE(declarations["v"].get_synonym() == "v");
            REQUIRE(declarations["s"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["u"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["v"].get_type() == DesignEntityType::STMT);

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            delete query_object;
        }

        SECTION("Multiple design entity, one variable") {
            std::string query = "stmt s; if ifs; while w; Select w";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 3);
            REQUIRE(declarations.find("s") != declarations.end());
            REQUIRE(declarations.find("ifs") != declarations.end());
            REQUIRE(declarations.find("w") != declarations.end());
            REQUIRE(declarations["s"].get_synonym() == "s");
            REQUIRE(declarations["ifs"].get_synonym() == "ifs");
            REQUIRE(declarations["w"].get_synonym() == "w");
            REQUIRE(declarations["s"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["ifs"].get_type() == DesignEntityType::IF);
            REQUIRE(declarations["w"].get_type() == DesignEntityType::WHILE);

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "w");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            delete query_object;
        }

        SECTION("Multiple design entity, multiple variable") {
            std::string query = "stmt s,u,v; if ifs; variable var1, var2; Select var1";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 6);

            std::string var[] = {"s", "u", "v", "ifs", "var1", "var2"};
            for (const std::string& text : var) {
                REQUIRE(declarations.find(text) != declarations.end());
                REQUIRE(declarations[text].get_synonym() == text);
            }

            REQUIRE(declarations["s"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["u"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["v"].get_type() == DesignEntityType::STMT);
            REQUIRE(declarations["ifs"].get_type() == DesignEntityType::IF);
            REQUIRE(declarations["var1"].get_type() == DesignEntityType::VARIABLE);
            REQUIRE(declarations["var2"].get_type() == DesignEntityType::VARIABLE);

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "var1");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            delete query_object;
        }
    }

    SECTION("One Such That clause") {
        std::map<std::string, SuchThatType> such_that_map{
            {"Modifies", SuchThatType::MODIFIES_S}, {"Uses", SuchThatType::USES_S},
            {"Parent", SuchThatType::PARENT},       {"Parent*", SuchThatType::PARENT_T},
            {"Follows", SuchThatType::FOLLOWS},     {"Follows*", SuchThatType::FOLLOWS_T},
        };

        SECTION("Parent/Parent* and Follows/Follows* type") {
            std::string op = GENERATE("Parent", "Parent*", "Follows", "Follows*");

            std::string query = "while w; Select w such that " + op + "(w, 7)";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.find("w") != declarations.end());

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "w");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            SuchThat such_that_obj = query_object->get_such_that();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            Ref left_ref = such_that_obj.get_left_ref();
            Ref right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == RefType::STATEMENT);
            REQUIRE(right_ref.get_type() == RefType::STATEMENT);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            StatementRef right_statement_ref = right_ref.get_statement_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "w");
            REQUIRE(right_statement_ref.get_type() == StatementRefType::STATEMENT_ID);
            REQUIRE(right_statement_ref.get_statement_id() == 7);

            delete query_object;
        }

        SECTION("Modifies and Uses type") {
            std::string op = GENERATE("Modifies", "Uses");

            std::string query = "stmt s; Select s such that " + op + "(s, \"i\")";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.find("s") != declarations.end());

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            SuchThat such_that_obj = query_object->get_such_that();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            Ref left_ref = such_that_obj.get_left_ref();
            Ref right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == RefType::STATEMENT);
            REQUIRE(right_ref.get_type() == RefType::ENTITY);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "s");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref.get_name() == "i");

            delete query_object;
        }

        SECTION("StatementRefType and EntityRefType combinations") {
            std::string query_1 = "stmt s; Select s such that Modifies(_, _)";
            std::string query_2 = "stmt s; Select s such that Modifies(s, s)";
            std::string query_3 = "stmt s; Select s such that Modifies(7, \"x\")";
            Parser::StringLexer lexer_1(query_1);
            Parser::PQLParser parser_1(lexer_1.tokens());
            Parser::StringLexer lexer_2(query_2);
            Parser::PQLParser parser_2(lexer_2.tokens());
            Parser::StringLexer lexer_3(query_3);
            Parser::PQLParser parser_3(lexer_3.tokens());
            QueryObject* query_object_1 = parser_1.parse_query();
            QueryObject* query_object_2 = parser_2.parse_query();
            QueryObject* query_object_3 = parser_3.parse_query();

            SuchThat such_that_obj_1 = query_object_1->get_such_that();
            SuchThat such_that_obj_2 = query_object_2->get_such_that();
            SuchThat such_that_obj_3 = query_object_3->get_such_that();
            Ref left_ref_1 = such_that_obj_1.get_left_ref();
            Ref right_ref_1 = such_that_obj_1.get_right_ref();
            Ref left_ref_2 = such_that_obj_2.get_left_ref();
            Ref right_ref_2 = such_that_obj_2.get_right_ref();
            Ref left_ref_3 = such_that_obj_3.get_left_ref();
            Ref right_ref_3 = such_that_obj_3.get_right_ref();

            StatementRef left_statement_ref_1 = left_ref_1.get_statement_ref();
            EntityRef right_entity_ref_1 = right_ref_1.get_entity_ref();
            REQUIRE(left_statement_ref_1.get_type() == StatementRefType::ANY);
            REQUIRE(right_entity_ref_1.get_type() == EntityRefType::ANY);

            StatementRef left_statement_ref_2 = left_ref_2.get_statement_ref();
            EntityRef right_entity_ref_2 = right_ref_2.get_entity_ref();
            REQUIRE(left_statement_ref_2.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref_2.get_synonym() == "s");
            REQUIRE(right_entity_ref_2.get_type() == EntityRefType::SYNONYM);
            REQUIRE(right_entity_ref_2.get_synonym() == "s");

            StatementRef left_statement_ref_3 = left_ref_3.get_statement_ref();
            EntityRef right_entity_ref_3 = right_ref_3.get_entity_ref();
            REQUIRE(left_statement_ref_3.get_type() == StatementRefType::STATEMENT_ID);
            REQUIRE(left_statement_ref_3.get_statement_id() == 7);
            REQUIRE(right_entity_ref_3.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref_3.get_name() == "x");

            delete query_object_1;
            delete query_object_2;
            delete query_object_3;
        }
    }

    SECTION("One Pattern clause") {
        SECTION("General pattern test") {
            std::string query = "assign a; Select a pattern a(_, _\"x + 1\"_)";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();

            Parser::StringLexer factor_lexer("x+1");
            Parser::SimpleNode* pattern_node = SimpleParser(factor_lexer).parse_expr();

            std::unordered_map<std::string, DesignEntity> declarations =
                query_object->get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.find("a") != declarations.end());

            std::string selection = query_object->get_selection();
            REQUIRE(selection == "a");

            bool has_such_that_cl = query_object->has_such_that();
            bool has_pattern_cl = query_object->has_pattern();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE(has_pattern_cl);

            Pattern pattern_obj = query_object->get_pattern();
            std::string assigned_synonym = pattern_obj.get_assigned_synonym();
            REQUIRE(assigned_synonym == "a");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(entity_ref.get_type() == EntityRefType::ANY);
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
            REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));

            delete query_object;
            delete pattern_node;
        }

        SECTION("ExpressionSpecType of type ANY") {
            std::string query = "assign a; Select a pattern a(_, _)";
            Parser::StringLexer lexer(query);
            Parser::PQLParser parser(lexer.tokens());
            QueryObject* query_object = parser.parse_query();
            Pattern pattern_obj = query_object->get_pattern();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::ANY);

            delete query_object;
        }
    }

    SECTION("One Such That clause, One Pattern clause") {
        std::string query =
            "variable v; assign a; while w; Select w such that Uses(a, v) pattern a(v, _\"z\"_)";
        Parser::StringLexer lexer(query);
        Parser::PQLParser parser(lexer.tokens());
        QueryObject* query_object = parser.parse_query();

        Parser::StringLexer factor_lexer("z");
        Parser::SimpleNode* pattern_node = SimpleParser(factor_lexer.tokens()).parse_expr();

        std::unordered_map<std::string, DesignEntity> declarations =
            query_object->get_declarations();
        REQUIRE(declarations.size() == 3);
        REQUIRE(declarations.find("v") != declarations.end());
        REQUIRE(declarations.find("a") != declarations.end());
        REQUIRE(declarations.find("w") != declarations.end());
        REQUIRE(declarations["v"].get_type() == DesignEntityType::VARIABLE);
        REQUIRE(declarations["a"].get_type() == DesignEntityType::ASSIGN);
        REQUIRE(declarations["w"].get_type() == DesignEntityType::WHILE);

        std::string selection = query_object->get_selection();
        REQUIRE(selection == "w");

        bool has_such_that_cl = query_object->has_such_that();
        bool has_pattern_cl = query_object->has_pattern();
        REQUIRE(has_such_that_cl);
        REQUIRE(has_pattern_cl);

        SuchThat such_that_obj = query_object->get_such_that();
        REQUIRE(such_that_obj.get_type() == SuchThatType::USES_S);

        Ref left_ref = such_that_obj.get_left_ref();
        Ref right_ref = such_that_obj.get_right_ref();
        REQUIRE(left_ref.get_type() == RefType::STATEMENT);
        REQUIRE(right_ref.get_type() == RefType::ENTITY);

        StatementRef left_statement_ref = left_ref.get_statement_ref();
        EntityRef right_entity_ref = right_ref.get_entity_ref();
        REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
        REQUIRE(left_statement_ref.get_synonym() == "a");
        REQUIRE(right_entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(right_entity_ref.get_synonym() == "v");

        Pattern pattern_obj = query_object->get_pattern();
        std::string assigned_synonym = pattern_obj.get_assigned_synonym();
        REQUIRE(assigned_synonym == "a");

        EntityRef entity_ref = pattern_obj.get_entity_ref();
        ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
        REQUIRE(entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
        REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));

        delete query_object;
        delete pattern_node;
    }
}