#include "ModifiesPConstraintLogic.h"
#include <stdexcept>

using namespace QueryEvaluator;

ModifiesPConstraintLogic::ModifiesPConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool ModifiesPConstraintLogic::is_valid(const AssignmentMap& assignments) const {
    if (lhs.get_type() == Parser::EntityRefType::ANY) {
        return false;
    }

    if (rhs.get_type() == Parser::EntityRefType::ANY) {
        auto lhs_procedure = get_procedure(assignments, lhs);

        return !lhs_procedure->get_modifies().empty();
    }

    auto rhs_variable_name = get_variable_name(assignments, rhs);
    auto lhs_procedure = get_procedure(assignments, lhs);

    auto lhs_procedure_modifies = lhs_procedure->get_modifies();
    return lhs_procedure_modifies.find(rhs_variable_name) != lhs_procedure_modifies.end();
}

std::unordered_set<std::string> ModifiesPConstraintLogic::get_synonyms() const { return synonyms; }
