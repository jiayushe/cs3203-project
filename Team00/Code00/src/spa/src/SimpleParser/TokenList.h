#ifndef SIMPLEPARSER_TOKENLIST_H
#define SIMPLEPARSER_TOKENLIST_H

#include <vector>
#include "Token.h"

namespace SimpleParser {

    // TODO: List of tokens, i.e. SimpleParser::Token, which supports quick rewind operation.
    class TokenList {
    public:
        ~TokenList();

        // TODO:
        Token *front();

        // TODO:
        Token *pop_front();

        // TODO:
        void push_back(Token *token);

        // TODO:
        int current_pos();

        // TODO:
        void reset_pos(int pos);

    private:
        int pos;
        std::vector<Token *> tokens;
    };

}

#endif //SIMPLEPARSER_TOKENLIST_H
