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
    EntityRefType get_type();
    std::string get_synonym();
    std::string get_name();

    EntityRef();

    void set_type(EntityRefType entityRefType);

    void set_synonym(const std::string &synonym);

    void set_name(const std::string &name);

private:
    EntityRefType type;
    std::string synonym;
    std::string name;
};

} // namespace Parser
