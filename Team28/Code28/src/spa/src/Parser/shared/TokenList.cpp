#include "TokenList.h"
#include <stdexcept>

using namespace Parser;

std::shared_ptr<Token> TokenList::front() {
    if (pos >= tokens.size()) {
        throw std::runtime_error("TokenList is empty");
    }
    return tokens[pos];
}

std::shared_ptr<Token> TokenList::pop_front() {
    auto token = front();
    pos += 1;
    return token;
}

void TokenList::push_back(std::shared_ptr<Token> token) { tokens.push_back(token); }

int TokenList::current_pos() { return pos; }

void TokenList::reset_pos(int new_pos) {
    if (new_pos < 0 || new_pos >= tokens.size()) {
        throw std::runtime_error("Invalid position supplied");
    }
    pos = new_pos;
}
