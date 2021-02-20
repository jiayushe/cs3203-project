#pragma once

#include "DesignEntity.h"
#include <memory>
#include <unordered_map>

namespace Parser {

class DeclarationMap {
public:
    int size() const;

    bool has(const std::string& synonym) const;

    DesignEntity get(const std::string& synonym) const;

    void add(const std::string& synonym, const DesignEntity& design_entity);

private:
    std::unordered_map<std::string, DesignEntity> declarations;
};

} // namespace Parser
