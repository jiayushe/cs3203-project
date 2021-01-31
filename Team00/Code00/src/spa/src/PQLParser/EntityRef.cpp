//
// Created by Victor Varian on 27/1/21.
//

//
// Created by Victor Varian on 27/1/21.
//

#include <string>
#include "EntityRef.h"

using namespace PQLParser;

EntityRef::EntityRef() {}

//EntityRef::EntityRef(EntityRefType type, std::string text) {
//    if (type == EntityRefType::SYNONYM) {
//        this->synonym = text;
//        this->type = type;
//    } else if (type == EntityRefType::NAME) {
//        this->name = text;
//        this->type = type;
//    } else {
//        throw 'Invalid EntityRefType passed';
//    }
//}

EntityRef::~EntityRef() {

}

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
        throw 'EntityRefType is not SYNONYM type';
    }

    return synonym;
}

std::string EntityRef::get_name() {
    if (type != EntityRefType::NAME) {
        throw 'EntityRefType is not NAME type';
    }

    return name;
}