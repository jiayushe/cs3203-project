#include "catch.hpp"

#include "Parser/SimpleParser/SimpleLexer.h"
#include <memory>
#include <string>

using namespace Parser;

TEST_CASE("Parser::SimpleLexer") {
    SECTION("words") {
        std::string source = "alphabetical alphanumeric123 12345";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "alphabetical");

        auto second = lexer.pop_token();
        REQUIRE(second->get_type() == TokenType::WORD);
        REQUIRE(second->get_value() == "alphanumeric123");

        auto third = lexer.pop_token();
        REQUIRE(third->get_type() == TokenType::WORD);
        REQUIRE(third->get_value() == "12345");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("brackets") {
        std::string source = "(){}";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::LPAREN);
        REQUIRE(first->get_value() == "(");

        auto second = lexer.pop_token();
        REQUIRE(second->get_type() == TokenType::RPAREN);
        REQUIRE(second->get_value() == ")");

        auto third = lexer.pop_token();
        REQUIRE(third->get_type() == TokenType::LCURL);
        REQUIRE(third->get_value() == "{");

        auto fourth = lexer.pop_token();
        REQUIRE(fourth->get_type() == TokenType::RCURL);
        REQUIRE(fourth->get_value() == "}");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("comparison operators") {
        std::string source = "> >= < <= != ==";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::GT);
        REQUIRE(first->get_value() == ">");

        auto second = lexer.pop_token();
        REQUIRE(second->get_type() == TokenType::GTE);
        REQUIRE(second->get_value() == ">=");

        auto third = lexer.pop_token();
        REQUIRE(third->get_type() == TokenType::LT);
        REQUIRE(third->get_value() == "<");

        auto fourth = lexer.pop_token();
        REQUIRE(fourth->get_type() == TokenType::LTE);
        REQUIRE(fourth->get_value() == "<=");

        auto fifth = lexer.pop_token();
        REQUIRE(fifth->get_type() == TokenType::NEQUAL);
        REQUIRE(fifth->get_value() == "!=");

        auto sixth = lexer.pop_token();
        REQUIRE(sixth->get_type() == TokenType::DEQUAL);
        REQUIRE(sixth->get_value() == "==");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("logical operators") {
        std::string source = "! && ||";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::NOT);
        REQUIRE(first->get_value() == "!");

        auto second = lexer.pop_token();
        REQUIRE(second->get_type() == TokenType::AND);
        REQUIRE(second->get_value() == "&&");

        auto third = lexer.pop_token();
        REQUIRE(third->get_type() == TokenType::OR);
        REQUIRE(third->get_value() == "||");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("arithmetic operators") {
        std::string source = "= + - * / %";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::EQUAL);
        REQUIRE(first->get_value() == "=");

        auto second = lexer.pop_token();
        REQUIRE(second->get_type() == TokenType::PLUS);
        REQUIRE(second->get_value() == "+");

        auto third = lexer.pop_token();
        REQUIRE(third->get_type() == TokenType::MINUS);
        REQUIRE(third->get_value() == "-");

        auto fourth = lexer.pop_token();
        REQUIRE(fourth->get_type() == TokenType::ASTERISK);
        REQUIRE(fourth->get_value() == "*");

        auto fifth = lexer.pop_token();
        REQUIRE(fifth->get_type() == TokenType::SLASH);
        REQUIRE(fifth->get_value() == "/");

        auto sixth = lexer.pop_token();
        REQUIRE(sixth->get_type() == TokenType::PERCENT);
        REQUIRE(sixth->get_value() == "%");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("semicolon") {
        std::string source = ";";
        auto base_source = std::make_shared<Parser::Source>(source);
        SimpleLexer lexer(base_source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::SEMICOLON);
        REQUIRE(first->get_value() == ";");

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }
}
