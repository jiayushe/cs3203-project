#pragma once

#include "Parser/SimpleParser/SimpleNode.h"
#include <memory>

namespace Parser {
enum class ExpressionSpecType { INVALID, ANY, PATTERN, EXACT_PATTERN };

class ExpressionSpec {
public:
    ExpressionSpecType get_type() const;
    std::shared_ptr<Parser::SimpleNode> get_pattern() const;
    std::shared_ptr<Parser::SimpleNode> get_exact_pattern() const;

    ExpressionSpec();

    void set_type(ExpressionSpecType expression_spec_type);
    void set_pattern(std::shared_ptr<Parser::SimpleNode> pattern);
    void set_exact_pattern(std::shared_ptr<Parser::SimpleNode> exact_pattern);

private:
    ExpressionSpecType type;
    std::shared_ptr<Parser::SimpleNode> pattern;
    std::shared_ptr<Parser::SimpleNode> exact_pattern;
};

} // namespace Parser
