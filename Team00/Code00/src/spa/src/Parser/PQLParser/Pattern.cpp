#include "Pattern.h"
#include <string>

using namespace Parser;

Pattern::Pattern() = default;

void Pattern::set_assigned_synonym(const std::string& new_assigned_synonym) {
    assigned_synonym = new_assigned_synonym;
}

void Pattern::set_entity_ref(const EntityRef& new_entity_ref) { entity_ref = new_entity_ref; }

void Pattern::set_expression_spec(const ExpressionSpec& new_expression_spec) {
    expression_spec = new_expression_spec;
}

std::string Pattern::get_assigned_synonym() const { return assigned_synonym; }

EntityRef Pattern::get_entity_ref() const { return entity_ref; }

ExpressionSpec Pattern::get_expression_spec() const { return expression_spec; }