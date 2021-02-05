#include "Pattern.h"
#include <string>

using namespace Parser;

Pattern::Pattern() {}

Pattern::~Pattern() {}

void Pattern::set_assigned_synonym(std::string assigned_synonym) {
    this->synonym = assigned_synonym;
}

void Pattern::set_entity_ref(EntityRef entity_ref) { this->entity_ref = entity_ref; }

void Pattern::set_expression_spec(ExpressionSpec expression_spec) {
    this->expression_spec = expression_spec;
}

std::string Pattern::get_assigned_synonym() { return synonym; }

EntityRef Pattern::get_entity_ref() { return entity_ref; }

ExpressionSpec Pattern::get_expression_spec() { return expression_spec; }