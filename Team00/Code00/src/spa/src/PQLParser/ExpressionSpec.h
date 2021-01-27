//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_EXPRESSIONSPEC_H
#define PQLPARSER_EXPRESSIONSPEC_H

//#include "SimpleParser/Node.h"

namespace PQLParser {
    enum class ExpressionSpecType {
        ANY,
        PATTERN,
    };

    class ExpressionSpec {
    public:
        ExpressionSpecType get_type();
        // SimpleParser::Node *get_pattern();

        ExpressionSpec(ExpressionSpecType type);

    private:
        ExpressionSpecType type;
        // SimpleParser::Node *pattern;

    };

}

#endif //PQLPARSER_EXPRESSIONSPEC_H
