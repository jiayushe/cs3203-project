#include "ProcedureDomain.h"

using namespace QueryEvaluator;

std::list<Assignment> ProcedureDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::list<Assignment> domain;
    for (auto& procedure : pkb->get_procedures()) {
        Assignment assignment(procedure.get_name());
        domain.push_back(assignment);
    }
    return domain;
}
