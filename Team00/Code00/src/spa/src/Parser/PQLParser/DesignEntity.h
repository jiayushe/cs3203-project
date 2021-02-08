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

    bool operator==(const DesignEntity& other) const;

private:
    DesignEntityType type;
    std::string synonym;
};

class DesignEntityHash {
public:
    std::size_t operator()(const DesignEntity& design_entity) const;
};

} // namespace Parser
