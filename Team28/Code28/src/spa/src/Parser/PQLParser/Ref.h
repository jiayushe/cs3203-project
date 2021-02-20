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
    RefType get_type() const;
    EntityRef get_entity_ref() const;
    StatementRef get_statement_ref() const;

    Ref();
    explicit Ref(EntityRef entity_ref);
    explicit Ref(StatementRef statement_ref);

private:
    RefType type;
    EntityRef entity_ref;
    StatementRef statement_ref;
};

} // namespace Parser
