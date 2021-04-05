#include "UsesPConstraintLogic.h"
#include <stdexcept>

using namespace QueryEvaluator;

UsesPConstraintLogic::UsesPConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                           const Parser::EntityRef& lhs,
                                           const Parser::EntityRef& rhs)
    : BaseConstraintLogic(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (rhs.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool UsesPConstraintLogic::is_valid(const AssignmentMap& assignments) const {

    if (lhs.get_type() == Parser::EntityRefType::ANY) {
        return false;
    }

    else if (rhs.get_type() == Parser::EntityRefType::ANY) {
        auto lhs_procedure = get_procedure(assignments, lhs);
        return !lhs_procedure->get_uses()->empty();
    }

    auto rhs_variable_name = get_variable_name(assignments, rhs);
    auto lhs_procedure = get_procedure(assignments, lhs);
    auto lhs_procedure_uses = lhs_procedure->get_uses();
    return lhs_procedure_uses->find(rhs_variable_name) != lhs_procedure_uses->end();
}

std::unordered_set<std::string> UsesPConstraintLogic::get_synonyms() const { return synonyms; }
