#pragma once

#include "Token.h"
#include "TokenList.h"

namespace Parser {

// Lexer for PQL + SIMPLE (converts string to tokens).
class BaseLexer {
public:
    // Gets a list of tokens representing the supplied source.
    TokenList* tokens();

private:
    Token* next_token();

    virtual bool has_more() = 0;

    virtual int pop_char() = 0;

    virtual int peek_char() = 0;
};

} // namespace Parser
