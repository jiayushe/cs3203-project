#pragma once

#include <string>

namespace Parser {
enum class DesignEntityType {
    INVALID,
    STMT,
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
    VARIABLE,
    CONSTANT,
    PROCEDURE,
};

class DesignEntity {
public:
    DesignEntity();

    DesignEntity(DesignEntityType type, const std::string& synonym);

    DesignEntityType get_type();
    std::string get_synonym();

private:
    DesignEntityType type;
    std::string synonym;
};

} // namespace Parser
