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

    bool operator==(const PatternWhile& other) const;

private:
    std::string while_synonym;
    EntityRef entity_ref;
};

class PatternWhileHash {
public:
    std::size_t operator()(const PatternWhile& pattern_while) const;
};

} // namespace Parser
