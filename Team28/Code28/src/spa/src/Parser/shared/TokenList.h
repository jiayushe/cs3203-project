#pragma once

#include "Token.h"
#include <memory>
#include <vector>

namespace Parser {

// List of tokens (see Token.h) which supports quick O(1) rewind operation.
// Useful for supporting parser combinator logic. See BaseParser.cpp for more info.
class TokenList {
public:
    // Gets a token from the front of the list (the token isn't removed).
    // Throws if list is empty.
    std::shared_ptr<Token> front();

    // Gets and removes a token from the front of the list.
    // Throws if list is empty.
    std::shared_ptr<Token> pop_front();

    // Adds a new token to the back of the list.
    void push_back(std::shared_ptr<Token> token);

    // Gets the current position of the list head.
    // This method is used in conjunction with reset_pos to enable a quick rewind operation.
    int current_pos();

    // Resets the position of the list head to the supplied value.
    // This method is used in conjunction with current_pos to enable a quick rewind operation.
    // Throws if the supplied position is out of bounds.
    void reset_pos(int pos);

private:
    int pos;
    std::vector<std::shared_ptr<Token>> tokens;
};

} // namespace Parser
