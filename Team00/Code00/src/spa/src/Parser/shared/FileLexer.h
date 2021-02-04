#pragma once

#include "Lexer.h"
#include <fstream>
#include <string>

namespace Parser {

// Lexer for the SIMPLE language (string to tokens) which reads
// the SIMPLE source from the given filename.
class FileLexer : public Lexer {
public:
    explicit FileLexer(const std::string& filename);

private:
    std::ifstream file;

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};

} // namespace Parser
