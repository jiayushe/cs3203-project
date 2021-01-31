//
// Created by Victor Varian on 30/1/21.
//

#ifndef AUTOTESTER_STRINGLEXER_H
#define AUTOTESTER_STRINGLEXER_H

#include <string>
#include "Lexer.h"

namespace PQLParser {

    // Lexer for the SIMPLE language (string to tokens) which reads
    // the SIMPLE source as a std::string.
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

#endif //AUTOTESTER_STRINGLEXER_H
