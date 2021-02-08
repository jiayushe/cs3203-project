#include "Parser/PQLParser/DesignEntity.h"
#include <functional>
#include <string>

using namespace Parser;

DesignEntity::DesignEntity() : type(DesignEntityType::INVALID) {}

DesignEntity::DesignEntity(DesignEntityType type, std::string synonym)
    : type(type), synonym(std::move(synonym)) {}

DesignEntityType DesignEntity::get_type() const { return type; }

std::string DesignEntity::get_synonym() const { return synonym; }

bool DesignEntity::operator==(const DesignEntity& other) const {
    return type == other.type && synonym == other.synonym;
}

std::size_t DesignEntityHash::operator()(const DesignEntity& design_entity) const {
    std::size_t h1 = static_cast<std::size_t>(design_entity.get_type());
    std::size_t h2 = std::hash<std::string>{}(design_entity.get_synonym());
    return h1 ^ (h2 << 1);
}
