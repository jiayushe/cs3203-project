#include "QueryObject.h"
#include <stdexcept>
#include <string>

using namespace Parser;

QueryObject::QueryObject() = default;

DeclarationMap QueryObject::get_declarations() const { return declarations; }

Result QueryObject::get_result() const { return result; }

std::vector<SuchThat> QueryObject::get_all_such_that() const { return all_such_that; }

std::vector<Pattern> QueryObject::get_all_pattern() const { return all_pattern; }

std::vector<With> QueryObject::get_all_with() const { return all_with; }

void QueryObject::add_declaration(const std::string& synonym, const DesignEntity& design_entity) {
    declarations.add(synonym, design_entity);
}

void QueryObject::set_result(const Result& new_result) { result = new_result; }

void QueryObject::add_such_that(const SuchThat& new_such_that) {
    all_such_that.push_back(new_such_that);
}

void QueryObject::add_pattern(const Pattern& new_pattern) { all_pattern.push_back(new_pattern); }

void QueryObject::add_with(const With& new_with) { all_with.push_back(new_with); }

std::string QueryObject::to_string() const {
    std::string output;
    output += "QueryObject(declaration.size=" + std::to_string(declarations.size());
    output +=
        ", has_such_that=" + (all_such_that.empty() ? std::string("TRUE") : std::string("FALSE"));
    output +=
        ", has_pattern= " + (all_pattern.empty() ? std::string("TRUE") : std::string("FALSE"));
    output += ", has_with= " + (all_with.empty() ? std::string("TRUE") : std::string("FALSE"));
    return output;
}