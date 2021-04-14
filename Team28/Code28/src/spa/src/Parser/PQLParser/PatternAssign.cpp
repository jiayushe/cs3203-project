#include "PatternAssign.h"
#include <string>

using namespace Parser;

PatternAssign::PatternAssign() = default;

void PatternAssign::set_assign_synonym(const std::string& new_assign_synonym) {
    assign_synonym = new_assign_synonym;
}

void PatternAssign::set_entity_ref(const EntityRef& new_entity_ref) { entity_ref = new_entity_ref; }

void PatternAssign::set_expression_spec(const ExpressionSpec& new_expression_spec) {
    expression_spec = new_expression_spec;
}

std::string PatternAssign::get_assign_synonym() const { return assign_synonym; }

EntityRef PatternAssign::get_entity_ref() const { return entity_ref; }

ExpressionSpec PatternAssign::get_expression_spec() const { return expression_spec; }

bool PatternAssign::operator==(const PatternAssign& other) const {
    return assign_synonym == other.assign_synonym && entity_ref == other.entity_ref &&
           expression_spec == other.expression_spec;
}

std::size_t PatternAssignHash::operator()(const PatternAssign& pattern_assign) const {
    std::hash<std::string> string_hash;
    EntityRefHash entity_ref_hash;
    ExpressionSpecHash expression_spec_hash;
    return string_hash(pattern_assign.get_assign_synonym()) ^
           entity_ref_hash(pattern_assign.get_entity_ref()) ^
           expression_spec_hash(pattern_assign.get_expression_spec());
}