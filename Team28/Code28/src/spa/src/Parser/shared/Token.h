#pragma once

#include <string>
#include <unordered_map>

namespace Parser {

// All possible token types for PQL + SIMPLE.
enum class TokenType {
    INVALID,

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
    DOT,          // .

    END, // EOF
};

// Gets friendly string representing the different token types.
std::string to_string(TokenType token_type);

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
