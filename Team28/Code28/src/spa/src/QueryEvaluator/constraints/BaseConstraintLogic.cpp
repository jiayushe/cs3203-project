#include "BaseConstraintLogic.h"

using namespace QueryEvaluator;

BaseConstraintLogic::BaseConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb)
    : pkb(std::move(pkb)) {}

std::shared_ptr<KnowledgeBase::Statement>
BaseConstraintLogic::get_statement(const AssignmentMap& assignment_map,
                                   const std::string& synonym) const {
    auto statement_id = assignment_map.at(synonym).get_int_value();
    return pkb->get_statement_by_id(statement_id);
}

std::shared_ptr<KnowledgeBase::Statement>
BaseConstraintLogic::get_statement(const AssignmentMap& assignment_map,
                                   const Parser::StatementRef& statement_ref) const {
    auto statement_id = get_statement_id(assignment_map, statement_ref);
    return pkb->get_statement_by_id(statement_id);
}

std::shared_ptr<KnowledgeBase::Variable>
BaseConstraintLogic::get_variable(const AssignmentMap& assignment_map,
                                  const std::string& synonym) const {
    auto variable_name = assignment_map.at(synonym).get_string_value();
    return pkb->get_variable_by_name(variable_name);
}

std::shared_ptr<KnowledgeBase::Variable>
BaseConstraintLogic::get_variable(const AssignmentMap& assignment_map,
                                  const Parser::EntityRef& entity_ref) const {
    auto variable_name = get_variable_name(assignment_map, entity_ref);
    return pkb->get_variable_by_name(variable_name);
}

std::shared_ptr<KnowledgeBase::Procedure>
BaseConstraintLogic::get_procedure(const AssignmentMap& assignment_map,
                                   const std::string& synonym) const {
    auto procedure_name = assignment_map.at(synonym).get_string_value();
    return pkb->get_procedure_by_name(procedure_name);
}

std::shared_ptr<KnowledgeBase::Procedure>
BaseConstraintLogic::get_procedure(const AssignmentMap& assignment_map,
                                   const Parser::EntityRef& entity_ref) const {
    auto procedure_name = get_procedure_name(assignment_map, entity_ref);
    return pkb->get_procedure_by_name(procedure_name);
}

int BaseConstraintLogic::get_constant_value(const AssignmentMap& assignment_map,
                                            const Parser::AttrRef& attr_ref) {
    return assignment_map.at(attr_ref.get_synonym()).get_int_value();
}

int BaseConstraintLogic::get_statement_id(const AssignmentMap& assignment_map,
                                          const Parser::AttrRef& attr_ref) {
    return assignment_map.at(attr_ref.get_synonym()).get_int_value();
}

int BaseConstraintLogic::get_statement_id(const AssignmentMap& assignment_map,
                                          const Parser::StatementRef& statement_ref) {

    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM:
        return assignment_map.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw std::runtime_error("Cannot get statement id for ANY statement ref");
    default:
        throw std::runtime_error("Unknown statement ref type");
    }
}

std::string BaseConstraintLogic::get_variable_name(const AssignmentMap& assignment_map,
                                                   const Parser::AttrRef& attr_ref) {
    return assignment_map.at(attr_ref.get_synonym()).get_string_value();
}

std::string BaseConstraintLogic::get_variable_name(const AssignmentMap& assignment_map,
                                                   const Parser::EntityRef& entity_ref) {

    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM:
        return assignment_map.at(entity_ref.get_synonym()).get_string_value();
    case Parser::EntityRefType::ANY:
        throw std::runtime_error("Cannot get variable name for ANY entity ref");
    default:
        throw std::runtime_error("Unhandled entity ref type");
    }
}

std::string BaseConstraintLogic::get_procedure_name(const AssignmentMap& assignment_map,
                                                    const Parser::AttrRef& attr_ref) {
    return assignment_map.at(attr_ref.get_synonym()).get_string_value();
}

std::string BaseConstraintLogic::get_procedure_name(const AssignmentMap& assignment_map,
                                                    const Parser::EntityRef& entity_ref) {

    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::NAME:
        return entity_ref.get_name();
    case Parser::EntityRefType::SYNONYM:
        return assignment_map.at(entity_ref.get_synonym()).get_string_value();
    case Parser::EntityRefType::ANY:
        throw std::runtime_error("Cannot get procedure name for ANY entity ref");
    default:
        throw std::runtime_error("Unhandled entity ref type");
    }
}
