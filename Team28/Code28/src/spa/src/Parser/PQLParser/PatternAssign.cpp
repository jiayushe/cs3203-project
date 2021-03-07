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