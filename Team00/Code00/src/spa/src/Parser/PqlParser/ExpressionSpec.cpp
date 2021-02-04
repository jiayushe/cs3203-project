#include "ExpressionSpec.h"

using namespace Parser;

ExpressionSpec::ExpressionSpec() {
    this->type = ExpressionSpecType::ANY;
}

ExpressionSpec::ExpressionSpec(Parser::SimpleNode* pattern) {
    this->type = ExpressionSpecType::PATTERN;
    this->pattern = pattern;
}

ExpressionSpec::~ExpressionSpec() {

}

ExpressionSpecType ExpressionSpec::get_type() {
    return type;
}

Parser::SimpleNode* ExpressionSpec::get_pattern() {
    if (type != ExpressionSpecType::PATTERN) {
        throw "ExpressionSpecType is not PATTERN type";
    }

    return pattern;
}