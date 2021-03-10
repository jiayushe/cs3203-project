#include "BaseConstraint.h"
#include <stdexcept>

using namespace QueryEvaluator;

int BaseConstraint::get_statement_id(const AssignmentMap& assignments,
                                     Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM: // + integer attribute
        return assignments.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw std::runtime_error("Cannot get statement id for ANY statement ref");
    default:
        throw std::runtime_error("Unknown statement ref type");
    }
}

std::string BaseConstraint::get_variable_name(const AssignmentMap& assignments,
                                              Parser::EntityRef& entity_ref) {
    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM: // + name attribute
        return assignments.at(entity_ref.get_synonym()).get_string_value();
    default:
        throw std::runtime_error("Unhandled entity ref type");
    }
}

int BaseConstraint::get_statement_id(const AssignmentMap& assignments, Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_int_value();
}

std::string BaseConstraint::get_variable_name(const AssignmentMap& assignments,
                                              Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_string_value();
}

std::string BaseConstraint::get_procedure_name(const AssignmentMap& assignments,
                                               Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_string_value();
}

int BaseConstraint::get_constant_value(const AssignmentMap& assignments,
                                       Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_int_value();
}

bool BaseConstraint::can_validate(std::unordered_set<std::string>& synonyms) const {
    for (auto const& required_synonym : get_synonyms()) {
        if (synonyms.find(required_synonym) == synonyms.end()) {
            return false;
        }
    }
    return true;
}
