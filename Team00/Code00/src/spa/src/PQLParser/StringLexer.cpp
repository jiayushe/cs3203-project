//
// Created by Victor Varian on 30/1/21.
//

#include "StringLexer.h"

using namespace PQLParser;

StringLexer::StringLexer(const std::string &value) : pos(0), value(value) {}

bool StringLexer::has_more() {
    return pos < value.size();
}

int StringLexer::pop_char() {
    int result = peek_char();
    pos += 1;
    return result;
}

int StringLexer::peek_char() {
    if (!has_more()) {
        throw "String is empty";
    }
    return value[pos];
}