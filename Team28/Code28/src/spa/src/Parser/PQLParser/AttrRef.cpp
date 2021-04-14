#include "AttrRef.h"

using namespace Parser;

AttrRef::AttrRef() = default;

void AttrRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void AttrRef::set_attr_name(const std::string& new_attr_name) { attr_name = new_attr_name; }

std::string AttrRef::get_synonym() const { return synonym; }

std::string AttrRef::get_attr_name() const { return attr_name; }

bool AttrRef::operator==(const AttrRef& other) const {
    return synonym == other.synonym && attr_name == other.attr_name;
}

std::size_t AttrRefHash::operator()(const AttrRef& attr_ref) const {
    std::hash<std::string> string_hash;
    return string_hash(attr_ref.get_synonym()) ^ string_hash(attr_ref.get_attr_name());
}