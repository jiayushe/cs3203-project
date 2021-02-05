#include "Parser/PQLParser/DesignEntity.h"
#include <string>

using namespace Parser;

DesignEntity::DesignEntity() {}

DesignEntity::DesignEntity(DesignEntityType type, std::string synonym)
    : type(type), synonym(synonym) {}

DesignEntity::~DesignEntity() {}

DesignEntityType DesignEntity::get_type() { return type; }

std::string DesignEntity::get_synonym() { return synonym; }