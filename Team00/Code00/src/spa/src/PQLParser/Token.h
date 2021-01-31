//
// Created by Victor Varian on 30/1/21.
//

#ifndef AUTOTESTER_TOKEN_H
#define AUTOTESTER_TOKEN_H

#include <string>

namespace PQLParser {

    // All possible token types for the SIMPLE language.
    enum TokenType {
        INVALID_TOKEN,

        WORD,           // Consecutive alphanumeric characters

        LPAREN,         // (
        RPAREN,         // )

        GT,             // >
        GTE,            // >=
        LT,             // <
        LTE,            // <=
        DEQUAL,         // ==
        NEQUAL,         // !=

        COMMA,          // ,
        EQUAL,          // =
        PLUS,           // +
        MINUS,          // -
        ASTERISK,       // *
        UNDERSCORE,     // _
        SEMICOLON,      // ;
        DOUBLE_QUOTE,   // "

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

#endif //AUTOTESTER_TOKEN_H
