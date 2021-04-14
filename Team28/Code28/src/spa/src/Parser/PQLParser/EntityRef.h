#pragma once

#include <string>

namespace Parser {
enum class EntityRefType {
    INVALID,
    ANY,
    SYNONYM,
    NAME,
};

class EntityRef {
public:
    EntityRefType get_type() const;
    std::string get_synonym() const;
    std::string get_name() const;

    EntityRef();

    void set_type(EntityRefType entityRefType);
    void set_synonym(const std::string& synonym);
    void set_name(const std::string& name);

    bool operator==(const EntityRef& other) const;

private:
    EntityRefType type;
    std::string synonym;
    std::string name;
};

class EntityRefHash {
public:
    std::size_t operator()(const EntityRef& entity_ref) const;
};

} // namespace Parser
