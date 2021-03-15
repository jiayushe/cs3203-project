#include "WithConstraintLogic.h"

using namespace QueryEvaluator;

WithConstraintLogic::WithConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                         const Parser::WithRef& lhs, const Parser::WithRef& rhs)
    : BaseConstraintLogic(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::WithRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    } else if (lhs.get_type() == Parser::WithRefType::ATTR_REF) {
        synonyms.insert(lhs.get_attr_ref().get_synonym());
    }

    if (rhs.get_type() == Parser::WithRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    } else if (rhs.get_type() == Parser::WithRefType::ATTR_REF) {
        synonyms.insert(rhs.get_attr_ref().get_synonym());
    }
}

bool WithConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {
    if (is_string_value(lhs) && is_string_value(rhs)) {
        return get_string_value(assignment_map, lhs) == get_string_value(assignment_map, rhs);
    } else if (is_integer_value(lhs) && is_integer_value(rhs)) {
        return get_integer_value(assignment_map, lhs) == get_integer_value(assignment_map, rhs);
    } else {
        return false;
    }
}

bool WithConstraintLogic::is_string_value(const Parser::WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case Parser::WithRefType::NAME:
        return true;
    case Parser::WithRefType::ATTR_REF: {
        auto attr_name = with_ref.get_attr_ref().get_attr_name();
        return attr_name == "procName" || attr_name == "varName";
    }
    case Parser::WithRefType::STATEMENT_ID:
    case Parser::WithRefType::SYNONYM:
        return false;
    default:
        throw std::runtime_error("Unhandled with ref type");
    }
}

bool WithConstraintLogic::is_integer_value(const Parser::WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case Parser::WithRefType::STATEMENT_ID:
    case Parser::WithRefType::SYNONYM:
        return true;
    case Parser::WithRefType::ATTR_REF: {
        auto attr_name = with_ref.get_attr_ref().get_attr_name();
        return attr_name == "stmt#" || attr_name == "value";
    }
    case Parser::WithRefType::NAME:
        return false;
    default:
        throw std::runtime_error("Unhandled with ref type");
    }
}

std::string WithConstraintLogic::get_string_value(const AssignmentMap& assignment_map,
                                                  const Parser::WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case Parser::WithRefType::NAME:
        return with_ref.get_name();
    case Parser::WithRefType::ATTR_REF:
        return assignment_map.at(with_ref.get_attr_ref().get_synonym()).get_string_value();
    default:
        throw std::runtime_error("Cannot get string value");
    }
}

int WithConstraintLogic::get_integer_value(const AssignmentMap& assignment_map,
                                           const Parser::WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case Parser::WithRefType::STATEMENT_ID:
        return with_ref.get_statement_id();
    case Parser::WithRefType::ATTR_REF:
        return assignment_map.at(with_ref.get_attr_ref().get_synonym()).get_int_value();
    case Parser::WithRefType::SYNONYM:
        return assignment_map.at(with_ref.get_synonym()).get_int_value();
    default:
        throw std::runtime_error("Cannot get int value");
    }
}

std::unordered_set<std::string> WithConstraintLogic::get_synonyms() const { return synonyms; }
