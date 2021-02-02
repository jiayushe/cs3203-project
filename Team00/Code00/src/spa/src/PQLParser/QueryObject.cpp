#include <string>
#include <iostream>
#include "QueryObject.h"
#include "DesignEntity.h"

using namespace PQLParser;

QueryObject::QueryObject() :
    has_such_that_cl(false),
    has_pattern_cl(false) {}

QueryObject::~QueryObject() { }

std::unordered_map<std::string, DesignEntity> QueryObject::get_declarations() {
    return declarations;
}

void QueryObject::add_declaration(std::string synonym, DesignEntity design_entity) {
    if (declarations.find(synonym) != declarations.end()) {
        throw "Synonym has been defined";
    }
    declarations[synonym] = design_entity;
}


std::string QueryObject::get_selection() {
    return selection;
}

bool QueryObject::has_such_that() {
    return has_such_that_cl;
}

bool QueryObject::has_pattern() {
    return has_pattern_cl;
}

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


void QueryObject::set_selection(std::string selection) {
    this->selection = selection;
}

void QueryObject::set_has_such_that(bool has_such_that) {
    this->has_such_that_cl = has_such_that;
}

void QueryObject::set_has_pattern(bool has_pattern) {
    this->has_pattern_cl = has_pattern;
}

void QueryObject::set_such_that(SuchThat such_that_cl) {
    this->such_that_obj = such_that_cl;
}

void QueryObject::set_pattern(Pattern pattern_cl) {
    this->pattern_obj = pattern_cl;
}

void QueryObject::to_string() {
    std::cout << "Declaration size:" << declarations.size() << std::endl;
    std::cout << "Has Such That:" << has_such_that_cl << std::endl;
    std::cout << "Has Pattern:" << has_pattern_cl << std::endl;
    std::cout << "Selection:" << selection << std::endl;
}