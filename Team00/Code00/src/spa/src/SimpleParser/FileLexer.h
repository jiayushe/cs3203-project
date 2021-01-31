#ifndef SIMPLEPARSER_FILELEXER_H
#define SIMPLEPARSER_FILELEXER_H

#include <string>
#include <fstream>
#include "Lexer.h"

namespace SimpleParser {

    // Lexer for the SIMPLE language (string to tokens) which reads
    // the SIMPLE source from the given filename.
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
