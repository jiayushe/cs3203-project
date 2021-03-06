#pragma once

#include "Parser/shared/BaseLexer.h"
#include "Parser/shared/Source.h"
#include <memory>

namespace Parser {
    class PQLLexer : public BaseLexer {
    public:
        explicit PQLLexer(std::shared_ptr<Source> source);

        std::shared_ptr<Token> peek_token() override;

        std::shared_ptr<Token> pop_token() override;

    private:
        std::shared_ptr<Source> source;
    };
} // namespace Parser