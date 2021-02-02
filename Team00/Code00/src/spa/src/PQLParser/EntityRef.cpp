#include <string>
#include "EntityRef.h"

using namespace PQLParser;

EntityRef::EntityRef() {}

EntityRef::~EntityRef() {}

void EntityRef::set_type(EntityRefType entityRefType) {
    this->type = entityRefType;
}

void EntityRef::set_synonym(std::string synonym) {
    this->synonym = synonym;
}

void EntityRef::set_name(std::string name) {
    this->name = name;
}

EntityRefType EntityRef::get_type() {
    return type;
}

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