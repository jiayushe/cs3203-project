#include "Parser/PQLParser/DesignEntity.h"
#include <string>

using namespace Parser;

DesignEntity::DesignEntity() : type(DesignEntityType::INVALID) {}

DesignEntity::DesignEntity(DesignEntityType type, const std::string &synonym) : type(type), synonym(synonym) {}

DesignEntityType DesignEntity::get_type() { return type; }

std::string DesignEntity::get_synonym() { return synonym; }