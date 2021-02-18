#pragma once

#include "SimpleLexer.h"

namespace Parser {
class SimpleStringLexer : public SimpleLexer {
public:
    SimpleStringLexer(const std::string& value);

protected:
    int pos;
    std::string value;

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};
} // namespace Parser