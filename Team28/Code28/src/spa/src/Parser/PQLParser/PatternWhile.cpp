#include "PatternWhile.h"

using namespace Parser;

PatternWhile::PatternWhile() = default;

void PatternWhile::set_while_synonym(const std::string& new_while_synonym) {
    while_synonym = new_while_synonym;
}

void PatternWhile::set_entity_ref(const EntityRef& new_entity_ref) { entity_ref = new_entity_ref; }

std::string PatternWhile::get_while_synonym() const { return while_synonym; }

EntityRef PatternWhile::get_entity_ref() const { return entity_ref; }