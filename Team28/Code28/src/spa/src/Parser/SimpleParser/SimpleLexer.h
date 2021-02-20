#pragma once

#include "Parser/shared/BaseLexer.h"
#include <fstream>

namespace Parser {
class SimpleLexer : public BaseLexer {
protected:
    std::shared_ptr<Token> next_token() override;
};
} // namespace Parser