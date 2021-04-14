#include "PatternIf.h"
#include <string>

using namespace Parser;

PatternIf::PatternIf() = default;

void PatternIf::set_if_synonym(const std::string& new_if_synonym) { if_synonym = new_if_synonym; }

void PatternIf::set_entity_ref(const EntityRef& new_entity_ref) { entity_ref = new_entity_ref; }

std::string PatternIf::get_if_synonym() const { return if_synonym; }

EntityRef PatternIf::get_entity_ref() const { return entity_ref; }

bool PatternIf::operator==(const PatternIf& other) const {
    return if_synonym == other.if_synonym && entity_ref == other.entity_ref;
}

std::size_t PatternIfHash::operator()(const PatternIf& pattern_if) const {
    std::hash<std::string> string_hash;
    EntityRefHash entity_ref_hash;
    return string_hash(pattern_if.get_if_synonym()) ^ entity_ref_hash(pattern_if.get_entity_ref());
}