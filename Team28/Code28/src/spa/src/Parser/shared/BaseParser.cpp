#include "BaseParser.h"
#include <algorithm>
#include <stdexcept>

using namespace Parser;

BaseParser::BaseParser(std::shared_ptr<BaseLexer> lexer) : lexer(std::move(lexer)) {}

std::shared_ptr<Token> BaseParser::expect_token(TokenType expected_type) {
    auto token = lexer->pop_token();
    if (!is_token_type(token, expected_type)) {
        throw std::runtime_error("Expected token type " + Parser::to_string(expected_type) +
                                 ", received " + Parser::to_string(token->get_type()));
    }
    return token;
}

std::shared_ptr<Token> BaseParser::expect_token(const std::vector<TokenType>& expected_types) {
    auto token = lexer->pop_token();
    for (auto const& expected_type : expected_types) {
        if (is_token_type(token, expected_type)) {
            return token;
        }
    }
    throw std::runtime_error("Expected token type " + to_string(expected_types) + ", received " +
                             Parser::to_string(token->get_type()));
}

std::shared_ptr<Token> BaseParser::expect_word(const std::string& expected_value) {
    auto token = lexer->pop_token();
    if (!is_word(token, expected_value)) {
        throw std::runtime_error("Expected word token with value '" + expected_value +
                                 "', received '" + token->get_value() + "'");
    }
    return token;
}

std::shared_ptr<Token> BaseParser::expect_name(const std::string& identifier) {
    auto token = lexer->pop_token();
    if (!is_name(token)) {
        throw std::runtime_error("Expected name token " + identifier + ", received '" +
                                 token->get_value() + "'");
    }
    return token;
}

std::shared_ptr<Token> BaseParser::expect_integer(const std::string& identifier) {
    auto token = lexer->pop_token();
    if (!is_integer(token)) {
        throw std::runtime_error("Expected integer token " + identifier + ", received '" +
                                 token->get_value() + "'");
    }
    return token;
}

bool BaseParser::is_token_type(std::shared_ptr<Token> token, TokenType expected_type) {
    return token->get_type() == expected_type;
}

bool BaseParser::is_word(std::shared_ptr<Token> token, const std::string& expected_value) {
    return token->get_type() == TokenType::WORD && token->get_value() == expected_value;
}

bool BaseParser::is_name(std::shared_ptr<Token> token) {
    return token->get_type() == TokenType::WORD && !token->get_value().empty() &&
           isalpha(token->get_value().front());
}

bool BaseParser::is_integer(std::shared_ptr<Token> token) {
    auto token_value = token->get_value();
    if (token->get_type() != TokenType::WORD || token_value.empty()) {
        return false;
    }
    return std::all_of(token_value.begin(), token_value.end(), ::isdigit);
}

std::string BaseParser::to_string(const std::vector<TokenType>& token_types) {
    std::string value;
    for (auto const token_type : token_types) {
        if (value.empty()) {
            value += "[" + Parser::to_string(token_type);
        } else {
            value += ", " + Parser::to_string(token_type);
        }
    }
    value += "]";
    return value;
}
