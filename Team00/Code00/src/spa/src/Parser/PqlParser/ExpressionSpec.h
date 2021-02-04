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
        Parser::SimpleNode* get_pattern();

        ExpressionSpec();

        ExpressionSpec(Parser::SimpleNode* pattern);

        ~ExpressionSpec();

    private:
        ExpressionSpecType type;
        Parser::SimpleNode* pattern;
    };

} // namespace Parser
