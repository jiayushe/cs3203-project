#include "Token.h"

using namespace Parser;

Token::Token(TokenType type, const std::string& value) : type(type), value(value) {}

TokenType Token::get_type() { return type; }

std::string Token::get_value() { return value; }
