#include "PatternIf.h"
#include <string>

using namespace Parser;

PatternIf::PatternIf() = default;

void PatternIf::set_if_synonym(const std::string& new_if_synonym) { if_synonym = new_if_synonym; }

void PatternIf::set_entity_ref(const EntityRef& new_entity_ref) { entity_ref = new_entity_ref; }

std::string PatternIf::get_if_synonym() const { return if_synonym; }

EntityRef PatternIf::get_entity_ref() const { return entity_ref; }