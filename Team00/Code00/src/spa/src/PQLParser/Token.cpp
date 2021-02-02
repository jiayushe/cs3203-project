#include "Token.h"

using namespace PQLParser;

Token::Token(TokenType type, std::string value) : type(type), value(value) {}

TokenType Token::get_type() {
    return type;
}

std::string Token::get_value() {
    return value;
}