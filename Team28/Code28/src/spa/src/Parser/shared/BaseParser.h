#pragma once

#include "Parser/PQLParser/DesignEntity.h"
#include "Parser/shared/Token.h"
#include "Parser/shared/TokenList.h"
#include <memory>
#include <vector>

namespace Parser {

// Parser for PQL + SIMPLE. This is a base class containing helper methods
// which parsers for both PQL and SIMPLE should find useful.
class BaseParser {
protected:
    std::shared_ptr<TokenList> tokens;

    // Construct a BaseParser instance.
    // Caller is responsible for cleaning up the memory for the supplied token list.
    BaseParser(std::shared_ptr<TokenList> tokens);

    // Registers that the next token should be of the expected type.
    // Throws if the expectation is not fulfilled.
    std::shared_ptr<Token> expect_token(TokenType expected_type);

    // Registers that the next token should be any of the expected types.
    // Throws if the expectation is not fulfilled.
    std::shared_ptr<Token> expect_token(const std::vector<TokenType>& expected_types);

    // Registers that the next token should be a WORD token with the expected value.
    // Throws if the expectation is not fulfilled.
    std::shared_ptr<Token> expect_word(const std::string& expected_value);

    // Registers that the next token should be a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    // Throws if the expectation is not fulfilled.
    std::shared_ptr<Token> expect_name(const std::string& identifier);

    // Registers that the next token should be a WORD token whose value is an integer
    // (a numeric string).
    // Throws if the expectation is not fulfilled.
    std::shared_ptr<Token> expect_integer(const std::string& identifier);

    // Checks if the given token type matches with the expected type.
    static bool is_token_type(std::shared_ptr<Token> token, TokenType expected_type);

    // Checks if the given token type matches with any of the expected types.
    static bool is_word(std::shared_ptr<Token> token, const std::string& expected_value);

    // Checks if the given token is a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    static bool is_name(std::shared_ptr<Token> token);

    // Checks if the given token is a WORD token whose value is an integer
    // (a numeric string).
    static bool is_integer(std::shared_ptr<Token> token);

    static std::string to_string(const std::vector<TokenType>& token_types);
};

} // namespace Parser
