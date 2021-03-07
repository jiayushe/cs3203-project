#include "PatternConstraint.h"

using namespace QueryEvaluator;

PatternConstraint::PatternConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                     const Parser::Pattern& pattern)
    : pkb(std::move(pkb)), pattern(pattern) {
    // FIXME: Handle all types of patterns
    auto pattern_assign = pattern.get_pattern_assign();
    synonyms.insert(pattern_assign.get_assign_synonym());
    auto entity_ref = pattern_assign.get_entity_ref();
    if (entity_ref.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(entity_ref.get_synonym());
    }
}

bool PatternConstraint::is_valid(const AssignmentMap& assignments) {
    // FIXME: Handle all types of patterns
    auto pattern_assign = pattern.get_pattern_assign();

    auto lhs = pattern_assign.get_entity_ref();
    auto rhs = pattern_assign.get_expression_spec();
    auto syn = pattern_assign.get_assign_synonym();

    auto statement_id = assignments.at(syn).get_int_value();
    auto assigned_statement = pkb->get_statement_by_id(statement_id);

    if (assigned_statement->get_type() != KnowledgeBase::StatementType::ASSIGN) {
        return false;
    }

    if (lhs.get_type() == Parser::EntityRefType::ANY) {
        switch (rhs.get_type()) {
        case Parser::ExpressionSpecType::ANY:
            return true;
        case Parser::ExpressionSpecType::PATTERN:
            return assigned_statement->get_pattern()->is_subtree(rhs.get_pattern());
        default:
            throw std::runtime_error("Unhandled expression spec type");
        }
    }

    auto lhs_name = get_variable_name(assignments, lhs);
    auto assigned_statement_modifies = assigned_statement->get_modifies();
    if (assigned_statement_modifies.find(lhs_name) == assigned_statement_modifies.end()) {
        return false;
    }
    switch (rhs.get_type()) {
    case Parser::ExpressionSpecType::ANY:
        return true;
    case Parser::ExpressionSpecType::PATTERN:
        return assigned_statement->get_pattern()->is_subtree(rhs.get_pattern());
    default:
        throw std::runtime_error("Unhandled expression spec type");
    }
}

std::unordered_set<std::string> PatternConstraint::get_synonyms() const { return synonyms; }
