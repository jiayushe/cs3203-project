#include "PQLLexer.h"
#include <cctype>
#include <stdexcept>
#include <string>

using namespace Parser;

PQLLexer::PQLLexer(std::shared_ptr<Source> source) : source(std::move(source)) {}

std::shared_ptr<Token> PQLLexer::peek_token() {
    int pos = source->current_pos();
    std::shared_ptr<Token> temp_token = pop_token();
    source->reset_pos(pos);
    return temp_token;
}

std::shared_ptr<Token> PQLLexer::pop_token() {
    if (!source->has_more()) {
        return std::make_shared<Token>(TokenType::END, "END");
    }

    int next = source->pop_char();
    if (isspace(next)) {
        return pop_token();
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
        while (source->has_more() && isalnum(source->peek_char())) {
            value.push_back(source->pop_char());
        }

        // Handle "such that" keyword
        if (value == "such" && source->has_more(5) && source->peek_string(5) == " that") {
            value += source->pop_string(5);
        }

        // Handle "Parent*" and "Follows*" keywords
        if ((value == "Parent" || value == "Follows") && source->has_more() && source->peek_char() == '*') {
            value.push_back(source->pop_char());
        }

        return std::make_shared<Token>(TokenType::WORD, value);
    }

    throw std::runtime_error("Invalid token '" + std::string(1, next) + "'");
}
