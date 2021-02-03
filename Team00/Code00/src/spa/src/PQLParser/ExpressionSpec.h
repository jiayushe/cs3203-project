#ifndef PQLPARSER_EXPRESSIONSPEC_H
#define PQLPARSER_EXPRESSIONSPEC_H

#include "SimpleParser/Node.h"

namespace PQLParser {
    enum class ExpressionSpecType {
        ANY,
        PATTERN,
    };

    class ExpressionSpec {
    public:
        ExpressionSpecType get_type();
        SimpleParser::Node *get_pattern();

        ExpressionSpec();

        ExpressionSpec(SimpleParser::Node *pattern);

        ~ExpressionSpec();

    private:
        ExpressionSpecType type;
        SimpleParser::Node *pattern;

    };

}

#endif //PQLPARSER_EXPRESSIONSPEC_H
