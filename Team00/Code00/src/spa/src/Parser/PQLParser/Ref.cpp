#include "Ref.h"

using namespace Parser;

Ref::Ref() : type(RefType::INVALID) {}

Ref::Ref(EntityRef entity_ref) : type(RefType::ENTITY), entity_ref(std::move(entity_ref)) {}

Ref::Ref(StatementRef statement_ref)
    : type(RefType::STATEMENT), statement_ref(std::move(statement_ref)) {}

RefType Ref::get_type() const { return type; }

EntityRef Ref::get_entity_ref() const {
    if (type != RefType::ENTITY) {
        throw "RefType is not ENTITY type";
    }
    return entity_ref;
}

StatementRef Ref::get_statement_ref() const {
    if (type != RefType::STATEMENT) {
        throw "RefType is not STATEMENT type";
    }
    return statement_ref;
}