#include "catch.hpp"

#include "Parser/PQLParser/PQLLexer.h"
#include <memory>
#include <string>

using namespace Parser;

TEST_CASE("Parser::PQLLexer") {
    SECTION("Parent*/Follows*") {
        std::string op = GENERATE("Follows*", "Parent*");
        auto source = std::make_shared<Parser::Source>(op);
        PQLLexer lexer(source);
        
        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == op);

        auto end = lexer.pop_token();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("Parent */Follows *") {
        std::string op = GENERATE("Follows", "Parent");
        std::string query = op + " *";
        auto source = std::make_shared<Parser::Source>(query);
        PQLLexer lexer(source);

        auto first = lexer.pop_token();
        REQUIRE(first->get_value() == op);
        REQUIRE_THROWS(lexer.pop_token());
    }

    SECTION("such that tokenization") {
        std::string query = "such that";
        std::string query_1 = "such    that";
        std::string query_2 = "  suchthat  ";
        auto source = std::make_shared<Parser::Source>(query);
        PQLLexer lexer(source);
        auto source_1 = std::make_shared<Parser::Source>(query_1);
        PQLLexer lexer_1(source_1);
        auto source_2 = std::make_shared<Parser::Source>(query_2);
        PQLLexer lexer_2(source_2);

        auto first = lexer.pop_token();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "such that");

        first = lexer_1.pop_token();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "such");
        auto second = lexer_1.pop_token();
        REQUIRE(second->get_type() == TokenType::WORD);
        REQUIRE(second->get_value() == "that");

        first = lexer_2.pop_token();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "suchthat");
    }
}
