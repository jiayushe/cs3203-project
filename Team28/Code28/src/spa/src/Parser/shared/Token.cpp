#include "Token.h"
#include <stdexcept>

using namespace Parser;

Token::Token(TokenType type, const std::string& value) : type(type), value(value) {}

TokenType Token::get_type() { return type; }

std::string Token::get_value() { return value; }

std::string Parser::to_string(TokenType token_type) {
    switch (token_type) {
    case TokenType::INVALID:
        return "INVALID";
    case TokenType::WORD:
        return "WORD";
    case TokenType::LCURL:
        return "LCURL";
    case TokenType::RCURL:
        return "RCURL";
    case TokenType::LPAREN:
        return "LPAREN";
    case TokenType::RPAREN:
        return "RPAREN";
    case TokenType::GT:
        return "GT";
    case TokenType::GTE:
        return "GTE";
    case TokenType::LT:
        return "LT";
    case TokenType::LTE:
        return "LTE";
    case TokenType::DEQUAL:
        return "DEQUAL";
    case TokenType::NEQUAL:
        return "NEQUAL";
    case TokenType::NOT:
        return "NOT";
    case TokenType::AND:
        return "AND";
    case TokenType::OR:
        return "OR";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::ASTERISK:
        return "ASTERISK";
    case TokenType::SLASH:
        return "SLASH";
    case TokenType::PERCENT:
        return "PERCENT";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::DOUBLE_QUOTE:
        return "DOUBLE_QUOTE";
    case TokenType::UNDERSCORE:
        return "UNDERSCORE";
    case TokenType::END:
        return "END";
    default:
        throw std::runtime_error("Unknown token type");
    }
}
