#include "catch.hpp"

#include "Parser/PQLParser/PQLStringLexer.h"
#include "Parser/shared/TokenList.h"
#include <memory>
#include <string>

using namespace Parser;

TEST_CASE("Parser::PQLStringLexer") {
    std::shared_ptr<TokenList> tokens;

    SECTION("Parent*/Follows*") {
        std::string op = GENERATE("Follows*", "Parent*");
        PQLStringLexer lexer(op);
        tokens = lexer.tokens();

        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == op);

        auto end = tokens->pop_front();
        REQUIRE(end->get_type() == TokenType::END);
    }

    SECTION("Parent */Follows *") {
        std::string op = GENERATE("Follows", "Parent");
        std::string query = op + " *";
        PQLStringLexer lexer(query);
        REQUIRE_THROWS(lexer.tokens());
    }

    SECTION("such that tokenization") {
        std::string query = "such that";
        std::string query_1 = "such    that";
        std::string query_2 = "  suchthat  ";
        PQLStringLexer lexer(query);
        PQLStringLexer lexer_1(query_1);
        PQLStringLexer lexer_2(query_2);

        tokens = lexer.tokens();
        auto first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "such that");

        tokens = lexer_1.tokens();
        first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "such");
        auto second = tokens->pop_front();
        REQUIRE(second->get_type() == TokenType::WORD);
        REQUIRE(second->get_value() == "that");

        tokens = lexer_2.tokens();
        first = tokens->pop_front();
        REQUIRE(first->get_type() == TokenType::WORD);
        REQUIRE(first->get_value() == "suchthat");
    }
}
