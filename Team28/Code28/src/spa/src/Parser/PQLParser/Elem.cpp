#include "Elem.h"
#include <stdexcept>

using namespace Parser;

Elem::Elem() : type(ElemType::INVALID) {}

void Elem::set_type(ElemType result_type) { this->type = result_type; }

void Elem::set_synonym(std::string synonym) { this->synonym = synonym; }

void Elem::set_attr_ref(AttrRef attr_ref) { this->attr_ref = attr_ref; }

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