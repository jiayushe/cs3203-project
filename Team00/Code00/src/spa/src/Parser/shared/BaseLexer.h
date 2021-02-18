#pragma once

#include "Token.h"
#include "TokenList.h"
#include <memory>

namespace Parser {

// Lexer for PQL + SIMPLE (converts string to tokens).
class BaseLexer {
public:
    // Gets a list of tokens representing the supplied source.
    std::shared_ptr<TokenList> tokens();

protected:
    virtual std::shared_ptr<Token> next_token() = 0;

    virtual bool has_more() = 0;

    virtual int pop_char() = 0;

    virtual int peek_char() = 0;
};

} // namespace Parser