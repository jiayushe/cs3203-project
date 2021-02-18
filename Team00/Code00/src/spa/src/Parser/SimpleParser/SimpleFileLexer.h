#pragma once

#include "SimpleLexer.h"
#include <fstream>

namespace Parser {
class SimpleFileLexer : public SimpleLexer {
public:
    SimpleFileLexer(const std::string& filename);

protected:
    std::ifstream file;

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};
} // namespace Parser