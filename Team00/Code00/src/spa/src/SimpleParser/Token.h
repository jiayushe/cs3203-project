#ifndef SIMPLEPARSER_TOKEN_H
#define SIMPLEPARSER_TOKEN_H

#include <string>

namespace SimpleParser {

    // All possible token types for the SIMPLE language.
    enum TokenType {
        INVALID_TOKEN,

        WORD,           // Consecutive alphanumeric characters

        LCURL,          // {
        RCURL,          // }
        LPAREN,         // (
        RPAREN,         // )

        GT,             // >
        GTE,            // >=
        LT,             // <
        LTE,            // <=
        DEQUAL,         // ==
        NEQUAL,         // !=

        NOT,           // !
        AND,            // &&
        OR,             // ||

        EQUAL,          // =
        PLUS,           // +
        MINUS,          // -
        ASTERISK,       // *
        SLASH,          // /
        PERCENT,        // %

        SEMICOLON,      // ;

        END,            // EOF
    };

    // A single token (in parser parlance) for the SIMPLE language.
    class Token {
    public:
        Token(TokenType type, std::string value);

        // Gets the type of the token.
        TokenType get_type();

        // Gets the value which the token stands for.
        std::string get_value();

    private:
        TokenType type;
        std::string value;
    };

}

#endif //SIMPLEPARSER_TOKEN_H
