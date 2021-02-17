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

    DesignEntity(DesignEntityType type, std::string synonym);

    DesignEntityType get_type() const;
    std::string get_synonym() const;

private:
    DesignEntityType type;
    std::string synonym;
};

} // namespace Parser
