#include "ProcedureDomain.h"

using namespace QueryEvaluator;

Domain ProcedureDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   const std::vector<UnaryConstraint>& constraints) {
    Domain domain;
    for (auto& procedure : pkb->get_procedures()) {
        Assignment assignment(procedure->get_name());
        if (std::all_of(constraints.begin(), constraints.end(),
                        [&](const UnaryConstraint& constraint) {
                            return constraint.is_valid(assignment);
                        })) {
            domain.insert(assignment);
        }
    }
    return domain;
}
