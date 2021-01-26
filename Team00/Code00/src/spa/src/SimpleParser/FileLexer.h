#ifndef SIMPLEPARSER_FILELEXER_H
#define SIMPLEPARSER_FILELEXER_H

#include <string>
#include <fstream>
#include "Lexer.h"

namespace SimpleParser {

    // TODO:
    class FileLexer : public Lexer {
    public:
        FileLexer(const std::string &filename);

    private:
        std::ifstream file;

        bool has_more() override;

        int pop_char() override;

        int peek_char() override;
    };

}

#endif //SIMPLEPARSER_FILELEXER_H
