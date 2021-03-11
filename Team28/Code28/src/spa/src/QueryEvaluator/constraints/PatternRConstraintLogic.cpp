#include "PatternRConstraintLogic.h"

using namespace QueryEvaluator;

PatternRConstraintLogic::PatternRConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                 const Parser::PatternAssign& pattern_assign)
    : BaseConstraintLogic(std::move(pkb)), assign_synonym(pattern_assign.get_assign_synonym()),
      expression_spec(pattern_assign.get_expression_spec()) {}

bool PatternRConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {
    switch (expression_spec.get_type()) {
    case Parser::ExpressionSpecType::ANY:
        return true;
    case Parser::ExpressionSpecType::EXACT_PATTERN: {
        auto exact_pattern = expression_spec.get_exact_pattern();
        auto assign_statement = get_statement(assignment_map, assign_synonym);
        return assign_statement->get_pattern()->is_equal(exact_pattern);
    }
    case Parser::ExpressionSpecType::PATTERN: {
        auto pattern = expression_spec.get_pattern();
        auto assign_statement = get_statement(assignment_map, assign_synonym);
        return assign_statement->get_pattern()->is_subtree(pattern);
    }
    default:
        throw std::runtime_error("Unknown expression spec type");
    }
}

std::unordered_set<std::string> PatternRConstraintLogic::get_synonyms() const {
    return {assign_synonym};
}
