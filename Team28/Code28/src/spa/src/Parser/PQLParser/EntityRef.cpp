#include "EntityRef.h"
#include <stdexcept>
#include <string>

using namespace Parser;

EntityRef::EntityRef() : type(EntityRefType::INVALID) {}

void EntityRef::set_type(EntityRefType entity_ref_type) { this->type = entity_ref_type; }

void EntityRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void EntityRef::set_name(const std::string& new_name) { name = new_name; }

EntityRefType EntityRef::get_type() const { return type; }

std::string EntityRef::get_synonym() const {
    if (type != EntityRefType::SYNONYM) {
        throw std::runtime_error("EntityRefType is not SYNONYM type");
    }
    return synonym;
}

std::string EntityRef::get_name() const {
    if (type != EntityRefType::NAME) {
        throw std::runtime_error("EntityRefType is not NAME type");
    }
    return name;
}