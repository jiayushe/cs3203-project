#pragma once

#include "Parser/shared/BaseLexer.h"

namespace Parser {
class PQLStringLexer : public BaseLexer {
public:
    PQLStringLexer(const std::string& filename);

protected:
    int pos;
    std::string value;

    std::shared_ptr<Token> next_token() override;

    std::string pop_string(int n);

    std::string peek_string(int n);

    bool has_more(int n);

    bool has_more() override;

    int pop_char() override;

    int peek_char() override;
};
} // namespace Parser