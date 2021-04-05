#include "CallsTConstraintLogic.h"

using namespace QueryEvaluator;

CallsTConstraintLogic::CallsTConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool CallsTConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {

    if (lhs.get_type() == Parser::EntityRefType::ANY &&
        rhs.get_type() == Parser::EntityRefType::ANY) {
        auto procedures = pkb->get_procedures();
        for (auto const& procedure : procedures) {
            if (!procedure->get_callees()->empty()) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::EntityRefType::ANY &&
        rhs.get_type() == Parser::EntityRefType::ANY) {
        auto lhs_procedure = get_procedure(assignment_map, lhs);
        return !lhs_procedure->get_callees()->empty();
    }

    if (lhs.get_type() == Parser::EntityRefType::ANY &&
        rhs.get_type() != Parser::EntityRefType::ANY) {
        auto rhs_procedure = get_procedure(assignment_map, rhs);
        return !rhs_procedure->get_callers()->empty();
    }

    auto lhs_procedure = get_procedure(assignment_map, lhs);
    auto rhs_procedure_name = get_procedure_name(assignment_map, rhs);

    auto lhs_procedure_callees = lhs_procedure->get_callees();
    return lhs_procedure_callees->find(rhs_procedure_name) != lhs_procedure_callees->end();
}

std::unordered_set<std::string> CallsTConstraintLogic::get_synonyms() const { return synonyms; }
