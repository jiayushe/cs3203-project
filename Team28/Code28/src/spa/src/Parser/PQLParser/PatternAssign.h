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

private:
    std::string assign_synonym;
    EntityRef entity_ref;
    ExpressionSpec expression_spec;
};

} // namespace Parser
