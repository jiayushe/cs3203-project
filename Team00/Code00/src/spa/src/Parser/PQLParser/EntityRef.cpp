#include "EntityRef.h"
#include <string>

using namespace Parser;

EntityRef::EntityRef() : type(EntityRefType::INVALID) {}

void EntityRef::set_type(EntityRefType entityRefType) { this->type = entityRefType; }

void EntityRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void EntityRef::set_name(const std::string& new_name) { name = new_name; }

EntityRefType EntityRef::get_type() { return type; }

std::string EntityRef::get_synonym() {
    if (type != EntityRefType::SYNONYM) {
        throw "EntityRefType is not SYNONYM type";
    }

    return synonym;
}

std::string EntityRef::get_name() {
    if (type != EntityRefType::NAME) {
        throw "EntityRefType is not NAME type";
    }

    return name;
}