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
    ExpressionSpecType get_type() const;
    std::shared_ptr<Parser::SimpleNode> get_pattern() const;

    ExpressionSpec();

    void set_type(ExpressionSpecType expression_spec_type);
    void set_pattern(std::shared_ptr<Parser::SimpleNode> pattern);

private:
    ExpressionSpecType type;
    std::shared_ptr<Parser::SimpleNode> pattern;
};

} // namespace Parser
