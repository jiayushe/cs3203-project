#include <string>
#include "DesignEntity.h"

using namespace PQLParser;

DesignEntity::DesignEntity() {}

DesignEntity::DesignEntity(DesignEntityType type, std::string synonym) :
    type(type),
    synonym(synonym) {}

DesignEntity::~DesignEntity() {}

DesignEntityType DesignEntity::get_type() {
    return type;
}

std::string DesignEntity::get_synonym() {
    return synonym;
}