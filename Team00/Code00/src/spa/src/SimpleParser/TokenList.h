#ifndef SIMPLEPARSER_TOKENLIST_H
#define SIMPLEPARSER_TOKENLIST_H

#include "Token.h"
#include <vector>

namespace SimpleParser {

// List of tokens (see Token.h) which supports quick O(1) rewind operation.
// See Parser.cpp to understand better how this list of tokens is used to enable efficient parsing.
class TokenList {
public:
    ~TokenList();

    // Gets a token from the front of the list (the token isn't removed).
    // Throws if list is empty.
    Token* front();

    // Gets and removes a token from the front of the list.
    // Throws if list is empty.
    Token* pop_front();

    // Adds a new token to the back of the list.
    void push_back(Token* token);

    // Gets the current position of the list head.
    // This method is used in conjunction with reset_pos to enable a quick rewind operation.
    int current_pos();

    // Resets the position of the list head to the supplied value.
    // This method is used in conjunction with current_pos to enable a quick rewind operation.
    // Throws if the supplied position is out of bounds.
    void reset_pos(int pos);

private:
    int pos;
    std::vector<Token*> tokens;
};

} // namespace SimpleParser

#endif // SIMPLEPARSER_TOKENLIST_H
