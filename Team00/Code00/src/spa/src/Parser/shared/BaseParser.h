#pragma once

#include "Parser/shared/Token.h"
#include "Parser/shared/TokenList.h"
#include <vector>

namespace Parser {

// TODO:
class BaseParser {
protected:
    TokenList* tokens;

    // TODO:
    BaseParser(TokenList* tokens);

    // Registers that the next token should be of the expected type.
    // Throws if the expectation is not fulfilled.
    Token* expect_token(TokenType expected_type);

    // Registers that the next token should be any of the expected types.
    // Throws if the expectation is not fulfilled.
    Token* expect_token(const std::vector<TokenType>& expected_types);

    // Registers that the next token should be a WORD token with the expected value.
    // Throws if the expectation is not fulfilled.
    Token* expect_word(const std::string& expected_value);

    // Registers that the next token should be a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    // Throws if the expectation is not fulfilled.
    Token* expect_name(const std::string& identifier);

    // Registers that the next token should be a WORD token whose value is an integer
    // (a numeric string).
    // Throws if the expectation is not fulfilled.
    Token* expect_integer(const std::string& identifier);

    // Checks if the given token type matches with the expected type.
    static bool is_token_type(Token* token, TokenType expected_type);

    // Checks if the given token type matches with any of the expected types.
    static bool is_word(Token* token, const std::string& expected_value);

    // Checks if the given token is a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    static bool is_name(Token* token);

    // Checks if the given token is a WORD token whose value is an integer
    // (a numeric string).
    static bool is_integer(Token* token);

    static std::string to_string(const std::vector<TokenType>& token_types);
};

} // namespace Parser
