#include "AttrRef.h"

using namespace Parser;

AttrRef::AttrRef() = default;

void AttrRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void AttrRef::set_attr_name(const std::string& new_attr_name) { attr_name = new_attr_name; }

std::string AttrRef::get_synonym() const { return synonym; }

std::string AttrRef::get_attr_name() const { return attr_name; }
