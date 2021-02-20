#include "BaseConstraint.h"

using namespace QueryEvaluator;

int BaseConstraint::get_statement_id(const AssignmentMap& assignments,
                                     Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM:
        return assignments.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw "Cannot get statement id for ANY statement ref";
    default:
        throw "Unknown statement ref type";
    }
}

std::string BaseConstraint::get_variable_name(const AssignmentMap& assignments,
                                              Parser::EntityRef& entity_ref) {
    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM:
        return assignments.at(entity_ref.get_synonym()).get_string_value();
    default:
        throw "Unhandled entity ref type";
    }
}

bool BaseConstraint::can_validate(std::unordered_set<std::string>& synonyms) const {
    for (auto const& required_synonym : get_synonyms()) {
        if (synonyms.find(required_synonym) == synonyms.end()) {
            return false;
        }
    }
    return true;
}
