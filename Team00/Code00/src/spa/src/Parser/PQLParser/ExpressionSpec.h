#pragma once

#include "Parser/SimpleParser/SimpleNode.h"
#include <memory>

namespace Parser {
enum class ExpressionSpecType {
    INVALID,
    ANY,
    PATTERN,
};

class ExpressionSpec {
public:
    ExpressionSpecType get_type();
    std::shared_ptr<Parser::SimpleNode> get_pattern();

    ExpressionSpec();

    void set_pattern(std::shared_ptr<Parser::SimpleNode> pattern);

    void set_type(ExpressionSpecType expression_spec_type);

private:
    ExpressionSpecType type;
    std::shared_ptr<Parser::SimpleNode> pattern;
};

} // namespace Parser
