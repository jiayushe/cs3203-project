#ifndef SIMPLEPARSER_STRINGLEXER_H
#define SIMPLEPARSER_STRINGLEXER_H

#include <string>
#include "Lexer.h"

namespace SimpleParser {

    // TODO:
    class StringLexer : public Lexer {
    public:
        StringLexer(const std::string &value);

    private:
        int pos;
        std::string value;

        bool has_more() override;

        int pop_char() override;

        int peek_char() override;
    };

}

#endif //SIMPLEPARSER_STRINGLEXER_H
