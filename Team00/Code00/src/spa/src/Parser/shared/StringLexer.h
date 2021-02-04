#pragma once

#include "Lexer.h"
#include <string>

namespace Parser {

// Lexer for the SIMPLE language (string to tokens) which reads
// the SIMPLE source as a std::string.
class StringLexer : public Lexer {
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
