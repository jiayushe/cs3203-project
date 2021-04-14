#pragma once

#include "EntityRef.h"
#include "ExpressionSpec.h"

namespace Parser {

class PatternAssign {
public:
    std::string get_assign_synonym() const;
    EntityRef get_entity_ref() const;
    ExpressionSpec get_expression_spec() const;

    PatternAssign();

    void set_assign_synonym(const std::string& assigned_synonym);
    void set_entity_ref(const EntityRef& entity_ref);
    void set_expression_spec(const ExpressionSpec& expression_spec);

    bool operator==(const PatternAssign& other) const;

private:
    std::string assign_synonym;
    EntityRef entity_ref;
    ExpressionSpec expression_spec;
};

class PatternAssignHash {
public:
    std::size_t operator()(const PatternAssign& pattern_assign) const;
};

} // namespace Parser
