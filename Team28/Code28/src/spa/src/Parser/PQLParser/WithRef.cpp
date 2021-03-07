#include "With.h"
#include <stdexcept>

using namespace Parser;

WithRef::WithRef() : type(WithRefType::INVALID), statement_number(-1) {}

void WithRef::set_type(WithRefType new_type) { type = new_type; }

void WithRef::set_name(const std::string& new_name) { name = new_name; }

void WithRef::set_statement_number(int new_statement_number) {
    statement_number = new_statement_number;
}

void WithRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void WithRef::set_attr_ref(const AttrRef& new_attr_ref) { attr_ref = new_attr_ref; }

WithRefType WithRef::get_type() const { return type; }

std::string WithRef::get_name() const {
    if (type != WithRefType::NAME) {
        throw std::runtime_error("WithRefType is not NAME type");
    }
    return name;
}

int WithRef::get_statement_number() const {
    if (type != WithRefType::STATEMENT_NUMBER) {
        throw std::runtime_error("WithRefType is not STATEMENT_NUMBER type");
    }
    return statement_number;
}

std::string WithRef::get_synonym() const {
    if (type != WithRefType::SYNONYM) {
        throw std::runtime_error("WithRefType is not SYNONYM type");
    }
    return synonym;
}

AttrRef WithRef::get_attr_ref() const {
    if (type != WithRefType::ATTR_REF) {
        throw std::runtime_error("WithRefType is not ATTR_REF type");
    }
    return attr_ref;
}