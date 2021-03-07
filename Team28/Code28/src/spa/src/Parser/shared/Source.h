#pragma once

#include "Token.h"
#include <memory>

namespace Parser {

// Source for PQL + SIMPLE (converts string to tokens).
class Source {
public:
    Source(const std::string& value, bool is_filename = false);

    std::string pop_string(int n);

    std::string peek_string(int n);

    bool has_more(int n);

    bool has_more();

    int pop_char();

    int peek_char();

    int current_pos();

    void reset_pos(int new_pos);

private:
    int pos;
    std::string value;
};

} // namespace Parser