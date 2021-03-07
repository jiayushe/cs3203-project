#pragma once

#include "EntityRef.h"
#include <string>

namespace Parser {
class PatternIf {
public:
    std::string get_if_synonym() const;
    EntityRef get_entity_ref() const;

    PatternIf();

    void set_if_synonym(const std::string& if_synonym);
    void set_entity_ref(const EntityRef& entity_ref);

private:
    std::string if_synonym;
    EntityRef entity_ref;
};

} // namespace Parser
