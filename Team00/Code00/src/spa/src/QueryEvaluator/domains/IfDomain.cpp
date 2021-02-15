#include "IfDomain.h"

using namespace QueryEvaluator;

std::list<Assignment> IfDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::list<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::IF) {
            Assignment assignment(statement->get_id());
            domain.push_back(assignment);
        }
    }
    return domain;
}
