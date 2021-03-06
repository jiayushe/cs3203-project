#include "SimpleLexer.h"

using namespace Parser;

SimpleLexer::SimpleLexer(std::shared_ptr<Source> source) : source(std::move(source)) {}

std::shared_ptr<Token> SimpleLexer::peek_token() {
    auto pos = source->current_pos();
    auto temp_token = pop_token();
    source->reset_pos(pos);
    return temp_token;
}

std::shared_ptr<Token> SimpleLexer::pop_token() {
    if (!source->has_more()) {
        return std::make_shared<Token>(TokenType::END, "END");
    }

    int next = source->pop_char();
    if (isspace(next)) {
        return pop_token();
    }

    switch (next) {
    case '{':
        return std::make_shared<Token>(TokenType::LCURL, "{");
    case '}':
        return std::make_shared<Token>(TokenType::RCURL, "}");
    case '(':
        return std::make_shared<Token>(TokenType::LPAREN, "(");
    case ')':
        return std::make_shared<Token>(TokenType::RPAREN, ")");
    case '>':
        if (source->has_more() && source->peek_char() == '=') {
            source->pop_char();
            return std::make_shared<Token>(TokenType::GTE, ">=");
        }
        return std::make_shared<Token>(TokenType::GT, ">");
    case '<':
        if (source->has_more() && source->peek_char() == '=') {
            source->pop_char();
            return std::make_shared<Token>(TokenType::LTE, "<=");
        }
        return std::make_shared<Token>(TokenType::LT, "<");
    case '=':
        if (source->has_more() && source->peek_char() == '=') {
            source->pop_char();
            return std::make_shared<Token>(TokenType::DEQUAL, "==");
        }
        return std::make_shared<Token>(TokenType::EQUAL, "=");
    case '!':
        if (source->has_more() && source->peek_char() == '=') {
            source->pop_char();
            return std::make_shared<Token>(TokenType::NEQUAL, "!=");
        }
        return std::make_shared<Token>(TokenType::NOT, "!");
    case '+':
        return std::make_shared<Token>(TokenType::PLUS, "+");
    case '-':
        return std::make_shared<Token>(TokenType::MINUS, "-");
    case '*':
        return std::make_shared<Token>(TokenType::ASTERISK, "*");
    case '/':
        return std::make_shared<Token>(TokenType::SLASH, "/");
    case '%':
        return std::make_shared<Token>(TokenType::PERCENT, "%");
    case ';':
        return std::make_shared<Token>(TokenType::SEMICOLON, ";");
    default:
        // Fall through - other cases are checked below
        break;
    }

    if (next == '&' && source->has_more() && source->peek_char() == '&') {
        source->pop_char();
        return std::make_shared<Token>(TokenType::AND, "&&");
    }

    if (next == '|' && source->has_more() && source->peek_char() == '|') {
        source->pop_char();
        return std::make_shared<Token>(TokenType::OR, "||");
    }

    if (isalnum(next)) {
        std::string value;
        value.push_back(next);
        while (source->has_more() && isalnum(source->peek_char())) {
            value.push_back(source->pop_char());
        }

        return std::make_shared<Token>(TokenType::WORD, value);
    }

    throw std::runtime_error("Invalid token '" + std::string(1, next) + "'");
}
