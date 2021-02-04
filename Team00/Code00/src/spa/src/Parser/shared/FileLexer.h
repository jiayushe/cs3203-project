#pragma once

#include "BaseLexer.h"
#include <fstream>
#include <string>

namespace Parser {

// Lexer for PQL + SIMPLE (string to tokens) which reads the source from the given filename.
class FileLexer : public BaseLexer {
public:
    explicit FileLexer(const std::string& filename);

private:
    std::ifstream file;

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};

} // namespace Parser
