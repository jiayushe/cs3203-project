#pragma once

#include "Token.h"
#include <memory>

namespace Parser {
class BaseLexer {
public:
    virtual std::shared_ptr<Token> peek_token() = 0;
    virtual std::shared_ptr<Token> pop_token() = 0;
};
} // namespace Parser