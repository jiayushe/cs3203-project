#pragma once

#include "Parser/SimpleParser/SimpleNode.h"

namespace Parser {
enum ExpressionSpecType {
    ANY,
    PATTERN,
};

class ExpressionSpec {
public:
    ExpressionSpecType get_type();
    SimpleParser::Node* get_pattern();

    ExpressionSpec();

    ExpressionSpec(SimpleParser::Node* pattern);

    ~ExpressionSpec();

private:
    ExpressionSpecType type;
    SimpleParser::Node* pattern;
};

} // namespace Parser
