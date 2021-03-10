#include "WhileDomain.h"

using namespace QueryEvaluator;

Domain WhileDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                               const std::vector<UnaryConstraint>& constraints) {
    Domain domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::WHILE) {
            Assignment assignment(statement->get_id());
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
