#include "PrintDomain.h"

using namespace QueryEvaluator;

Domain PrintDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                               const std::vector<UnaryConstraint>& constraints) {
    Domain domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::PRINT) {
            Assignment assignment(statement->get_id(), *(statement->get_direct_uses().begin()));
            if (std::all_of(constraints.begin(), constraints.end(),
                            [&](const UnaryConstraint& constraint) {
                                return constraint.is_valid(assignment);
                            })) {
                domain.insert(assignment);
            }
        }
    }
    return domain;
}
