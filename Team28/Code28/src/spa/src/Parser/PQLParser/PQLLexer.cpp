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
    case '<':
        return std::make_shared<Token>(TokenType::LT, "<");
    case '>':
        return std::make_shared<Token>(TokenType::GT, ">");
    case '.':
        return std::make_shared<Token>(TokenType::DOT, ".");
    case '=':
        return std::make_shared<Token>(TokenType::EQUAL, "=");
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

        // Handle "prog_line" keyword
        if (value == "prog" && source->has_more(5) && source->peek_string(5) == "_line") {
            value += source->pop_string(5);
        }

        // Handle "such that" keyword
        if (value == "such" && source->has_more(5) && source->peek_string(5) == " that") {
            value += source->pop_string(5);
        }

        // Handle "stmt#" keyword
        if (value == "stmt" && source->has_more(1) && source->peek_string(1) == "#") {
            value += source->pop_string(1);
        }

        // Handle "Parent*", "Follows*", "Calls*", "Next*", and "Affects*" keywords
        if ((value == "Parent" || value == "Follows" || value == "Calls" || value == "Next" ||
             value == "Affects" || value == "NextBip" || value == "AffectsBip") &&
            source->has_more() && source->peek_char() == '*') {
            value.push_back(source->pop_char());
        }

        return std::make_shared<Token>(TokenType::WORD, value);
    }

    throw std::runtime_error("Invalid token '" + std::string(1, next) + "'");
}
