#include "BaseLexer.h"
#include <cctype>
#include <string>

using namespace Parser;

std::shared_ptr<TokenList> BaseLexer::tokens() {
    auto tokens = std::make_shared<TokenList>();
    std::shared_ptr<Token> token;
    do {
        token = next_token();
        tokens->push_back(token);
    } while (token->get_type() != TokenType::END);
    return tokens;
}

std::shared_ptr<Token> BaseLexer::next_token() {
    if (!has_more()) {
        return std::make_shared<Token>(TokenType::END, "END");
    }

    int next = pop_char();
    if (isspace(next)) {
        return next_token();
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
        if (has_more() && peek_char() == '=') {
            pop_char();
            return std::make_shared<Token>(TokenType::GTE, ">=");
        }
        return std::make_shared<Token>(TokenType::GT, ">");
    case '<':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return std::make_shared<Token>(TokenType::LTE, "<=");
        }
        return std::make_shared<Token>(TokenType::LT, "<");
    case '=':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return std::make_shared<Token>(TokenType::DEQUAL, "==");
        }
        return std::make_shared<Token>(TokenType::EQUAL, "=");
    case '!':
        if (has_more() && peek_char() == '=') {
            pop_char();
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

    if (next == '&' && has_more() && peek_char() == '&') {
        pop_char();
        return std::make_shared<Token>(TokenType::AND, "&&");
    }

    if (next == '|' && has_more() && peek_char() == '|') {
        pop_char();
        return std::make_shared<Token>(TokenType::OR, "||");
    }

    if (isalnum(next)) {
        std::string value;
        value.push_back(next);
        while (has_more() && isalnum(peek_char())) {
            value.push_back(pop_char());
        }
        return std::make_shared<Token>(TokenType::WORD, value);
    }

    throw "Invalid token '" + std::string(1, next) + "'";
}
