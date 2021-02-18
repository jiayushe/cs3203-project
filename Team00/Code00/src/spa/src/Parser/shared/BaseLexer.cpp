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