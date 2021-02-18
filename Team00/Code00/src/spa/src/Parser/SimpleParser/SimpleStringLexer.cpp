#include "SimpleStringLexer.h"
#include "Parser/shared/TokenList.h"
#include <cctype>
#include <string>

using namespace Parser;

SimpleStringLexer::SimpleStringLexer(const std::string& value) : pos(0), value(value) {}

bool SimpleStringLexer::has_more() { return pos < value.size(); }

int SimpleStringLexer::pop_char() {
    int result = peek_char();
    pos += 1;
    return result;
}

int SimpleStringLexer::peek_char() {
    if (!has_more()) {
        throw "String is empty";
    }
    return value[pos];
}