#pragma once

#include <string>

namespace Parser {

// All possible token types for PQL + SIMPLE.
enum TokenType {
    INVALID_TOKEN,

    WORD, // Consecutive alphanumeric characters

    LCURL,  // {
    RCURL,  // }
    LPAREN, // (
    RPAREN, // )

    GT,     // >
    GTE,    // >=
    LT,     // <
    LTE,    // <=
    DEQUAL, // ==
    NEQUAL, // !=

    NOT, // !
    AND, // &&
    OR,  // ||

    EQUAL,    // =
    PLUS,     // +
    MINUS,    // -
    ASTERISK, // *
    SLASH,    // /
    PERCENT,  // %

    SEMICOLON,    // ;
    COMMA,        // ,
    DOUBLE_QUOTE, // "
    UNDERSCORE,   // _

    END, // EOF
};

// A single token (in parser parlance) for PQL + SIMPLE.
class Token {
public:
    Token(TokenType type, const std::string& value);

    // Gets the type of the token.
    TokenType get_type();

    // Gets the value which the token stands for.
    std::string get_value();

private:
    TokenType type;
    std::string value;
};

} // namespace Parser
