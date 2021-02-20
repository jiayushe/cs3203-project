#include "PQLStringLexer.h"
#include "Parser/shared/TokenList.h"
#include <cctype>
#include <string>

using namespace Parser;

PQLStringLexer::PQLStringLexer(const std::string& value) : pos(0), value(value) {}

bool PQLStringLexer::has_more() { return pos < value.size(); }

bool PQLStringLexer::has_more(int n) { return pos + n - 1 < value.size(); }

int PQLStringLexer::pop_char() {
    int result = peek_char();
    pos += 1;
    return result;
}

std::string PQLStringLexer::pop_string(int n) {
    std::string str = peek_string(n);
    pos += n;
    return str;
}

std::string PQLStringLexer::peek_string(int n) {
    std::string str;
    int tmp_pos = pos;

    while (n > 0) {
        str += value[tmp_pos];
        tmp_pos += 1;
        n--;
    }
    return str;
}

int PQLStringLexer::peek_char() {
    if (!has_more()) {
        throw "String is empty";
    }
    return value[pos];
}

std::shared_ptr<Token> PQLStringLexer::next_token() {
    if (!has_more()) {
        return std::make_shared<Token>(TokenType::END, "END");
    }

    int next = pop_char();
    if (isspace(next)) {
        return next_token();
    }

    switch (next) {
    case '(':
        return std::make_shared<Token>(TokenType::LPAREN, "(");
    case ')':
        return std::make_shared<Token>(TokenType::RPAREN, ")");
    case ';':
        return std::make_shared<Token>(TokenType::SEMICOLON, ";");
    case ',':
        return std::make_shared<Token>(TokenType::COMMA, ",");
    case '_':
        return std::make_shared<Token>(TokenType::UNDERSCORE, "_");
    case '\"':
        return std::make_shared<Token>(TokenType::DOUBLE_QUOTE, "\"");
    default:
        // Fall through - other cases are checked below
        break;
    }

    if (isalnum(next)) {
        std::string value;
        value.push_back(next);
        while (has_more() && isalnum(peek_char())) {
            value.push_back(pop_char());
        }

        // Handle "such that" keyword
        if (value == "such" && has_more(5) && peek_string(5) == " that") {
            value += pop_string(5);
        }

        // Handle "Parent*" and "Follows*" keywords
        if ((value == "Parent" || value == "Follows") && has_more() && peek_char() == '*') {
            value.push_back(pop_char());
        }

        return std::make_shared<Token>(TokenType::WORD, value);
    }

    throw "Invalid token '" + std::string(1, next) + "'";
}
