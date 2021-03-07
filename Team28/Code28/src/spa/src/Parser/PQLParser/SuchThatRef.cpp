#include "SuchThatRef.h"
#include <stdexcept>

using namespace Parser;

SuchThatRef::SuchThatRef() : type(SuchThatRefType::INVALID) {}

SuchThatRef::SuchThatRef(EntityRef entity_ref)
    : type(SuchThatRefType::ENTITY), entity_ref(std::move(entity_ref)) {}

SuchThatRef::SuchThatRef(StatementRef statement_ref)
    : type(SuchThatRefType::STATEMENT), statement_ref(std::move(statement_ref)) {}

SuchThatRefType SuchThatRef::get_type() const { return type; }

EntityRef SuchThatRef::get_entity_ref() const {
    if (type != SuchThatRefType::ENTITY) {
        throw std::runtime_error("RefType is not ENTITY type");
    }
    return entity_ref;
}

StatementRef SuchThatRef::get_statement_ref() const {
    if (type != SuchThatRefType::STATEMENT) {
        throw std::runtime_error("RefType is not STATEMENT type");
    }
    return statement_ref;
}