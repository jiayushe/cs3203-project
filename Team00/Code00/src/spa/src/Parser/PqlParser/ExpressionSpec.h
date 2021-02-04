#pragma once

#include "Parser/SimpleParser/SimpleNode.h"

namespace Parser {
    enum class ExpressionSpecType {
        ANY,
        PATTERN,
    };

    class ExpressionSpec {
    public:
        ExpressionSpecType get_type();
        Parser::SimpleNode* get_pattern();

        ExpressionSpec();

        ~ExpressionSpec();

        void set_pattern(Parser::SimpleNode* pattern);

        void set_type(ExpressionSpecType expression_spec_type);

    private:
        ExpressionSpecType type;
        Parser::SimpleNode* pattern;
    };

} // namespace Parser
