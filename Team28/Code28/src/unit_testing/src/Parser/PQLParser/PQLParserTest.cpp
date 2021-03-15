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
                {"procedure", DesignEntityType::PROCEDURE},
                {"call", DesignEntityType::CALL},
                {"prog_line", DesignEntityType::PROG_LINE}};

            std::string op = GENERATE("stmt", "read", "print", "while", "if", "assign", "variable",
                                      "constant", "procedure", "call", "prog_line");

            std::string query = op + " s; Select s";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("s"));
            REQUIRE(declarations.get("s").get_synonym() == "s");
            REQUIRE(declarations.get("s").get_type() == design_entity_map[op]);

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "s");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());
        }

        SECTION("One design entity, multiple variable") {
            std::string query = "stmt s,u,v; Select <s,u,v>";
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

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();

            std::string var[] = {"s", "u", "v"};
            for (int i = 0; i < tmp_tuple.size(); i++) {
                REQUIRE(tmp_tuple[i].get_type() == ElemType::SYNONYM);
                REQUIRE(tmp_tuple[i].get_synonym() == var[i]);
            }

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());
        }

        SECTION("Multiple design entity, one variable") {
            std::string query = "stmt s; if ifs; while w; Select BOOLEAN";
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

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::BOOLEAN);

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());
        }

        SECTION("Multiple design entity, multiple variable") {
            std::string query = "constant s,u,v; read r1, r2; procedure p1, p2; call c1, c2;";
            query += "Select <p1.procName, c1.procName, r1.varName, r1.stmt#, s.value, u.value, v>";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 9);

            std::string var[] = {"s", "u", "v", "r1", "r2", "p1", "p2", "c1", "c2"};
            for (const std::string& text : var) {
                REQUIRE(declarations.has(text));
                REQUIRE(declarations.get(text).get_synonym() == text);
            }

            REQUIRE(declarations.get("s").get_type() == DesignEntityType::CONSTANT);
            REQUIRE(declarations.get("u").get_type() == DesignEntityType::CONSTANT);
            REQUIRE(declarations.get("v").get_type() == DesignEntityType::CONSTANT);
            REQUIRE(declarations.get("r1").get_type() == DesignEntityType::READ);
            REQUIRE(declarations.get("r2").get_type() == DesignEntityType::READ);
            REQUIRE(declarations.get("p1").get_type() == DesignEntityType::PROCEDURE);
            REQUIRE(declarations.get("p2").get_type() == DesignEntityType::PROCEDURE);
            REQUIRE(declarations.get("c1").get_type() == DesignEntityType::CALL);
            REQUIRE(declarations.get("c2").get_type() == DesignEntityType::CALL);

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();

            std::string synonym[] = {"p1", "c1", "r1", "r1", "s", "u"};
            std::string attr_name[] = {"procName", "procName", "varName",
                                       "stmt#",    "value",    "value"};

            for (int i = 0; i < tmp_tuple.size() - 1; i++) {
                REQUIRE(tmp_tuple[i].get_type() == ElemType::ATTR_REF);
                REQUIRE(tmp_tuple[i].get_attr_ref().get_synonym() == synonym[i]);
                REQUIRE(tmp_tuple[i].get_attr_ref().get_attr_name() == attr_name[i]);
            }

            int last_index = tmp_tuple.size() - 1;
            REQUIRE(tmp_tuple[last_index].get_type() == ElemType::SYNONYM);
            REQUIRE(tmp_tuple[last_index].get_synonym() == "v");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());
        }
    }

    SECTION("One Such That clause") {
        std::unordered_map<std::string, SuchThatType> such_that_map{
            {"Modifies", SuchThatType::MODIFIES_S},
            {"Uses", SuchThatType::USES_S},
            {"ModifiesP", SuchThatType::MODIFIES_P},
            {"UsesP", SuchThatType::USES_P},
            {"Parent", SuchThatType::PARENT},
            {"Parent*", SuchThatType::PARENT_T},
            {"Follows", SuchThatType::FOLLOWS},
            {"Follows*", SuchThatType::FOLLOWS_T},
            {"Calls", SuchThatType::CALLS},
            {"Calls*", SuchThatType::CALLS_T},
            {"Next", SuchThatType::NEXT},
            {"Next*", SuchThatType::NEXT_T},
            {"Affects", SuchThatType::AFFECTS},
            {"Affects*", SuchThatType::AFFECTS_T},
        };

        SECTION("Parent/Parent*/Follows/Follows*/Next/Next*/Affects/Affects* type") {
            std::string op = GENERATE("Parent", "Parent*", "Follows", "Follows*", "Next", "Next*",
                                      "Affects", "Affects*");

            std::string query = "prog_line pg; Select pg such that " + op + "(pg, 7)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("pg"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "pg");

            REQUIRE(query_object.get_all_such_that().size() == 1);
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());

            std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == SuchThatRefType::STATEMENT);
            REQUIRE(right_ref.get_type() == SuchThatRefType::STATEMENT);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            StatementRef right_statement_ref = right_ref.get_statement_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "pg");
            REQUIRE(right_statement_ref.get_type() == StatementRefType::STATEMENT_ID);
            REQUIRE(right_statement_ref.get_statement_id() == 7);
        }

        SECTION("Calls and Calls* type") {
            std::string op = GENERATE("Calls", "Calls*");
            std::string query = "procedure p1,p2,p3; Select p3 such that " + op + "(p1, p2)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 3);
            REQUIRE(declarations.has("p1"));
            REQUIRE(declarations.has("p2"));
            REQUIRE(declarations.has("p3"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "p3");

            REQUIRE(query_object.get_all_such_that().size() == 1);
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());

            std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == SuchThatRefType::ENTITY);
            REQUIRE(right_ref.get_type() == SuchThatRefType::ENTITY);

            EntityRef left_entity_ref = left_ref.get_entity_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_entity_ref.get_type() == EntityRefType::SYNONYM);
            REQUIRE(left_entity_ref.get_synonym() == "p1");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::SYNONYM);
            REQUIRE(right_entity_ref.get_synonym() == "p2");
        }

        SECTION("ModifiesS and UsesS type") {
            std::string op = GENERATE("Modifies", "Uses");

            std::string query = "stmt s; Select s such that " + op + "(s, \"i\")";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("s"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "s");

            REQUIRE(query_object.get_all_such_that().size() == 1);
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());

            std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == SuchThatRefType::STATEMENT);
            REQUIRE(right_ref.get_type() == SuchThatRefType::ENTITY);

            StatementRef left_statement_ref = left_ref.get_statement_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
            REQUIRE(left_statement_ref.get_synonym() == "s");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::NAME);
            REQUIRE(right_entity_ref.get_name() == "i");
        }

        SECTION("ModifiesP and UsesP type") {
            std::string op = GENERATE("Modifies", "Uses");

            std::string query = "procedure p; Select BOOLEAN such that " + op + "(p, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("p"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::BOOLEAN);

            REQUIRE(query_object.get_all_such_that().size() == 1);
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());

            std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map[op + "P"]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == SuchThatRefType::ENTITY);
            REQUIRE(right_ref.get_type() == SuchThatRefType::ENTITY);

            EntityRef left_entity_ref = left_ref.get_entity_ref();
            EntityRef right_entity_ref = right_ref.get_entity_ref();
            REQUIRE(left_entity_ref.get_type() == EntityRefType::SYNONYM);
            REQUIRE(left_entity_ref.get_synonym() == "p");
            REQUIRE(right_entity_ref.get_type() == EntityRefType::ANY);
        }

        SECTION("Quirky such that clause") {
            std::string query = "stmt such; Select such such that Modifies(such, \"such\")";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("such"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "such");

            REQUIRE(query_object.get_all_such_that().size() == 1);
            REQUIRE(query_object.get_all_pattern().empty());
            REQUIRE(query_object.get_all_with().empty());

            std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
            SuchThat such_that_obj = such_that_vector.front();
            REQUIRE(such_that_obj.get_type() == such_that_map["Modifies"]);

            SuchThatRef left_ref = such_that_obj.get_left_ref();
            SuchThatRef right_ref = such_that_obj.get_right_ref();
            REQUIRE(left_ref.get_type() == SuchThatRefType::STATEMENT);
            REQUIRE(right_ref.get_type() == SuchThatRefType::ENTITY);

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

            std::vector<SuchThat> such_that_vector_1 = query_object_1.get_all_such_that();
            SuchThat such_that_obj_1 = such_that_vector_1.front();
            std::vector<SuchThat> such_that_vector_2 = query_object_2.get_all_such_that();
            SuchThat such_that_obj_2 = such_that_vector_2.front();
            std::vector<SuchThat> such_that_vector_3 = query_object_3.get_all_such_that();
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
            std::string query = "assign a; Select a pattern a(_, _\"x + y * z\"_)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto factor_source = std::make_shared<Parser::Source>("x+y*z");
            auto pattern_node = SimpleParser(factor_source).parse_expr();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("a"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "a");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().size() == 1);
            REQUIRE(query_object.get_all_with().empty());

            std::vector<Pattern> pattern = query_object.get_all_pattern();
            REQUIRE(pattern.size() == 1);
            REQUIRE(pattern.front().get_type() == PatternType::ASSIGN);
            PatternAssign pattern_obj = pattern.front().get_pattern_assign();
            std::string assigned_synonym = pattern_obj.get_assign_synonym();
            REQUIRE(assigned_synonym == "a");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(entity_ref.get_type() == EntityRefType::ANY);
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::PATTERN);
            REQUIRE(expression_spec.get_pattern()->is_equal(pattern_node));
        }

        SECTION("Quirky pattern clause") {
            std::string query = "assign pattern; variable Pattern; Select pattern pattern "
                                "pattern(Pattern, \"pattern\")";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto factor_source = std::make_shared<Parser::Source>("pattern");
            auto pattern_node = SimpleParser(factor_source).parse_expr();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 2);
            REQUIRE(declarations.has("pattern"));
            REQUIRE(declarations.has("Pattern"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "pattern");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().size() == 1);
            REQUIRE(query_object.get_all_with().empty());

            std::vector<Pattern> pattern = query_object.get_all_pattern();
            REQUIRE(pattern.size() == 1);
            REQUIRE(pattern.front().get_type() == PatternType::ASSIGN);
            PatternAssign pattern_obj = pattern.front().get_pattern_assign();
            std::string assigned_synonym = pattern_obj.get_assign_synonym();
            REQUIRE(assigned_synonym == "pattern");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            ExpressionSpec expression_spec = pattern_obj.get_expression_spec();
            REQUIRE(entity_ref.get_type() == EntityRefType::SYNONYM);
            REQUIRE(entity_ref.get_synonym() == "Pattern");
            REQUIRE(expression_spec.get_type() == ExpressionSpecType::EXACT_PATTERN);
            REQUIRE(expression_spec.get_exact_pattern()->is_equal(pattern_node));
        }

        SECTION("ExpressionSpecType of type ANY") {
            std::string query = "assign a; Select a pattern a(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();
            std::vector<Pattern> pattern = query_object.get_all_pattern();
            REQUIRE(pattern.size() == 1);
            REQUIRE(pattern.front().get_type() == PatternType::ASSIGN);
            PatternAssign pattern_obj = pattern.front().get_pattern_assign();
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

        Result result = query_object.get_result();
        REQUIRE(result.get_type() == ResultType::TUPLE);
        std::vector<Elem> tmp_tuple = result.get_tuple();
        REQUIRE(tmp_tuple.size() == 1);
        Elem elem = tmp_tuple.front();
        REQUIRE(elem.get_type() == ElemType::SYNONYM);
        REQUIRE(elem.get_synonym() == "w");

        REQUIRE(query_object.get_all_such_that().size() == 1);
        REQUIRE(query_object.get_all_pattern().size() == 1);
        REQUIRE(query_object.get_all_with().empty());

        std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
        SuchThat such_that_obj = such_that_vector.front();
        REQUIRE(such_that_obj.get_type() == SuchThatType::USES_S);

        SuchThatRef left_ref = such_that_obj.get_left_ref();
        SuchThatRef right_ref = such_that_obj.get_right_ref();
        REQUIRE(left_ref.get_type() == SuchThatRefType::STATEMENT);
        REQUIRE(right_ref.get_type() == SuchThatRefType::ENTITY);

        StatementRef left_statement_ref = left_ref.get_statement_ref();
        EntityRef right_entity_ref = right_ref.get_entity_ref();
        REQUIRE(left_statement_ref.get_type() == StatementRefType::SYNONYM);
        REQUIRE(left_statement_ref.get_synonym() == "a");
        REQUIRE(right_entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(right_entity_ref.get_synonym() == "v");

        std::vector<Pattern> pattern = query_object.get_all_pattern();
        REQUIRE(pattern.size() == 1);
        REQUIRE(pattern.front().get_type() == PatternType::ASSIGN);
        PatternAssign pattern_obj = pattern.front().get_pattern_assign();
        std::string assigned_synonym = pattern_obj.get_assign_synonym();
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