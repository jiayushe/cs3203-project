#include "Parser/PQLParser/DesignEntity.h"

using namespace Parser;

DesignEntity::DesignEntity() : type(DesignEntityType::INVALID) {}

DesignEntity::DesignEntity(DesignEntityType type, std::string synonym)
    : type(type), synonym(std::move(synonym)) {}

DesignEntityType DesignEntity::get_type() const { return type; }

std::string DesignEntity::get_synonym() const { return synonym; }
