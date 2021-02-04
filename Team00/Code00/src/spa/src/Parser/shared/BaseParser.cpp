#include "BaseParser.h"

using namespace Parser;

BaseParser::BaseParser(TokenList* tokens) : tokens(tokens) {}

void BaseParser::expect_equal(std::string given_string, std::string expected_string) {
    if (given_string != expected_string) {
        throw "Expected token type " + expected_string + ", received " + given_string;
    }
}

Token* BaseParser::expect_token(TokenType expected_type) {
    auto token = tokens->pop_front();
    if (!is_token_type(token, expected_type)) {
        throw "Expected token type " + std::to_string(expected_type) + ", received " +
            std::to_string(token->get_type());
    }
    return token;
}

Token* BaseParser::expect_token(const std::vector<TokenType>& expected_types) {
    auto token = tokens->pop_front();
    for (auto const& expected_type : expected_types) {
        if (is_token_type(token, expected_type)) {
            return token;
        }
    }
    throw "Expected token type " + to_string(expected_types) + ", received " +
        std::to_string(token->get_type());
}

Token* BaseParser::expect_word(const std::string& expected_value) {
    auto token = tokens->pop_front();
    if (!is_word(token, expected_value)) {
        throw "Expected word token with value '" + expected_value + "', received '" +
            token->get_value() + "'";
    }
    return token;
}

Token* BaseParser::expect_name(const std::string& identifier) {
    auto token = tokens->pop_front();
    if (!is_name(token)) {
        throw "Expected name token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

Token* BaseParser::expect_integer(const std::string& identifier) {
    auto token = tokens->pop_front();
    if (!is_integer(token)) {
        throw "Expected integer token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

bool BaseParser::is_token_type(Token* token, TokenType expected_type) {
    return token->get_type() == expected_type;
}

bool BaseParser::is_word(Token* token, const std::string& expected_value) {
    return token->get_type() == TokenType::WORD && token->get_value() == expected_value;
}

bool BaseParser::is_name(Token* token) {
    return token->get_type() == TokenType::WORD && !token->get_value().empty() &&
           isalpha(token->get_value().front());
}

bool BaseParser::is_integer(Token* token) {
    auto token_value = token->get_value();
    if (token->get_type() != TokenType::WORD || token_value.empty()) {
        return false;
    }
    return std::all_of(token_value.begin(), token_value.end(),
                       [](char character) { return isdigit(character); });
}

std::string BaseParser::to_string(const std::vector<TokenType>& token_types) {
    std::string value;
    for (auto const token_type : token_types) {
        if (value.empty()) {
            value += "[" + std::to_string(token_type);
        } else {
            value += ", " + std::to_string(token_type);
        }
    }
    value += "]";
    return value;
}
