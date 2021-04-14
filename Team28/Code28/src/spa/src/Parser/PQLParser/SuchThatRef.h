#pragma once

#include "EntityRef.h"
#include "StatementRef.h"

namespace Parser {

enum class SuchThatRefType {
    INVALID,
    ENTITY,
    STATEMENT,
};

class SuchThatRef {
public:
    SuchThatRefType get_type() const;
    EntityRef get_entity_ref() const;
    StatementRef get_statement_ref() const;

    SuchThatRef();
    explicit SuchThatRef(EntityRef entity_ref);
    explicit SuchThatRef(StatementRef statement_ref);

    bool operator==(const SuchThatRef& other) const;

private:
    SuchThatRefType type;
    EntityRef entity_ref;
    StatementRef statement_ref;
};

class SuchThatRefHash {
public:
    std::size_t operator()(const SuchThatRef& such_that) const;
};

} // namespace Parser
