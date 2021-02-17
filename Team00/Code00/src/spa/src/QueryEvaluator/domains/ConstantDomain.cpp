#include "ConstantDomain.h"

using namespace QueryEvaluator;

std::vector<Assignment> ConstantDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::vector<Assignment> domain;
    for (auto& constant : pkb->get_constants()) {
        Assignment assignment(constant->get_value());
        domain.push_back(assignment);
    }
    return domain;
}
