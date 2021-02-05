#include "ExpressionSpec.h"

using namespace Parser;

ExpressionSpec::ExpressionSpec() {
    this->type = ExpressionSpecType::ANY;
}

ExpressionSpec::~ExpressionSpec() {
    delete pattern;
}

void ExpressionSpec::set_type(ExpressionSpecType expression_spec_type) {
    this->type = expression_spec_type;
}


void ExpressionSpec::set_pattern(Parser::SimpleNode* pattern) {
    this->pattern = pattern;
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