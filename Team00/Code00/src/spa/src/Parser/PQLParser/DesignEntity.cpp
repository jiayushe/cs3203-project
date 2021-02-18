#include "Parser/PQLParser/DesignEntity.h"

using namespace Parser;

DesignEntity::DesignEntity() : type(DesignEntityType::INVALID) {}

DesignEntity::DesignEntity(DesignEntityType type, std::string synonym)
    : type(type), synonym(std::move(synonym)) {}

DesignEntityType DesignEntity::get_type() const { return type; }

std::string DesignEntity::get_synonym() const { return synonym; }

std::string Parser::to_string(DesignEntityType entity_type) {
    switch (entity_type) {
    case DesignEntityType::INVALID:
        return "INVALID";
    case DesignEntityType::STMT:
        return "STMT";
    case DesignEntityType::READ:
        return "READ";
    case DesignEntityType::PRINT:
        return "PRINT";
    case DesignEntityType::WHILE:
        return "WHILE";
    case DesignEntityType::IF:
        return "IF";
    case DesignEntityType::ASSIGN:
        return "ASSIGN";
    case DesignEntityType::VARIABLE:
        return "VARIABLE";
    case DesignEntityType::CONSTANT:
        return "CONSTANT";
    case DesignEntityType::PROCEDURE:
        return "PROCEDURE";
    default:
        throw "Unknown token type";
    }
}
