#pragma once

#include "BaseLexer.h"
#include <string>

namespace Parser {

// Lexer for PQL + SIMPLE (string to tokens) which reads the source as a std::string.
class StringLexer : public BaseLexer {
public:
    explicit StringLexer(const std::string& value);

private:
    int pos;
    std::string value;

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};

} // namespace Parser
