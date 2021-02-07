#include "StatementDomain.h"

using namespace QueryEvaluator;

std::list<Assignment> StatementDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::list<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        Assignment assignment(statement.get_id());
        domain.push_back(assignment);
    }
    return domain;
}