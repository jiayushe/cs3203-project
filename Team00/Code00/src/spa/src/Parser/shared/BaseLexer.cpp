#include "BaseLexer.h"
#include <cctype>
#include <string>

using namespace Parser;

TokenList* BaseLexer::tokens() {
    auto tokens = new TokenList();
    Token* token;
    do {
        token = next_token();
        tokens->push_back(token);
    } while (token->get_type() != TokenType::END);
    return tokens;
}

Token* BaseLexer::next_token() {
    if (!has_more()) {
        return new Token(TokenType::END, "END");
    }

    int next = pop_char();
    if (isspace(next)) {
        return next_token();
    }

    switch (next) {
    case '{':
        return new Token(TokenType::LCURL, "{");
    case '}':
        return new Token(TokenType::RCURL, "}");
    case '(':
        return new Token(TokenType::LPAREN, "(");
    case ')':
        return new Token(TokenType::RPAREN, ")");
    case '>':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return new Token(TokenType::GTE, ">=");
        }
        return new Token(TokenType::GT, ">");
    case '<':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return new Token(TokenType::LTE, "<=");
        }
        return new Token(TokenType::LT, "<");
    case '=':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return new Token(TokenType::DEQUAL, "==");
        }
        return new Token(TokenType::EQUAL, "=");
    case '!':
        if (has_more() && peek_char() == '=') {
            pop_char();
            return new Token(TokenType::NEQUAL, "!=");
        }
        return new Token(TokenType::NOT, "!");
    case '+':
        return new Token(TokenType::PLUS, "+");
    case '-':
        return new Token(TokenType::MINUS, "-");
    case '*':
        return new Token(TokenType::ASTERISK, "*");
    case '/':
        return new Token(TokenType::SLASH, "/");
    case '%':
        return new Token(TokenType::PERCENT, "%");
    case ';':
        return new Token(TokenType::SEMICOLON, ";");
    case ',':
        return new Token(TokenType::COMMA, ",");
    case '_':
        return new Token(TokenType::UNDERSCORE, "_");
    case '\"':
        return new Token(TokenType::DOUBLE_QUOTE, "\"");
    default:
        // Fall through - other cases are checked below
        break;
    }

    if (next == '&' && has_more() && peek_char() == '&') {
        pop_char();
        return new Token(TokenType::AND, "&&");
    }

    if (next == '|' && has_more() && peek_char() == '|') {
        pop_char();
        return new Token(TokenType::OR, "||");
    }

    if (isalnum(next)) {
        std::string value;
        value.push_back(next);
        while (has_more() && isalnum(peek_char())) {
            value.push_back(pop_char());
        }
        return new Token(TokenType::WORD, value);
    }

    throw "Invalid token '" + std::string(1, next) + "'";
}
