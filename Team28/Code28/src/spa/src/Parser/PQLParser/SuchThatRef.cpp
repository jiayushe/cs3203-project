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

bool SuchThatRef::operator==(const SuchThatRef& other) const {
    return type == other.type && entity_ref == other.entity_ref &&
           statement_ref == other.statement_ref;
}

std::size_t SuchThatRefHash::operator()(const SuchThatRef& such_that) const {
    std::hash<int> int_hash;
    EntityRefHash entity_ref_hash;
    StatementRefHash statement_ref_hash;
    switch (such_that.get_type()) {
    case SuchThatRefType::INVALID:
        return int_hash(1);
    case SuchThatRefType::ENTITY:
        return int_hash(2) ^ entity_ref_hash(such_that.get_entity_ref());
    case SuchThatRefType::STATEMENT:
        return int_hash(3) ^ statement_ref_hash(such_that.get_statement_ref());
    default:
        throw std::runtime_error("Unknown such that ref type");
    }
}
