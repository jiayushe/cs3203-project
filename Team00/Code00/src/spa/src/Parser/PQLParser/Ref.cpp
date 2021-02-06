#include "Ref.h"

using namespace Parser;

Ref::Ref() : type(RefType::INVALID) {}

Ref::Ref(const EntityRef& entity_ref) : type(RefType::ENTITY), entity_ref(entity_ref) {}

Ref::Ref(const StatementRef& statement_ref)
    : type(RefType::STATEMENT), statement_ref(statement_ref) {}

RefType Ref::get_type() { return type; }

EntityRef Ref::get_entity_ref() {
    if (type != RefType::ENTITY) {
        throw "RefType is not ENTITY type";
    }

    return entity_ref;
}

StatementRef Ref::get_statement_ref() {
    if (type != RefType::STATEMENT) {
        throw "RefType is not STATEMENT type";
    }

    return statement_ref;
}