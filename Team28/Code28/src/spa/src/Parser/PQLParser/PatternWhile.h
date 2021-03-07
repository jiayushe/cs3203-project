#pragma once

#include "EntityRef.h"
#include <string>

namespace Parser {
class PatternWhile {
public:
    std::string get_while_synonym() const;
    EntityRef get_entity_ref() const;

    PatternWhile();

    void set_while_synonym(const std::string& while_synonym);
    void set_entity_ref(const EntityRef& entity_ref);

private:
    std::string while_synonym;
    EntityRef entity_ref;
};

} // namespace Parser
