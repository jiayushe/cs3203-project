#include "ExpressionSpec.h"
#include <stdexcept>

using namespace Parser;

ExpressionSpec::ExpressionSpec() : type(ExpressionSpecType::INVALID) {}

void ExpressionSpec::set_type(ExpressionSpecType expression_spec_type) {
    this->type = expression_spec_type;
}

void ExpressionSpec::set_pattern(std::shared_ptr<Parser::SimpleNode> new_pattern) {
    pattern = new_pattern;
}

void ExpressionSpec::set_exact_pattern(std::shared_ptr<Parser::SimpleNode> new_pattern) {
    exact_pattern = new_pattern;
}

ExpressionSpecType ExpressionSpec::get_type() const { return type; }

std::shared_ptr<Parser::SimpleNode> ExpressionSpec::get_pattern() const {
    if (type != ExpressionSpecType::PATTERN) {
        throw std::runtime_error("ExpressionSpecType is not PATTERN type");
    }
    return pattern;
}

std::shared_ptr<Parser::SimpleNode> ExpressionSpec::get_exact_pattern() const {
    if (type != ExpressionSpecType::EXACT_PATTERN) {
        throw std::runtime_error("ExpressionSpecType is not EXACT_PATTERN type");
    }
    return exact_pattern;
}

bool ExpressionSpec::operator==(const ExpressionSpec& other) const {
    switch (type) {
    case ExpressionSpecType::INVALID:
        return other.type == ExpressionSpecType::INVALID;
    case ExpressionSpecType::ANY:
        return other.type == ExpressionSpecType::ANY;
    case ExpressionSpecType::PATTERN:
        return other.type == ExpressionSpecType::PATTERN && pattern->is_equal(other.pattern);
    case ExpressionSpecType::EXACT_PATTERN:
        return other.type == ExpressionSpecType::EXACT_PATTERN &&
               exact_pattern->is_equal(other.exact_pattern);
    default:
        throw std::runtime_error("Unknown expression spec type");
    }
}

std::size_t ExpressionSpecHash::operator()(const ExpressionSpec& expression_spec) const {
    return static_cast<std::size_t>(expression_spec.get_type());
}