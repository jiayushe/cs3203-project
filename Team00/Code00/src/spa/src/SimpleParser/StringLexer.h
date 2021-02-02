#ifndef SIMPLEPARSER_STRINGLEXER_H
#define SIMPLEPARSER_STRINGLEXER_H

#include "Lexer.h"
#include <string>

namespace SimpleParser {

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

} // namespace SimpleParser

#endif // SIMPLEPARSER_STRINGLEXER_H
