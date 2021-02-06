#include "catch.hpp"

#include "Parser/shared/StringLexer.h"
#include "Parser/shared/Token.h"
#include <string>
#include <memory>

using namespace Parser;

TEST_CASE("Parser::StringLexer") {
    std::shared_ptr<TokenList> tokens;

    SECTION("words") {
        std::string source = "alphabetical alphanumeric123 12345";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "alphabetical");

        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::WORD);
        REQUIRE(second->get_value() == "alphanumeric123");

        auto third = tokens->pop_front();
        REQUIRE(third->get_type() == TokenType::WORD);
        REQUIRE(third->get_value() == "12345");
    }

    SECTION("brackets") {
        std::string source = "(){}";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::LPAREN);
        REQUIRE(first->get_value() == "(");

        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::RPAREN);
        REQUIRE(second->get_value() == ")");

        auto third = tokens->pop_front();
        REQUIRE(third->get_type() == TokenType::LCURL);
        REQUIRE(third->get_value() == "{");

        auto fourth = tokens->pop_front();
        REQUIRE(fourth->get_type() == TokenType::RCURL);
        REQUIRE(fourth->get_value() == "}");
    }

    SECTION("comparison operators") {
        std::string source = "> >= < <= != ==";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::GT);
        REQUIRE(first->get_value() == ">");

        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::GTE);
        REQUIRE(second->get_value() == ">=");

        auto third = tokens->pop_front();
        REQUIRE(third->get_type() == TokenType::LT);
        REQUIRE(third->get_value() == "<");

        auto fourth = tokens->pop_front();
        REQUIRE(fourth->get_type() == TokenType::LTE);
        REQUIRE(fourth->get_value() == "<=");

        auto fifth = tokens->pop_front();
        REQUIRE(fifth->get_type() == TokenType::NEQUAL);
        REQUIRE(fifth->get_value() == "!=");

        auto sixth = tokens->pop_front();
        REQUIRE(sixth->get_type() == TokenType::DEQUAL);
        REQUIRE(sixth->get_value() == "==");
    }

    SECTION("logical operators") {
        std::string source = "! && ||";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::NOT);
        REQUIRE(first->get_value() == "!");

        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::AND);
        REQUIRE(second->get_value() == "&&");

        auto third = tokens->pop_front();
        REQUIRE(third->get_type() == TokenType::OR);
        REQUIRE(third->get_value() == "||");
    }

    SECTION("arithmetic operators") {
        std::string source = "= + - * / %";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::EQUAL);
        REQUIRE(first->get_value() == "=");

        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::PLUS);
        REQUIRE(second->get_value() == "+");

        auto third = tokens->pop_front();
        REQUIRE(third->get_type() == TokenType::MINUS);
        REQUIRE(third->get_value() == "-");

        auto fourth = tokens->pop_front();
        REQUIRE(fourth->get_type() == TokenType::ASTERISK);
        REQUIRE(fourth->get_value() == "*");

        auto fifth = tokens->pop_front();
        REQUIRE(fifth->get_type() == TokenType::SLASH);
        REQUIRE(fifth->get_value() == "/");

        auto sixth = tokens->pop_front();
        REQUIRE(sixth->get_type() == TokenType::PERCENT);
        REQUIRE(sixth->get_value() == "%");
    }

    SECTION("semicolon") {
        std::string source = ";";
        StringLexer lexer(source);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::SEMICOLON);
        REQUIRE(first->get_value() == ";");
    }

    auto end = tokens->pop_front();
    REQUIRE(end->get_type() == TokenType::END);
}
