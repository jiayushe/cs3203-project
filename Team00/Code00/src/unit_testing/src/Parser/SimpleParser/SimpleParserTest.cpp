#include "catch.hpp"

#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/StringLexer.h"
#include <iterator>
#include <string>

using namespace Parser;

TEST_CASE("Parser::SimpleParser") {
    SECTION("read") {
        std::string source = "read varname123;";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto read_node = parser.parse_read();
        REQUIRE(read_node->get_type() == SimpleNodeType::READ);
        REQUIRE(read_node->get_statement_id() == 1);
        REQUIRE(read_node->get_children().size() == 1);

        auto var_name_node = read_node->get_child(0);
        REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
        REQUIRE(var_name_node->get_value() == "varname123");
        REQUIRE(var_name_node->get_children().size() == 0);

        delete read_node;
    }

    SECTION("print") {
        std::string source = "print var123name;";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto print_node = parser.parse_print();
        REQUIRE(print_node->get_type() == SimpleNodeType::PRINT);
        REQUIRE(print_node->get_statement_id() == 1);
        REQUIRE(print_node->get_children().size() == 1);

        auto var_name_node = print_node->get_child(0);
        REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
        REQUIRE(var_name_node->get_value() == "var123name");
        REQUIRE(var_name_node->get_children().size() == 0);

        delete print_node;
    }

    SECTION("call") {
        std::string source = "call Proc123Name456;";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto call_node = parser.parse_call();
        REQUIRE(call_node->get_type() == SimpleNodeType::CALL);
        REQUIRE(call_node->get_statement_id() == 1);
        REQUIRE(call_node->get_children().size() == 1);

        auto proc_name_node = call_node->get_child(0);
        REQUIRE(proc_name_node->get_type() == SimpleNodeType::PROC_NAME);
        REQUIRE(proc_name_node->get_value() == "Proc123Name456");
        REQUIRE(proc_name_node->get_children().size() == 0);

        delete call_node;
    }

    SECTION("assign") {
        SECTION("assign to variable") {
            std::string source = "var123name = anothervar123;";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto assign_node = parser.parse_assign();
            REQUIRE(assign_node->get_type() == SimpleNodeType::ASSIGN);
            REQUIRE(assign_node->get_statement_id() == 1);
            REQUIRE(assign_node->get_children().size() == 2);

            auto left_var_name_node = assign_node->get_child(0);
            REQUIRE(left_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_var_name_node->get_value() == "var123name");
            REQUIRE(left_var_name_node->get_children().size() == 0);

            auto right_var_name_node = assign_node->get_child(1);
            REQUIRE(right_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(right_var_name_node->get_value() == "anothervar123");
            REQUIRE(right_var_name_node->get_children().size() == 0);

            delete assign_node;
        }

        SECTION("assign to constant") {
            std::string source = "var123name = 123456;";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto assign_node = parser.parse_assign();
            REQUIRE(assign_node->get_type() == SimpleNodeType::ASSIGN);
            REQUIRE(assign_node->get_statement_id() == 1);
            REQUIRE(assign_node->get_children().size() == 2);

            auto var_name_node = assign_node->get_child(0);
            REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(var_name_node->get_value() == "var123name");
            REQUIRE(var_name_node->get_children().size() == 0);

            auto const_value_node = assign_node->get_child(1);
            REQUIRE(const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(const_value_node->get_value() == "123456");
            REQUIRE(const_value_node->get_children().size() == 0);

            delete assign_node;
        }

        SECTION("assign to arithmetic operation") {
            std::string source = "var123name = anothervar123 + 123456;";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto assign_node = parser.parse_assign();
            REQUIRE(assign_node->get_type() == SimpleNodeType::ASSIGN);
            REQUIRE(assign_node->get_statement_id() == 1);
            REQUIRE(assign_node->get_children().size() == 2);

            auto var_name_node = assign_node->get_child(0);
            REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(var_name_node->get_value() == "var123name");
            REQUIRE(var_name_node->get_children().size() == 0);

            auto arith_node = assign_node->get_child(1);
            REQUIRE(arith_node->get_type() == SimpleNodeType::ARITHMETIC);

            delete assign_node;
        }
    }

    SECTION("while") {
        std::string source;
        source += "while (x > 5) {";
        source += "  print varname123;";
        source += "}";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto while_node = parser.parse_while();
        REQUIRE(while_node->get_type() == SimpleNodeType::WHILE);
        REQUIRE(while_node->get_statement_id() == 1);
        REQUIRE(while_node->get_children().size() == 2);
        REQUIRE(while_node->get_child(0)->get_type() == SimpleNodeType::CONDITIONAL);
        REQUIRE(while_node->get_child(1)->get_type() == SimpleNodeType::STMT_LST);
        REQUIRE(while_node->get_child(1)->get_child(0)->get_type() == SimpleNodeType::PRINT);
        REQUIRE(while_node->get_child(1)->get_child(0)->get_statement_id() == 2);

        delete while_node;
    }

    SECTION("if") {
        std::string source;
        source += "if (y <= 5) then {";
        source += "  print varname123;";
        source += "} else {";
        source += "  read varname123;";
        source += "}";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto if_node = parser.parse_if();
        REQUIRE(if_node->get_type() == SimpleNodeType::IF);
        REQUIRE(if_node->get_statement_id() == 1);
        REQUIRE(if_node->get_children().size() == 3);

        auto cond_node = if_node->get_child(0);
        REQUIRE(cond_node->get_type() == SimpleNodeType::CONDITIONAL);

        // Make sure that the first child is a statement list for the 'then' clause
        auto then_stmt_lst_node = if_node->get_child(1);
        REQUIRE(then_stmt_lst_node->get_type() == SimpleNodeType::STMT_LST);
        REQUIRE(then_stmt_lst_node->get_child(0)->get_type() == SimpleNodeType::PRINT);
        REQUIRE(then_stmt_lst_node->get_child(0)->get_statement_id() == 2);

        // Make sure that the first child is a statement list for the 'else' clause
        auto else_stmt_lst_node = if_node->get_child(2);
        REQUIRE(else_stmt_lst_node->get_type() == SimpleNodeType::STMT_LST);
        REQUIRE(else_stmt_lst_node->get_child(0)->get_type() == SimpleNodeType::READ);
        REQUIRE(else_stmt_lst_node->get_child(0)->get_statement_id() == 3);

        delete if_node;
    }

    SECTION("arithmetic") {
        SECTION("simple arithmetic operations") {
            std::string op = GENERATE("+", "-", "*", "/", "%");

            std::string source = "x " + op + " 5";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto arith_node = parser.parse_expr();
            REQUIRE(arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(arith_node->get_value() == op);
            REQUIRE(arith_node->get_children().size() == 2);

            auto var_name_node = arith_node->get_child(0);
            REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(var_name_node->get_value() == "x");
            REQUIRE(var_name_node->get_children().size() == 0);

            auto const_value_node = arith_node->get_child(1);
            REQUIRE(const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(const_value_node->get_value() == "5");
            REQUIRE(const_value_node->get_children().size() == 0);

            delete arith_node;
        }

        SECTION("left-associative") {
            std::string source = "x + z + 5";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto arith_node = parser.parse_expr();
            REQUIRE(arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(arith_node->get_value() == "+");
            REQUIRE(arith_node->get_children().size() == 2);

            auto right_const_value_node = arith_node->get_child(1);
            REQUIRE(right_const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(right_const_value_node->get_value() == "5");
            REQUIRE(right_const_value_node->get_children().size() == 0);

            auto child_arith_node = arith_node->get_child(0);
            REQUIRE(child_arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(child_arith_node->get_value() == "+");
            REQUIRE(child_arith_node->get_children().size() == 2);

            auto left_var_name_node = child_arith_node->get_child(0);
            REQUIRE(left_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_var_name_node->get_value() == "x");
            REQUIRE(left_var_name_node->get_children().size() == 0);

            auto middle_var_name_node = child_arith_node->get_child(1);
            REQUIRE(middle_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(middle_var_name_node->get_value() == "z");
            REQUIRE(middle_var_name_node->get_children().size() == 0);

            delete arith_node;
        }

        SECTION("*, /, and % have higher precedence than + and -") {
            std::string source = "x + z * 5";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto arith_node = parser.parse_expr();
            REQUIRE(arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(arith_node->get_value() == "+");
            REQUIRE(arith_node->get_children().size() == 2);

            auto left_var_name_node = arith_node->get_child(0);
            REQUIRE(left_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_var_name_node->get_value() == "x");
            REQUIRE(left_var_name_node->get_children().size() == 0);

            auto child_arith_node = arith_node->get_child(1);
            REQUIRE(child_arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(child_arith_node->get_value() == "*");
            REQUIRE(child_arith_node->get_children().size() == 2);

            auto middle_var_name_node = child_arith_node->get_child(0);
            REQUIRE(middle_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(middle_var_name_node->get_value() == "z");
            REQUIRE(middle_var_name_node->get_children().size() == 0);

            auto right_const_value_node = child_arith_node->get_child(1);
            REQUIRE(right_const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(right_const_value_node->get_value() == "5");
            REQUIRE(right_const_value_node->get_children().size() == 0);

            delete arith_node;
        }

        SECTION("(...) has higher precedence than *, /, and %") {
            std::string source = "(x + z) * 5";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto arith_node = parser.parse_expr();
            REQUIRE(arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(arith_node->get_value() == "*");
            REQUIRE(arith_node->get_children().size() == 2);

            auto right_const_value_node = arith_node->get_child(1);
            REQUIRE(right_const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(right_const_value_node->get_value() == "5");
            REQUIRE(right_const_value_node->get_children().size() == 0);

            auto child_arith_node = arith_node->get_child(0);
            REQUIRE(child_arith_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(child_arith_node->get_value() == "+");
            REQUIRE(child_arith_node->get_children().size() == 2);

            auto left_var_name_node = child_arith_node->get_child(0);
            REQUIRE(left_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_var_name_node->get_value() == "x");
            REQUIRE(left_var_name_node->get_children().size() == 0);

            auto middle_var_name_node = child_arith_node->get_child(1);
            REQUIRE(middle_var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(middle_var_name_node->get_value() == "z");
            REQUIRE(middle_var_name_node->get_children().size() == 0);

            delete arith_node;
        }
    }

    SECTION("conditional") {
        SECTION("simple comparison operations") {
            std::string op = GENERATE(">", ">=", "<", "<=", "==", "!=");

            std::string source = "x " + op + " 5";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto cond_node = parser.parse_cond_expr();
            REQUIRE(cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(cond_node->get_value() == op);
            REQUIRE(cond_node->get_children().size() == 2);

            auto var_name_node = cond_node->get_child(0);
            REQUIRE(var_name_node->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(var_name_node->get_value() == "x");
            REQUIRE(var_name_node->get_children().size() == 0);

            auto const_value_node = cond_node->get_child(1);
            REQUIRE(const_value_node->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(const_value_node->get_value() == "5");
            REQUIRE(const_value_node->get_children().size() == 0);

            delete cond_node;
        }

        SECTION("negation") {
            std::string source = "!(x > 5)";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto cond_node = parser.parse_cond_expr();
            REQUIRE(cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(cond_node->get_value() == "!");
            REQUIRE(cond_node->get_children().size() == 1);

            auto inner_cond_node = cond_node->get_child(0);
            REQUIRE(inner_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(inner_cond_node->get_value() == ">");
            REQUIRE(inner_cond_node->get_children().size() == 2);
            REQUIRE(inner_cond_node->get_child(0)->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(inner_cond_node->get_child(1)->get_type() == SimpleNodeType::CONST_VALUE);

            delete cond_node;
        }

        SECTION("and/or") {
            std::string op = GENERATE("&&", "||");

            std::string source = "(x > 5) " + op + " (6 <= y)";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto cond_node = parser.parse_cond_expr();
            REQUIRE(cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(cond_node->get_value() == op);
            REQUIRE(cond_node->get_children().size() == 2);

            auto left_cond_node = cond_node->get_child(0);
            REQUIRE(left_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(left_cond_node->get_value() == ">");
            REQUIRE(left_cond_node->get_children().size() == 2);
            REQUIRE(left_cond_node->get_child(0)->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_cond_node->get_child(1)->get_type() == SimpleNodeType::CONST_VALUE);

            auto right_cond_node = cond_node->get_child(1);
            REQUIRE(right_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(right_cond_node->get_value() == "<=");
            REQUIRE(right_cond_node->get_children().size() == 2);
            REQUIRE(right_cond_node->get_child(0)->get_type() == SimpleNodeType::CONST_VALUE);
            REQUIRE(right_cond_node->get_child(1)->get_type() == SimpleNodeType::VAR_NAME);

            delete cond_node;
        }

        SECTION("deeply nested") {
            std::string source = "(!(x + 5 < (x * 7))) || (x > 0)";
            StringLexer lexer(source);
            SimpleParser parser(lexer);

            auto cond_node = parser.parse_cond_expr();
            REQUIRE(cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(cond_node->get_value() == "||");
            REQUIRE(cond_node->get_children().size() == 2);

            auto right_cond_node = cond_node->get_child(1);
            REQUIRE(right_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(right_cond_node->get_value() == ">");
            REQUIRE(right_cond_node->get_children().size() == 2);
            REQUIRE(right_cond_node->get_child(0)->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(right_cond_node->get_child(1)->get_type() == SimpleNodeType::CONST_VALUE);

            auto left_cond_node = cond_node->get_child(0);
            REQUIRE(left_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(left_cond_node->get_value() == "!");
            REQUIRE(left_cond_node->get_children().size() == 1);

            auto left_inner_cond_node = left_cond_node->get_child(0);
            REQUIRE(left_inner_cond_node->get_type() == SimpleNodeType::CONDITIONAL);
            REQUIRE(left_inner_cond_node->get_value() == "<");
            REQUIRE(left_inner_cond_node->get_children().size() == 2);

            auto left_inner_left_cond_node = left_inner_cond_node->get_child(0);
            REQUIRE(left_inner_left_cond_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(left_inner_left_cond_node->get_value() == "+");
            REQUIRE(left_inner_left_cond_node->get_children().size() == 2);
            REQUIRE(left_inner_left_cond_node->get_child(0)->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_inner_left_cond_node->get_child(1)->get_type() == SimpleNodeType::CONST_VALUE);

            auto left_inner_right_cond_node = left_inner_cond_node->get_child(1);
            REQUIRE(left_inner_right_cond_node->get_type() == SimpleNodeType::ARITHMETIC);
            REQUIRE(left_inner_right_cond_node->get_value() == "*");
            REQUIRE(left_inner_right_cond_node->get_children().size() == 2);
            REQUIRE(left_inner_right_cond_node->get_child(0)->get_type() == SimpleNodeType::VAR_NAME);
            REQUIRE(left_inner_right_cond_node->get_child(1)->get_type() == SimpleNodeType::CONST_VALUE);

            delete cond_node;
        }
    }

    SECTION("stmt_lst") {
        std::string source;
        source += "read w;";
        source += "print x;";
        source += "call y;";
        source += "while (x > 5) {";
        source += "  read a;";
        source += "}";
        source += "if (x > 5) then {";
        source += "  read b;";
        source += "} else {";
        source += "  read c;";
        source += "}";
        source += "z = 10 ;";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto stmt_lst_node = parser.parse_stmt_lst();
        REQUIRE(stmt_lst_node->get_type() == SimpleNodeType::STMT_LST);
        REQUIRE(stmt_lst_node->get_children().size() == 6);
        REQUIRE(stmt_lst_node->get_child(0)->get_type() == SimpleNodeType::READ);
        REQUIRE(stmt_lst_node->get_child(0)->get_statement_id() == 1);
        REQUIRE(stmt_lst_node->get_child(1)->get_type() == SimpleNodeType::PRINT);
        REQUIRE(stmt_lst_node->get_child(1)->get_statement_id() == 2);
        REQUIRE(stmt_lst_node->get_child(2)->get_type() == SimpleNodeType::CALL);
        REQUIRE(stmt_lst_node->get_child(2)->get_statement_id() == 3);
        REQUIRE(stmt_lst_node->get_child(3)->get_type() == SimpleNodeType::WHILE);
        REQUIRE(stmt_lst_node->get_child(3)->get_statement_id() == 4);
        REQUIRE(stmt_lst_node->get_child(4)->get_type() == SimpleNodeType::IF);
        REQUIRE(stmt_lst_node->get_child(4)->get_statement_id() == 6);
        REQUIRE(stmt_lst_node->get_child(5)->get_type() == SimpleNodeType::ASSIGN);
        REQUIRE(stmt_lst_node->get_child(5)->get_statement_id() == 9);

        delete stmt_lst_node;
    }

    SECTION("procedure") {
        std::string source;
        source += "procedure procname123 {";
        source += "  read x;";
        source += "}";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto procedure_node = parser.parse_procedure();
        REQUIRE(procedure_node->get_type() == SimpleNodeType::PROCEDURE);
        REQUIRE(procedure_node->get_children().size() == 2);

        auto proc_name_node = procedure_node->get_child(0);
        REQUIRE(proc_name_node->get_type() == SimpleNodeType::PROC_NAME);
        REQUIRE(proc_name_node->get_value() == "procname123");
        REQUIRE(proc_name_node->get_children().size() == 0);

        auto stmt_lst_node = procedure_node->get_child(1);
        REQUIRE(stmt_lst_node->get_type() == SimpleNodeType::STMT_LST);

        delete procedure_node;
    }

    SECTION("program") {
        std::string source;
        source += "procedure procname123 {";
        source += "  read x;";
        source += "}";
        StringLexer lexer(source);
        SimpleParser parser(lexer);

        auto program_node = parser.parse_program();
        REQUIRE(program_node->get_type() == SimpleNodeType::PROGRAM);
        REQUIRE(program_node->get_children().size() == 1);
        REQUIRE(program_node->get_child(0)->get_type() == SimpleNodeType::PROCEDURE);

        delete program_node;
    }
}
