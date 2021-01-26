#include "TokenList.h"

using namespace SimpleParser;

TokenList::~TokenList() {
    for (auto const token : tokens) {
        delete token;
    }
}

Token *TokenList::front() {
    if (pos >= tokens.size()) {
        throw "TokenList is empty";
    }
    return tokens[pos];
}

Token *TokenList::pop_front() {
    auto token = front();
    pos += 1;
    return token;
}

void TokenList::push_back(Token *token) {
    tokens.push_back(token);
}

int TokenList::current_pos() {
    return pos;
}

void TokenList::reset_pos(int new_pos) {
    pos = new_pos;
}
