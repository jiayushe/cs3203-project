#include "DeclarationMap.h"
#include <stdexcept>

using namespace Parser;

bool DeclarationMap::has(const std::string& synonym) const {
    return declarations.find(synonym) != declarations.end();
}

DesignEntity DeclarationMap::get(const std::string& synonym) const {
    if (!has(synonym)) {
        throw std::runtime_error("Declaration for synonym '" + synonym + "' not found");
    }
    return declarations.at(synonym);
}

void DeclarationMap::add(const std::string& synonym, const DesignEntity& design_entity) {
    if (has(synonym)) {
        throw std::runtime_error("Declaration for synonym '" + synonym + "' has been defined");
    }
    declarations[synonym] = design_entity;
}

int DeclarationMap::size() const { return declarations.size(); }
