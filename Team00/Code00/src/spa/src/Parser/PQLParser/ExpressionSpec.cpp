#include "ExpressionSpec.h"

using namespace Parser;

ExpressionSpec::ExpressionSpec() : type(ExpressionSpecType::INVALID) {}

void ExpressionSpec::set_type(ExpressionSpecType expression_spec_type) {
    this->type = expression_spec_type;
}

void ExpressionSpec::set_pattern(std::shared_ptr<Parser::SimpleNode> new_pattern) { pattern = new_pattern; }

ExpressionSpecType ExpressionSpec::get_type() { return type; }

std::shared_ptr<Parser::SimpleNode> ExpressionSpec::get_pattern() {
    if (type != ExpressionSpecType::PATTERN) {
        throw "ExpressionSpecType is not PATTERN type";
    }

    return pattern;
}