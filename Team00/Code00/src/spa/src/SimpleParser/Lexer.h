#ifndef SIMPLEPARSER_LEXER_H
#define SIMPLEPARSER_LEXER_H

#include "Token.h"
#include "TokenList.h"

namespace SimpleParser {

    // Lexer for the SIMPLE language (converts string to tokens).
    class Lexer {
    public:
        // Gets a list of tokens representing the supplied SIMPLE source.
        TokenList *tokens();

    private:
        Token *next_token();

        virtual bool has_more() = 0;

        virtual int pop_char() = 0;

        virtual int peek_char() = 0;
    };

}

#endif //SIMPLEPARSER_LEXER_H
