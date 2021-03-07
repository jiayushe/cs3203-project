#include "Elem.h"
#include <stdexcept>

using namespace Parser;

Elem::Elem(std::string synonym) : type(ElemType::SYNONYM), synonym(std::move(synonym)) {}

Elem::Elem(AttrRef attr_ref) : type(ElemType::ATTR_REF), attr_ref(std::move(attr_ref)) {}

ElemType Elem::get_type() const { return type; }

std::string Elem::get_synonym() const {
    if (type != ElemType::SYNONYM) {
        throw std::runtime_error("ElemType is not SYNONYM type");
    }
    return synonym;
}

AttrRef Elem::get_attr_ref() const {
    if (type != ElemType::ATTR_REF) {
        throw std::runtime_error("ElemType is not ATTR_REF type");
    }
    return attr_ref;
}