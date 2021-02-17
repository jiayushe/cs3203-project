#include "WhileDomain.h"

using namespace QueryEvaluator;

std::vector<Assignment> WhileDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::vector<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::WHILE) {
            Assignment assignment(statement->get_id());
            domain.push_back(assignment);
        }
    }
    return domain;
}
