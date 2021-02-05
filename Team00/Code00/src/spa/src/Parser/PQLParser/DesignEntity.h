#pragma once

#include <string>

namespace Parser {
enum class DesignEntityType {
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

    DesignEntity(DesignEntityType type, std::string synonym);

    ~DesignEntity();

    DesignEntityType get_type();
    std::string get_synonym();

private:
    DesignEntityType type;
    std::string synonym;
};

} // namespace Parser
