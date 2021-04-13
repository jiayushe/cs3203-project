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
            {"NextBip", SuchThatType::NEXTBIP},
            {"NextBip*", SuchThatType::NEXTBIP_T},
            {"AffectsBip", SuchThatType::AFFECTSBIP},
            {"AffectsBip*", SuchThatType::AFFECTSBIP_T},
        };

        SECTION("Parent/Parent*/Follows/Follows*/Next/Next*/Affects/Affects* type") {
            std::string op = GENERATE("Parent", "Parent*", "Follows", "Follows*", "Next", "Next*",
                                      "Affects", "Affects*", "NextBip", "NextBip*", "AffectsBip", "AffectsBip*");

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

    SECTION("Multiple Such That clauses") {
        std::string op = GENERATE("such that Modifies(p, _) and Uses(p1, _)",
                                  "such that Modifies(p, _) such that Uses(p1, _)");

        std::string query = "procedure p, p1; Select BOOLEAN " + op;
        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        auto query_object = parser.parse_query();

        auto declarations = query_object.get_declarations();
        REQUIRE(declarations.size() == 2);
        REQUIRE(declarations.has("p"));
        REQUIRE(declarations.has("p1"));

        Result result = query_object.get_result();
        REQUIRE(result.get_type() == ResultType::BOOLEAN);

        REQUIRE(query_object.get_all_such_that().size() == 2);
        REQUIRE(query_object.get_all_pattern().empty());
        REQUIRE(query_object.get_all_with().empty());

        std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
        SuchThat such_that_0 = such_that_vector[0];
        SuchThat such_that_1 = such_that_vector[1];
        REQUIRE(such_that_0.get_type() == SuchThatType::MODIFIES_P);
        REQUIRE(such_that_1.get_type() == SuchThatType::USES_P);

        SuchThatRef left_ref_0 = such_that_0.get_left_ref();
        SuchThatRef right_ref_0 = such_that_0.get_right_ref();
        REQUIRE(left_ref_0.get_type() == SuchThatRefType::ENTITY);
        REQUIRE(right_ref_0.get_type() == SuchThatRefType::ENTITY);

        EntityRef left_entity_ref = left_ref_0.get_entity_ref();
        EntityRef right_entity_ref = right_ref_0.get_entity_ref();
        REQUIRE(left_entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(left_entity_ref.get_synonym() == "p");
        REQUIRE(right_entity_ref.get_type() == EntityRefType::ANY);

        SuchThatRef left_ref_1 = such_that_1.get_left_ref();
        SuchThatRef right_ref_1 = such_that_1.get_right_ref();
        REQUIRE(left_ref_1.get_type() == SuchThatRefType::ENTITY);
        REQUIRE(right_ref_1.get_type() == SuchThatRefType::ENTITY);

        left_entity_ref = left_ref_1.get_entity_ref();
        right_entity_ref = right_ref_1.get_entity_ref();
        REQUIRE(left_entity_ref.get_type() == EntityRefType::SYNONYM);
        REQUIRE(left_entity_ref.get_synonym() == "p1");
        REQUIRE(right_entity_ref.get_type() == EntityRefType::ANY);
    }

    SECTION("One Pattern clause") {
        SECTION("General PatternAssign test") {
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

        SECTION("General PatternIf test") {
            std::string query = "if ifs; Select ifs pattern ifs(\"ifs\", _, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("ifs"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "ifs");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().size() == 1);
            REQUIRE(query_object.get_all_with().empty());

            std::vector<Pattern> pattern = query_object.get_all_pattern();
            REQUIRE(pattern.size() == 1);
            REQUIRE(pattern.front().get_type() == PatternType::IF);
            PatternIf pattern_obj = pattern.front().get_pattern_if();
            std::string assigned_synonym = pattern_obj.get_if_synonym();
            REQUIRE(assigned_synonym == "ifs");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            REQUIRE(entity_ref.get_type() == EntityRefType::NAME);
            REQUIRE(entity_ref.get_name() == "ifs");
        }

        SECTION("General PatternWhile test") {
            std::string query = "while while; Select while pattern while(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            auto query_object = parser.parse_query();

            auto declarations = query_object.get_declarations();
            REQUIRE(declarations.size() == 1);
            REQUIRE(declarations.has("while"));

            Result result = query_object.get_result();
            REQUIRE(result.get_type() == ResultType::TUPLE);
            std::vector<Elem> tmp_tuple = result.get_tuple();
            REQUIRE(tmp_tuple.size() == 1);
            Elem elem = tmp_tuple.front();
            REQUIRE(elem.get_type() == ElemType::SYNONYM);
            REQUIRE(elem.get_synonym() == "while");

            REQUIRE(query_object.get_all_such_that().empty());
            REQUIRE(query_object.get_all_pattern().size() == 1);
            REQUIRE(query_object.get_all_with().empty());

            std::vector<Pattern> pattern = query_object.get_all_pattern();
            REQUIRE(pattern.size() == 1);
            REQUIRE(pattern.front().get_type() == PatternType::WHILE);
            PatternWhile pattern_obj = pattern.front().get_pattern_while();
            std::string assigned_synonym = pattern_obj.get_while_synonym();
            REQUIRE(assigned_synonym == "while");

            EntityRef entity_ref = pattern_obj.get_entity_ref();
            REQUIRE(entity_ref.get_type() == EntityRefType::ANY);
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

    SECTION("Multiple Pattern clauses") {
        std::string op = GENERATE("pattern a(_, _) and ifs(_, _, _) and w(_, _)",
                                  "pattern a(_, _) pattern ifs(_, _, _) pattern w(_, _)");
        std::string query = "assign a; if ifs; while w; Select BOOLEAN " + op;

        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        auto query_object = parser.parse_query();

        auto declarations = query_object.get_declarations();
        REQUIRE(declarations.size() == 3);
        REQUIRE(declarations.has("a"));
        REQUIRE(declarations.has("ifs"));
        REQUIRE(declarations.has("w"));

        Result result = query_object.get_result();
        REQUIRE(result.get_type() == ResultType::BOOLEAN);

        REQUIRE(query_object.get_all_such_that().empty());
        REQUIRE(query_object.get_all_pattern().size() == 3);
        REQUIRE(query_object.get_all_with().empty());

        std::vector<Pattern> pattern = query_object.get_all_pattern();
        REQUIRE(pattern[0].get_type() == PatternType::ASSIGN);
        PatternAssign pattern_assign = pattern[0].get_pattern_assign();
        REQUIRE(pattern[1].get_type() == PatternType::IF);
        PatternIf pattern_if = pattern[1].get_pattern_if();
        REQUIRE(pattern[2].get_type() == PatternType::WHILE);
        PatternWhile pattern_while = pattern[2].get_pattern_while();

        REQUIRE(pattern_assign.get_assign_synonym() == "a");
        REQUIRE(pattern_assign.get_entity_ref().get_type() == EntityRefType::ANY);
        REQUIRE(pattern_assign.get_expression_spec().get_type() == ExpressionSpecType::ANY);

        REQUIRE(pattern_if.get_if_synonym() == "ifs");
        REQUIRE(pattern_if.get_entity_ref().get_type() == EntityRefType::ANY);

        REQUIRE(pattern_while.get_while_synonym() == "w");
        REQUIRE(pattern_while.get_entity_ref().get_type() == EntityRefType::ANY);
    }

    SECTION("One With clause") {
        SECTION("WithRef NAME test") {
            std::string op1 = GENERATE("\"name1\"", "10", "p.procName", "s.stmt#", "pg");
            std::string op = GENERATE("\"NAME2\"", "v.varName");
            std::string query =
                "prog_line pg; procedure p; variable v; stmt s; Select BOOLEAN with " + op + " = " +
                op1;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);

            if (op1 == "\"name1\"" || op1 == "p.procName") {
                auto query_object = parser.parse_query();
                auto declarations = query_object.get_declarations();
                REQUIRE(declarations.size() == 4);
                REQUIRE(declarations.has("p"));
                REQUIRE(declarations.has("pg"));
                REQUIRE(declarations.has("s"));
                REQUIRE(declarations.has("v"));

                Result result = query_object.get_result();
                REQUIRE(result.get_type() == ResultType::BOOLEAN);

                REQUIRE(query_object.get_all_such_that().empty());
                REQUIRE(query_object.get_all_pattern().empty());
                REQUIRE(query_object.get_all_with().size() == 1);

                std::vector<With> with = query_object.get_all_with();
                REQUIRE(with.size() == 1);
                WithRef left_with_ref = with.front().get_left_ref();
                WithRef right_with_ref = with.front().get_right_ref();

                if (op == "NAME2") {
                    REQUIRE(left_with_ref.get_type() == WithRefType::NAME);
                    REQUIRE(left_with_ref.get_name() == "NAME2");
                } else if (op == "p.procName") {
                    REQUIRE(left_with_ref.get_type() == WithRefType::ATTR_REF);
                    REQUIRE(left_with_ref.get_attr_ref().get_synonym() == "v");
                    REQUIRE(left_with_ref.get_attr_ref().get_attr_name() == "varName");
                }

                if (op1 == "name1") {
                    REQUIRE(right_with_ref.get_type() == WithRefType::NAME);
                    REQUIRE(right_with_ref.get_name() == "name1");
                } else if (op1 == "p.procName") {
                    REQUIRE(right_with_ref.get_type() == WithRefType::ATTR_REF);
                    REQUIRE(right_with_ref.get_attr_ref().get_synonym() == "p");
                    REQUIRE(right_with_ref.get_attr_ref().get_attr_name() == "procName");
                }
            } else {
                REQUIRE_THROWS(parser.parse_query());
            }
        }

        SECTION("WithRef INTEGER test") {
            std::string op1 = GENERATE("\"name1\"", "10", "p.procName", "s.stmt#", "pg");
            std::string op = GENERATE("20", "s1.stmt#", "pg1");
            std::string query =
                "prog_line pg, pg1; procedure p; stmt s, s1; Select BOOLEAN with " + op + "=" + op1;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);

            if (op1 == "10" || op1 == "s.stmt#" || op1 == "pg") {
                auto query_object = parser.parse_query();
                auto declarations = query_object.get_declarations();
                REQUIRE(declarations.size() == 5);

                REQUIRE(query_object.get_all_such_that().empty());
                REQUIRE(query_object.get_all_pattern().empty());
                REQUIRE(query_object.get_all_with().size() == 1);

                std::vector<With> with = query_object.get_all_with();
                REQUIRE(with.size() == 1);
                WithRef left_with_ref = with.front().get_left_ref();
                WithRef right_with_ref = with.front().get_right_ref();

                if (op == "20") {
                    REQUIRE(left_with_ref.get_type() == WithRefType::STATEMENT_ID);
                    REQUIRE(left_with_ref.get_statement_id() == 20);
                } else if (op == "s1.stmt#") {
                    REQUIRE(left_with_ref.get_type() == WithRefType::ATTR_REF);
                    REQUIRE(left_with_ref.get_attr_ref().get_synonym() == "s1");
                    REQUIRE(left_with_ref.get_attr_ref().get_attr_name() == "stmt#");
                } else if (op == "pg1") {
                    REQUIRE(left_with_ref.get_type() == WithRefType::SYNONYM);
                    REQUIRE(left_with_ref.get_synonym() == "pg1");
                }

                if (op1 == "10") {
                    REQUIRE(right_with_ref.get_type() == WithRefType::STATEMENT_ID);
                    REQUIRE(right_with_ref.get_statement_id() == 10);
                } else if (op1 == "s.stmt#") {
                    REQUIRE(right_with_ref.get_type() == WithRefType::ATTR_REF);
                    REQUIRE(right_with_ref.get_attr_ref().get_synonym() == "s");
                    REQUIRE(right_with_ref.get_attr_ref().get_attr_name() == "stmt#");
                } else if (op1 == "pg") {
                    REQUIRE(right_with_ref.get_type() == WithRefType::SYNONYM);
                    REQUIRE(right_with_ref.get_synonym() == "pg");
                }
            } else {
                REQUIRE_THROWS(parser.parse_query());
            }
        }
    }

    SECTION("Multiple With clauses") {
        std::string op = GENERATE("with 10 =20 and 30= 40", "with 10 =20 with 30= 40");
        std::string query = "Select BOOLEAN " + op;

        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        auto query_object = parser.parse_query();

        auto declarations = query_object.get_declarations();
        REQUIRE(declarations.size() == 0);

        Result result = query_object.get_result();
        REQUIRE(result.get_type() == ResultType::BOOLEAN);

        REQUIRE(query_object.get_all_such_that().empty());
        REQUIRE(query_object.get_all_pattern().empty());
        REQUIRE(query_object.get_all_with().size() == 2);

        std::vector<With> with = query_object.get_all_with();
        REQUIRE(with[0].get_left_ref().get_type() == WithRefType::STATEMENT_ID);
        REQUIRE(with[0].get_left_ref().get_statement_id() == 10);
        REQUIRE(with[0].get_right_ref().get_type() == WithRefType::STATEMENT_ID);
        REQUIRE(with[0].get_right_ref().get_statement_id() == 20);
        REQUIRE(with[1].get_left_ref().get_type() == WithRefType::STATEMENT_ID);
        REQUIRE(with[1].get_left_ref().get_statement_id() == 30);
        REQUIRE(with[1].get_right_ref().get_type() == WithRefType::STATEMENT_ID);
        REQUIRE(with[1].get_right_ref().get_statement_id() == 40);
    }

    SECTION("Multiple Such That clause, Multiple Pattern clause, Multiple With clause") {
        std::string query = "variable v; assign a, a1, a2; while w; procedure p, p1; Select w ";
        query += "such that Uses(a, v) pattern a(v, _\"z\"_) ";
        query += "such that Calls*(p, p1) and Modifies(p, _) ";
        query += "with 10=10 and 20=20 ";
        query += "pattern a1(v, _) and a2(v, _) with 30=30";
        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        auto query_object = parser.parse_query();

        auto factor_source = std::make_shared<Parser::Source>("z");
        auto pattern_node = SimpleParser(factor_source).parse_expr();

        auto declarations = query_object.get_declarations();
        REQUIRE(declarations.size() == 7);
        REQUIRE(declarations.has("v"));
        REQUIRE(declarations.has("a"));
        REQUIRE(declarations.has("a1"));
        REQUIRE(declarations.has("a2"));
        REQUIRE(declarations.has("w"));
        REQUIRE(declarations.has("p"));
        REQUIRE(declarations.has("p1"));

        Result result = query_object.get_result();
        REQUIRE(result.get_type() == ResultType::TUPLE);
        std::vector<Elem> tmp_tuple = result.get_tuple();
        REQUIRE(tmp_tuple.size() == 1);
        Elem elem = tmp_tuple.front();
        REQUIRE(elem.get_type() == ElemType::SYNONYM);
        REQUIRE(elem.get_synonym() == "w");

        REQUIRE(query_object.get_all_such_that().size() == 3);
        REQUIRE(query_object.get_all_pattern().size() == 3);
        REQUIRE(query_object.get_all_with().size() == 3);

        std::vector<SuchThat> such_that_vector = query_object.get_all_such_that();
        REQUIRE(such_that_vector[0].get_type() == SuchThatType::USES_S);
        REQUIRE(such_that_vector[1].get_type() == SuchThatType::CALLS_T);
        REQUIRE(such_that_vector[2].get_type() == SuchThatType::MODIFIES_P);

        std::vector<Pattern> pattern_vector = query_object.get_all_pattern();
        REQUIRE(pattern_vector[0].get_type() == PatternType::ASSIGN);
        REQUIRE(
            pattern_vector[0].get_pattern_assign().get_expression_spec().get_pattern()->is_equal(
                pattern_node));
        REQUIRE(pattern_vector[1].get_type() == PatternType::ASSIGN);
        REQUIRE(pattern_vector[2].get_type() == PatternType::ASSIGN);

        std::vector<With> with_vector = query_object.get_all_with();
        REQUIRE(with_vector[0].get_left_ref().get_statement_id() == 10);
        REQUIRE(with_vector[0].get_right_ref().get_statement_id() == 10);
        REQUIRE(with_vector[1].get_left_ref().get_statement_id() == 20);
        REQUIRE(with_vector[1].get_right_ref().get_statement_id() == 20);
        REQUIRE(with_vector[2].get_left_ref().get_statement_id() == 30);
        REQUIRE(with_vector[2].get_right_ref().get_statement_id() == 30);
    }

    SECTION("Syntactic validation") {
        SECTION("Validate such that clause") {
            std::string query = "stmt s; variable v; Select s such  that Modifies(s, v)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Validate prog_line clause") {
            std::string query = "prog_line prog; prog line; Select prog";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Validate stmt# clause") {
            std::string query = "prog_line prog; stmt stmt; Select prog with stmt.stmt #=prog";
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

            std::string query_3 = "assign a; variable v; Select a pattern a(v, s1)";
            auto source_3 = std::make_shared<Parser::Source>(query_3);
            Parser::PQLParser parser_3(source_3);
            REQUIRE_THROWS(parser_3.parse_query());

            std::string query_4 = "assign a; Select a pattern a(s1, _)";
            auto source_4 = std::make_shared<Parser::Source>(query_4);
            Parser::PQLParser parser_4(source_4);
            REQUIRE_THROWS(parser_4.parse_query());

            std::string query_5 = "assign a; variable v; Select a pattern a1(v, _)";
            auto source_5 = std::make_shared<Parser::Source>(query_5);
            Parser::PQLParser parser_5(source_5);
            REQUIRE_THROWS(parser_5.parse_query());

            std::string query_6 = "while w; variable v; Select w pattern w1(v, _)";
            auto source_6 = std::make_shared<Parser::Source>(query_6);
            Parser::PQLParser parser_6(source_6);
            REQUIRE_THROWS(parser_6.parse_query());

            std::string query_7 = "if ifs; variable v; Select ifs pattern if(v, _, _)";
            auto source_7 = std::make_shared<Parser::Source>(query_7);
            Parser::PQLParser parser_7(source_7);
            REQUIRE_THROWS(parser_7.parse_query());

            std::string query_8 = "while w; variable v; Select w pattern w1(v, _)";
            auto source_8 = std::make_shared<Parser::Source>(query_8);
            Parser::PQLParser parser_8(source_8);
            REQUIRE_THROWS(parser_8.parse_query());

            std::string query_9 = "if ifs; variable v; Select ifs pattern ifs(v1, _, _)";
            auto source_9 = std::make_shared<Parser::Source>(query_9);
            Parser::PQLParser parser_9(source_9);
            REQUIRE_THROWS(parser_9.parse_query());

            std::string query_10 = "constant c; Select c with c1.value = 10";
            auto source_10 = std::make_shared<Parser::Source>(query_10);
            Parser::PQLParser parser_10(source_10);
            REQUIRE_THROWS(parser_10.parse_query());

            std::string query_11 = "variable v; Select v with \"var\" = v1.varName";
            auto source_11 = std::make_shared<Parser::Source>(query_11);
            Parser::PQLParser parser_11(source_11);
            REQUIRE_THROWS(parser_11.parse_query());
        }

        SECTION("Pattern synonym must be assign/if/while") {
            std::string op = GENERATE("s", "pg", "const", "c", "r", "p", "v1");
            std::string query =
                "stmt s; prog_line pg; constant const; call c; read r; print p; variable v, v1; ";
            query += "Select s pattern " + op + "(v, _)";
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

        SECTION("PatternWhile LHS must be variable") {
            std::string query = "while w; stmt s; Select w pattern w(s, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("PatternIf LHS must be variable") {
            std::string query = "if ifs; stmt s; Select ifs pattern ifs(s, _, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Invalid with attrRef procName pairs") {
            std::string op = GENERATE("varName", "value", "stmt#", "stmt");
            std::string op1 = GENERATE("p", "c");
            std::string query = "procedure p; call c; Select p with " + op1 + "." + op;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Invalid with attrRef procName pairs") {
            std::string op = GENERATE("procName", "value", "stmt#", "stmt");
            std::string op1 = GENERATE("v", "r", "p");
            std::string query = "variable v; read r; print p; Select p with " + op1 + "." + op;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Invalid with attrRef value pairs") {
            std::string op = GENERATE("varName", "procName", "stmt#", "stmt");
            std::string op1 = GENERATE("const");
            std::string query = "constant const; Select BOOLEAN with " + op1 + "." + op;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Invalid with attrRef value pairs") {
            std::string op = GENERATE("varName", "procName", "value", "stmt");
            std::string op1 = GENERATE("s", "r", "p", "c", "w", "ifs", "a");
            std::string query = "stmt s; read r; print p; call c; while w; if ifs; assign a;";
            query += "Select BOOLEAN with " + op1 + "." + op;
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Calls and CallsT LHS and RHS (if synonym) need to be procedure") {
            std::string op = GENERATE("Calls", "Calls*");
            std::string query = "procedure p, p1; Select p such that " + op + "(p, p1)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_NOTHROW(parser.parse_query());
        }

        SECTION("Modifies, Uses LHS must not be ANY") {
            std::string op = GENERATE("Uses", "Modifies");
            std::string query = "stmt s; Select s such that " + op + "(_, _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Modifies, Uses RHS must be variable") {
            SECTION("Positive test cases") {
                std::string op = GENERATE("Uses", "Modifies");
                std::string query = "stmt s; variable v; Select s such that " + op + "(s, v)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_NOTHROW(parser.parse_query());
            }

            SECTION("Negative test cases") {
                std::string op = GENERATE("Uses", "Modifies");
                std::string query = "stmt s; Select s such that " + op + "(s, s)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_THROWS(parser.parse_query());
            }
        }

        SECTION("Modifies LHS can accept read and procedure but not print") {
            std::string op = GENERATE("r", "proc", "p");
            std::string query =
                "read r; procedure proc; print p; Select r such that Modifies(" + op + ", _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);

            if (op == "p") {
                REQUIRE_THROWS(parser.parse_query());
            } else {
                REQUIRE_NOTHROW(parser.parse_query());
            }
        }

        SECTION("Uses LHS can accept print and procedure but not read") {
            std::string op = GENERATE("r", "p", "proc");
            std::string query =
                "read r; print p; procedure proc; Select p such that Uses(" + op + ", _)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);

            if (op == "r") {
                REQUIRE_THROWS(parser.parse_query());
            } else {
                REQUIRE_NOTHROW(parser.parse_query());
            }
        }

        SECTION("Modifies, Uses, Follows, Follows*, Next, Next* LHS must be statementRef") {
            SECTION("Positive test cases") {
                std::string op =
                    GENERATE("Uses", "Modifies", "Follows", "Follows*", "Next", "Next*");
                std::string op1 = GENERATE("s", "ifs", "w", "pg", "c", "a");
                std::string query = "stmt s; if ifs; while w; prog_line pg; call c; assign a; ";
                query += "Select w such that " + op + "(" + op1 + ", _)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_NOTHROW(parser.parse_query());
            }

            SECTION("Negative test cases") {
                std::string op =
                    GENERATE("Uses", "Modifies", "Follows", "Follows*", "Next", "Next*");
                std::string op1 = GENERATE("const", "v");
                std::string query = "constant const; variable v; ";
                query += "Select v such that " + op + "(" + op1 + ", _)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_THROWS(parser.parse_query());
            }
        }

        SECTION("Parent/Parent* LHS only accepts stmt, prog_line, if, while") {
            SECTION("Positive test cases") {
                std::string op = GENERATE("Parent", "Parent*");
                std::string op1 = GENERATE("s", "pg", "ifs", "w");
                std::string query = "stmt s; prog_line pg; if ifs; while w; ";
                query += "Select w such that " + op + "(" + op1 + ", _)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_NOTHROW(parser.parse_query());
            }

            SECTION("Negative test cases") {
                std::string op = GENERATE("Parent", "Parent*");
                std::string op1 = GENERATE("proc", "const", "c", "r", "p", "v", "a");
                std::string query = "procedure proc; constant const; call c; read r; print p; "
                                    "variable v; assign a; ";
                query += "Select v such that " + op + "(" + op1 + ", _)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_THROWS(parser.parse_query());
            }
        }

        SECTION("Affects/Affects* LHS and RHS only accepts stmt, prog_line, assign") {
            SECTION("Positive test cases") {
                std::string op = GENERATE("Affects", "Affects*");
                std::string op1 = GENERATE("s", "pg", "a");
                std::string op2 = GENERATE("s", "pg", "a");
                std::string query = "stmt s; prog_line pg; assign a; ";
                query += "Select a such that " + op + "(" + op1 + ", " + op2 + ")";

                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_NOTHROW(parser.parse_query());
            }

            SECTION("Negative test cases") {
                std::string op = GENERATE("Affects", "Affects*");
                std::string op1 = GENERATE("const", "c", "r", "p", "v", "ifs", "w", "proc");
                std::string query = "constant const; call c; read r; print p; variable v; if ifs; "
                                    "while w; procedure proc ";
                query += "Select v such that " + op + "(" + op1 + ", _)";

                std::string query_1 = "constant const; call c; read r; print p; variable v; if "
                                      "ifs; while w; procedure proc ";
                query_1 += "Select v such that " + op + "(_, " + op1 + ")";

                auto source = std::make_shared<Parser::Source>(query);
                auto source_1 = std::make_shared<Parser::Source>(query_1);
                Parser::PQLParser parser(source);
                Parser::PQLParser parser_1(source_1);
                REQUIRE_THROWS(parser.parse_query());
                REQUIRE_THROWS(parser_1.parse_query());
            }
        }

        SECTION("Modifies, Uses RHS must be variable") {
            std::string op = GENERATE("Uses", "Modifies");
            std::string query = "stmt s; Select s such that " + op + "(s, s)";
            auto source = std::make_shared<Parser::Source>(query);
            Parser::PQLParser parser(source);
            REQUIRE_THROWS(parser.parse_query());
        }

        SECTION("Follows, Follows*, Parent, Parent*, Next, Next* RHS must be statementRef") {
            SECTION("Positive test cases") {
                std::string op =
                    GENERATE("Follows", "Follows*", "Parent", "Parent*", "Next", "Next*");
                std::string op1 = GENERATE("s", "r", "p", "ifs", "w", "pg", "c", "a");
                std::string query =
                    "stmt s; read r; print p; if ifs; while w; prog_line pg; call c; assign a; ";
                query += "Select w such that " + op + "(_, " + op1 + ")";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_NOTHROW(parser.parse_query());
            }

            SECTION("Negative test cases") {
                std::string op =
                    GENERATE("Follows", "Follows*", "Parent", "Parent*", "Next", "Next*");
                std::string op1 = GENERATE("const", "v");
                std::string query = "constant const; variable v; ";
                query += "Select v such that " + op + "(" + op1 + ", _)";
                auto source = std::make_shared<Parser::Source>(query);
                Parser::PQLParser parser(source);
                REQUIRE_THROWS(parser.parse_query());
            }
        }
    }
}