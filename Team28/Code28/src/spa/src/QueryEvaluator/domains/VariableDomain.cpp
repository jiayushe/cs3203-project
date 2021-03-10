#include "VariableDomain.h"

using namespace QueryEvaluator;

Domain VariableDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                  const std::vector<UnaryConstraint>& constraints) {
    Domain domain;
    for (auto& variable : pkb->get_variables()) {
        Assignment assignment(variable->get_name());
        if (std::all_of(constraints.begin(), constraints.end(),
                        [&](const UnaryConstraint& constraint) {
                            return constraint.is_valid(assignment);
                        })) {
            domain.insert(assignment);
        }
    }
    return domain;
}
