#pragma once

#include "EntityRef.h"
#include "StatementRef.h"

namespace Parser {
enum class RefType {
    INVALID,
    ENTITY,
    STATEMENT,
};

class Ref {
public:
    RefType get_type();
    EntityRef get_entity_ref();
    StatementRef get_statement_ref();

    Ref();

    explicit Ref(const EntityRef &entity_ref);

    explicit Ref(const StatementRef &statement_ref);

private:
    RefType type;
    EntityRef entity_ref;
    StatementRef statement_ref;
};

} // namespace Parser
