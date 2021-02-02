#include "ExpressionSpec.h"

using namespace PQLParser;

ExpressionSpec::ExpressionSpec() {
    this->type = ExpressionSpecType::ANY;
}

ExpressionSpec::ExpressionSpec(SimpleParser::Node *pattern) {
    this->type = ExpressionSpecType::PATTERN;
    this->pattern = pattern;
}

ExpressionSpec::~ExpressionSpec() {

}

ExpressionSpecType ExpressionSpec::get_type() {
    return type;
}

SimpleParser::Node *ExpressionSpec::get_pattern() {
    if (type != ExpressionSpecType::PATTERN) {
        throw "ExpressionSpecType is not PATTERN type";
    }

    return pattern;
}