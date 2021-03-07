#include "catch.hpp"

#include "Parser/PQLParser/PQLLexer.h"
#include "Parser/PQLParser/PQLParser.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include <iterator>
#include <string>
#include <unordered_map>

using namespace Parser;

TEST_CASE("Parser::PQLParser") {
    SECTION("Without clause") {
        SECTION("One design entity, one variable") {
            std::unordered_map<std::string, DesignEntityType> design_entity_map{
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
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("s"));
            REQUIRE(declarations.get("s").get_synonym() == "s");
            REQUIRE(declarations.get("s").get_type() == design_entity_map[op]);

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);
        }

        SECTION("One design entity, multiple variable") {
            std::string query = "stmt s,u,v; Select s";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 3);
            REQUIRE(declarations.has("s"));
            REQUIRE(declarations.has("u"));
            REQUIRE(declarations.has("v"));
            REQUIRE(declarations.get("s").get_synonym() == "s");
            REQUIRE(declarations.get("u").get_synonym() == "u");
            REQUIRE(declarations.get("v").get_synonym() == "v");
            REQUIRE(declarations.get("s").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("u").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("v").get_type() == DesignEntityType::STMT);

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);
        }

        SECTION("Multiple design entity, one variable") {
            std::string query = "stmt s; if ifs; while w; Select w";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 3);
            REQUIRE(declarations.has("s"));
            REQUIRE(declarations.has("ifs"));
            REQUIRE(declarations.has("w"));
            REQUIRE(declarations.get("s").get_synonym() == "s");
            REQUIRE(declarations.get("ifs").get_synonym() == "ifs");
            REQUIRE(declarations.get("w").get_synonym() == "w");
            REQUIRE(declarations.get("s").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("ifs").get_type() == DesignEntityType::IF);
            REQUIRE(declarations.get("w").get_type() == DesignEntityType::WHILE);

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "w");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);
        }

        SECTION("Multiple design entity, multiple variable") {
            std::string query = "stmt s,u,v; if ifs; variable var1, var2; Select var1";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 6);

            std::string var[] = {"s", "u", "v", "ifs", "var1", "var2"};
            for (const std::string& text : var) {
                REQUIRE(declarations.has(text));
                REQUIRE(declarations.get(text).get_synonym() == text);
            }

            REQUIRE(declarations.get("s").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("u").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("v").get_type() == DesignEntityType::STMT);
            REQUIRE(declarations.get("ifs").get_type() == DesignEntityType::IF);
            REQUIRE(declarations.get("var1").get_type() == DesignEntityType::VARIABLE);
            REQUIRE(declarations.get("var2").get_type() == DesignEntityType::VARIABLE);

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "var1");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);
        }
    }

    SECTION("One Such That clause") {
        std::unordered_map<std::string, SuchThatType> such_that_map{
            {"Modifies", SuchThatType::MODIFIES_S}, {"Uses", SuchThatType::USES_S},
            {"Parent", SuchThatType::PARENT},       {"Parent*", SuchThatType::PARENT_T},
            {"Follows", SuchThatType::FOLLOWS},     {"Follows*", SuchThatType::FOLLOWS_T},
        };

        SECTION("Parent/Parent* and Follows/Follows* type") {
            std::string op = GENERATE("Parent", "Parent*", "Follows", "Follows*");

            std::string query = "while w; Select w such that " + op + "(w, 7)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("w"));

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "w");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            std::vector<SuchThat> such_that_vector = query_object.get_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == RefType::STATEMENT);
            REQUIRE(right_ref.get_type() == RefType::STATEMENT);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            StatementRef right_statement_ref = right_ref.get_statement_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "w");
            REQUIRE(right_statement_ref.get_type() == StatementRefType::STATEMENT_ID);
            REQUIRE(right_statement_ref.get_statement_id() == 7);
        }

        SECTION("Modifies and Uses type") {
            std::string op = GENERATE("Modifies", "Uses");

            std::string query = "stmt s; Select s such that " + op + "(s, \"i\")";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("s"));

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "s");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            std::vector<SuchThat> such_that_vector = query_object.get_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == RefType::STATEMENT);
            REQUIRE(right_ref.get_type() == RefType::ENTITY);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "s");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref.get_name() == "i");
        }

        SECTION("Quirky such that clause") {
            std::string query = "stmt such; Select such such that Modifies(such, \"such\")";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("such"));

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "such");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE(has_such_that_cl);
            REQUIRE_FALSE(has_pattern_cl);

            std::vector<SuchThat> such_that_vector = query_object.get_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map["Modifies"]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == RefType::STATEMENT);
            REQUIRE(right_ref.get_type() == RefType::ENTITY);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "such");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref.get_name() == "such");
        }

        SECTION("StatementRefType and EntityRefType combinations") {
            std::string query_1 = "stmt s; Select s such that Modifies(s, _)";
            std::string query_2 = "stmt s; variable v; Select s such that Modifies(s, v)";
            std::string query_3 = "stmt s; Select s such that Modifies(7, \"x\")";
            auto source_1 = std::make_shared<Parser::Source>(query_1);
            Parser::PQLParser parser_1(source_1);
            auto source_2 = std::make_shared<Parser::Source>(query_2);
            Parser::PQLParser parser_2(source_2);
            auto source_3 = std::make_shared<Parser::Source>(query_3);
            Parser::PQLParser parser_3(source_3);
            auto query_object_1 = parser_1.parse_query();
            auto query_object_2 = parser_2.parse_query();
            auto query_object_3 = parser_3.parse_query();

            std::vector<SuchThat> such_that_vector_1 = query_object_1.get_such_that();
            SuchThat such_that_obj_1 = such_that_vector_1.front();
            std::vector<SuchThat> such_that_vector_2 = query_object_2.get_such_that();
            SuchThat such_that_obj_2 = such_that_vector_2.front();
            std::vector<SuchThat> such_that_vector_3 = query_object_3.get_such_that();
            SuchThat such_that_obj_3 = such_that_vector_3.front();
            SuchThatRef left_ref_1 = such_that_obj_1.get_left_ref();
            SuchThatRef right_ref_1 = such_that_obj_1.get_right_ref();
            SuchThatRef left_ref_2 = such_that_obj_2.get_left_ref();
            SuchThatRef right_ref_2 = such_that_obj_2.get_right_ref();
            SuchThatRef left_ref_3 = such_that_obj_3.get_left_ref();
            SuchThatRef right_ref_3 = such_that_obj_3.get_right_ref();

            StatementRef left_statement_ref_1 = left_ref_1.get_statement_ref();
            EntityRef right_entity_ref_1 = right_ref_1.get_entity_ref();
            REQUIRE(left_statement_ref_1.get_type() == StatementRefType::SYNONYM);
            REQUIRE(right_entity_ref_1.get_type() == EntityRefType::ANY);

            StatementRef left_statement_ref_2 = left_ref_2.get_statement_ref();
            EntityRef right_entity_ref_2 = right_ref_2.get_entity_ref();
            REQUIRE(left_statement_ref_2.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref_2.get_synonym() == "s");
            REQUIRE(right_entity_ref_2.get_type() == EntityRefType::SYNONYM);
            REQUIRE(right_entity_ref_2.get_synonym() == "v");

            StatementRef left_statement_ref_3 = left_ref_3.get_statement_ref();
            EntityRef right_entity_ref_3 = right_ref_3.get_entity_ref();
            REQUIRE(left_statement_ref_3.get_type() == StatementRefType::STATEMENT_ID);
            REQUIRE(left_statement_ref_3.get_statement_id() == 7);
            REQUIRE(right_entity_ref_3.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref_3.get_name() == "x");
        }
    }

    SECTION("One PatternAssign clause") {
        SECTION("General pattern test") {
            std::string query = "assign a; Select a pattern a(_, _\"x\"_)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto factor_source = std::make_shared<Parser::Source>("x");
            auto pattern_node = SimpleParser(factor_source).parse_expr();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("a"));

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "a");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE(has_pattern_cl);

            std::vector<PatternAssign> pattern_vector = query_object.get_pattern();
            PatternAssign pattern_obj = pattern_vector.front();
            std::string assigned_synonym = pattern_obj.get_assigned_synonym();
            REQUIRE(assigned_synonym == "a");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(entity_ref.get_type() == EntityRefType::ANY);
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
            REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));
        }

        SECTION("Quirky pattern clause") {
            std::string query = "assign pattern; variable Pattern; Select pattern pattern "
                                "pattern(Pattern, _\"123\"_)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto factor_source = std::make_shared<Parser::Source>("123");
            auto pattern_node = SimpleParser(factor_source).parse_expr();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 2);
            REQUIRE(declarations.has("pattern"));
            REQUIRE(declarations.has("PatternAssign"));

            std::string selection = query_object.get_selection();
            REQUIRE(selection == "pattern");

            bool has_such_that_cl = !query_object.get_such_that().empty();
            bool has_pattern_cl = !query_object.get_pattern().empty();
            REQUIRE_FALSE(has_such_that_cl);
            REQUIRE(has_pattern_cl);

            std::vector<PatternAssign> pattern_vector = query_object.get_pattern();
            PatternAssign pattern_obj = pattern_vector.front();
            std::string assigned_synonym = pattern_obj.get_assigned_synonym();
            REQUIRE(assigned_synonym == "pattern");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(entity_ref.get_type() == EntityRefType::SYNONYM);
            REQUIRE(entity_ref.get_synonym() == "PatternAssign");
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
            REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));
        }

        SECTION("ExpressionSpecType of type ANY") {
            std::string query = "assign a; Select a pattern a(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();
            std::vector<PatternAssign> pattern_vector = query_object.get_pattern();
            PatternAssign pattern_obj = pattern_vector.front();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::ANY);
        }
    }

    SECTION("One Such That clause, One PatternAssign clause") {
        std::string query =
            "variable v; assign a; while w; Select w such that Uses(a, v) pattern a(v, _\"z\"_)";
        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        auto query_object = parser.parse_query();

        auto factor_source = std::make_shared<Parser::Source>("z");
        auto pattern_node = SimpleParser(factor_source).parse_expr();

        auto declarations = query_object.get_declarations();
        REQUIRE(declarations.size() == 3);
        REQUIRE(declarations.has("v"));
        REQUIRE(declarations.has("a"));
        REQUIRE(declarations.has("w"));
        REQUIRE(declarations.get("v").get_type() == DesignEntityType::VARIABLE);
        REQUIRE(declarations.get("a").get_type() == DesignEntityType::ASSIGN);
        REQUIRE(declarations.get("w").get_type() == DesignEntityType::WHILE);

        std::string selection = query_object.get_selection();
        REQUIRE(selection == "w");

        bool has_such_that_cl = !query_object.get_such_that().empty();
        bool has_pattern_cl = !query_object.get_pattern().empty();
        REQUIRE(has_such_that_cl);
        REQUIRE(has_pattern_cl);

        std::vector<SuchThat> such_that_vector = query_object.get_such_that();
        SuchThat such_that_obj = such_that_vector.front();
        REQUIRE(such_that_obj.get_type() == SuchThatType::USES_S);

        SuchThatRef left_ref = such_that_obj.get_left_ref();
        SuchThatRef right_ref = such_that_obj.get_right_ref();
        REQUIRE(left_ref.get_type() == RefType::STATEMENT);
        REQUIRE(right_ref.get_type() == RefType::ENTITY);

        StatementRef left_statement_ref = left_ref.get_statement_ref();
        EntityRef right_entity_ref = right_ref.get_entity_ref();
        REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
        REQUIRE(left_statement_ref.get_synonym() == "a");
        REQUIRE(right_entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(right_entity_ref.get_synonym() == "v");

        std::vector<PatternAssign> pattern_vector = query_object.get_pattern();
        PatternAssign pattern_obj = pattern_vector.front();
        std::string assigned_synonym = pattern_obj.get_assigned_synonym();
        REQUIRE(assigned_synonym == "a");

        EntityRef entity_ref = pattern_obj.get_entity_ref();
        ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
        REQUIRE(entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
        REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));
    }

    SECTION("Syntactic validation") {
        SECTION("Validate such that clause") {
            std::string query = "stmt s; Select s such  that Modifies(s, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }
    }

    SECTION("Semantic validation") {
        SECTION("Cannot have duplicate synonym") {
            std::string query = "stmt s, s; Select s";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());

            std::string query_1 = "stmt s; assign s; Select s";
            auto source_1 = std::make_shared<Parser::Source>(query_1);
            Parser::PQLParser parser_1(source_1);
            REQUIRE_THROWS(parser_1.parse_query());
        }

        SECTION("Synonym cannot be missing from the declaration") {
            std::string query = "stmt s; Select s1";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());

            std::string query_1 = "stmt s; Select s such that Follows(s, s1)";
            auto source_1 = std::make_shared<Parser::Source>(query_1);
            Parser::PQLParser parser_1(source_1);
            REQUIRE_THROWS(parser_1.parse_query());

            std::string query_2 = "stmt s; Select s such that Follows(s1, s)";
            auto source_2 = std::make_shared<Parser::Source>(query_2);
            Parser::PQLParser parser_2(source_2);
            REQUIRE_THROWS(parser_2.parse_query());

            std::string query_3 = "assign a; Select a pattern a(_, s1)";
            auto source_3 = std::make_shared<Parser::Source>(query_3);
            Parser::PQLParser parser_3(source_3);
            REQUIRE_THROWS(parser_3.parse_query());

            std::string query_4 = "assign a; Select a pattern a(s1, _)";
            auto source_4 = std::make_shared<Parser::Source>(query_4);
            Parser::PQLParser parser_4(source_4);
            REQUIRE_THROWS(parser_4.parse_query());

            std::string query_5 = "assign a; Select a pattern a1(_, _)";
            auto source_5 = std::make_shared<Parser::Source>(query_5);
            Parser::PQLParser parser_5(source_5);
            REQUIRE_THROWS(parser_5.parse_query());
        }

        SECTION("PatternAssign synonym must be assign") {
            std::string query = "stmt s; Select s pattern s(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("PatternAssign LHS must be variable") {
            std::string query = "assign a; stmt s; Select a pattern a(s, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Modifies, Uses LHS must not be ANY") {
            std::string op = GENERATE("Uses", "Modifies");
            std::string query = "stmt s; Select s such that " + op + "(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Modifies, Uses, Follows, Follows*, Parent, Parent* LHS must be statement") {
            std::string op =
                GENERATE("Uses", "Modifies", "Follows", "Follows*", "Parent", "Parent*");
            std::string query = "variable v; Select v such that " + op + "(v, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Modifies, Uses RHS must be variable") {
            std::string op = GENERATE("Uses", "Modifies");
            std::string query = "stmt s; Select s such that " + op + "(s, s)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Follows, Follows*, Parent, Parent* RHS must be statement") {
            std::string op = GENERATE("Follows", "Follows*", "Parent", "Parent*");
            std::string query = "variable v; Select v such that " + op + "(_, v)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }
    }
}