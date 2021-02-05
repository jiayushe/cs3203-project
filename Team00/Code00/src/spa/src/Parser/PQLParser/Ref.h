#pragma once

#include "EntityRef.h"
#include "StatementRef.h"

namespace Parser {
enum class RefType {
    ENTITY,
    STATEMENT,
};

class Ref {
public:
    RefType get_type();
    EntityRef get_entity_ref();
    StatementRef get_statement_ref();

    Ref();

    Ref(EntityRef entity_ref);

    Ref(StatementRef statement_ref);

    ~Ref();

private:
    RefType type;
    EntityRef entity_ref;
    StatementRef statement_ref;
};

} // namespace Parser
