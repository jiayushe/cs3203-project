#include "VariableDomain.h"

using namespace QueryEvaluator;

std::list<Assignment> VariableDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::list<Assignment> domain;
    for (auto& variable : pkb->get_variables()) {
        Assignment assignment(variable.get_name());
        domain.push_back(assignment);
    }
    return domain;
}
