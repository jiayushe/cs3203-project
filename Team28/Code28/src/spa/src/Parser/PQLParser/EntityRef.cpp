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

bool EntityRef::operator==(const EntityRef& other) const {
    return type == other.type && synonym == other.synonym && name == other.name;
}

std::size_t EntityRefHash::operator()(const EntityRef& entity_ref) const {
    std::hash<int> int_hash;
    std::hash<std::string> string_hash;
    switch (entity_ref.get_type()) {
    case EntityRefType::INVALID:
        return int_hash(1);
    case EntityRefType::ANY:
        return int_hash(2);
    case EntityRefType::NAME:
        return int_hash(3) ^ string_hash(entity_ref.get_name());
    case EntityRefType::SYNONYM:
        return int_hash(4) ^ string_hash(entity_ref.get_synonym());
    default:
        throw std::runtime_error("Unknown entity ref type");
    }
}