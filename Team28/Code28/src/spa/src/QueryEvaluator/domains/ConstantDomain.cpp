#include "ConstantDomain.h"

using namespace QueryEvaluator;

Domain ConstantDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                  const std::vector<UnaryConstraint>& constraints) {
    Domain domain;
    for (auto& constant : pkb->get_constants()) {
        Assignment assignment(constant->get_value());
        if (std::all_of(constraints.begin(), constraints.end(),
                        [&](const UnaryConstraint& constraint) {
                            return constraint.is_valid(assignment);
                        })) {
            domain.insert(assignment);
        }
    }
    return domain;
}
