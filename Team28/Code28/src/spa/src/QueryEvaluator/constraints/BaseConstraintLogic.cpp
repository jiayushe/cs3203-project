#include "BaseConstraintLogic.h"

using namespace QueryEvaluator;

int BaseConstraintLogic::get_statement_id(const AssignmentMap& assignments,
                                          const Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM:
        return assignments.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw std::runtime_error("Cannot get statement id for ANY statement ref");
    default:
        throw std::runtime_error("Unknown statement ref type");
    }
}

std::string BaseConstraintLogic::get_variable_name(const AssignmentMap& assignments,
                                                   const Parser::EntityRef& entity_ref) {
    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM:
        return assignments.at(entity_ref.get_synonym()).get_string_value();
    case Parser::EntityRefType::ANY:
        throw std::runtime_error("Cannot get variable name for ANY entity ref");
    default:
        throw std::runtime_error("Unhandled entity ref type");
    }
}

std::string BaseConstraintLogic::get_procedure_name(const AssignmentMap& assignments,
                                                    const Parser::EntityRef& entity_ref) {
    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM:
        return assignments.at(entity_ref.get_synonym()).get_string_value();
    case Parser::EntityRefType::ANY:
        throw std::runtime_error("Cannot get procedure name for ANY entity ref");
    default:
        throw std::runtime_error("Unhandled entity ref type");
    }
}

int BaseConstraintLogic::get_statement_id(const AssignmentMap& assignments,
                                          const Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_int_value();
}

std::string BaseConstraintLogic::get_variable_name(const AssignmentMap& assignments,
                                                   const Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_string_value();
}

std::string BaseConstraintLogic::get_procedure_name(const AssignmentMap& assignments,
                                                    const Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_string_value();
}

int BaseConstraintLogic::get_constant_value(const AssignmentMap& assignments,
                                            const Parser::AttrRef& attr_ref) {
    return assignments.at(attr_ref.get_synonym()).get_int_value();
}
