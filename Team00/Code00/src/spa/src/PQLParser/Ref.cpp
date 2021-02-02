#include <string>
#include "Ref.h"

using namespace PQLParser;

Ref::Ref() {}

Ref::Ref(EntityRef entity_ref) : entity_ref(entity_ref) {
    this->type = RefType::ENTITY;
}

Ref::Ref(StatementRef statement_ref) : statement_ref(statement_ref) {
    this->type = RefType::STATEMENT;
}

Ref::~Ref() {

}

RefType Ref::get_type() {
    return type;
}

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