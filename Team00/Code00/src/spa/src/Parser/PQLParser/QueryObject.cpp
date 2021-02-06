#include "QueryObject.h"
#include "DesignEntity.h"
#include <iostream>
#include <string>

using namespace Parser;

QueryObject::QueryObject() : has_such_that_cl(false), has_pattern_cl(false) {}

std::unordered_map<std::string, DesignEntity> QueryObject::get_declarations() {
    return declarations;
}

void QueryObject::add_declaration(const std::string& synonym, const DesignEntity& design_entity) {
    if (declarations.find(synonym) != declarations.end()) {
        throw "Synonym has been defined";
    }
    declarations[synonym] = design_entity;
}

std::string QueryObject::get_selection() { return selection; }

bool QueryObject::has_such_that() { return has_such_that_cl; }

bool QueryObject::has_pattern() { return has_pattern_cl; }

SuchThat QueryObject::get_such_that() {
    if (!has_such_that_cl) {
        throw "Query object doesn't have a such that clause";
    }
    return such_that_obj;
}

Pattern QueryObject::get_pattern() {
    if (!has_pattern_cl) {
        throw "Query object doesn't have a pattern clause";
    }
    return pattern_obj;
}

void QueryObject::set_selection(const std::string& new_selection) { selection = new_selection; }

void QueryObject::set_has_such_that(bool new_has_such_that) {
    has_such_that_cl = new_has_such_that;
}

void QueryObject::set_has_pattern(bool new_has_pattern) { has_pattern_cl = new_has_pattern; }

void QueryObject::set_such_that(const SuchThat& new_such_that_cl) {
    such_that_obj = new_such_that_cl;
}

void QueryObject::set_pattern(const Pattern& new_pattern_cl) { pattern_obj = new_pattern_cl; }

void QueryObject::to_string() {
    std::cout << "Declaration size:" << declarations.size() << std::endl;
    std::cout << "Has Such That:" << has_such_that_cl << std::endl;
    std::cout << "Has Pattern:" << has_pattern_cl << std::endl;
    std::cout << "Selection:" << selection << std::endl;
}