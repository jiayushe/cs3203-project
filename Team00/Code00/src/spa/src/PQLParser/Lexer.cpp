//
// Created by Victor Varian on 30/1/21.
//

#include <string>
#include <cctype>
#include <iostream>
#include "Lexer.h"

using namespace PQLParser;

TokenList *Lexer::tokens() {
    auto tokens = new TokenList();
    Token *token;
    do {
        token = next_token();
        tokens->push_back(token);
    } while (token->get_type() != TokenType::END);
    return tokens;
}

Token *Lexer::next_token() {
    if (!has_more()) {
        return new Token(TokenType::END, "END");
    }

    int next = pop_char();
    if (isspace(next)) {
        return next_token();
    }

    // TODO add token from SimpleLexer
    switch (next) {
        case '(':
            return new Token(TokenType::LPAREN, "(");
        case ')':
            return new Token(TokenType::RPAREN, ")");
        case ',':
            return new Token(TokenType::COMMA, ",");
        case '_':
            return new Token(TokenType::UNDERSCORE, "_");
        case ';':
            return new Token(TokenType::SEMICOLON, ";");
        case '\"':
            return new Token(TokenType::DOUBLE_QUOTE, "\"");
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

        return new Token(TokenType::WORD, value);
    }

    throw "Invalid token '" + std::string(1, next) + "'";
}