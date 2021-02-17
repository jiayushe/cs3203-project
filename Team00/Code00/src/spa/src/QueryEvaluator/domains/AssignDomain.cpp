#include "AssignDomain.h"

using namespace QueryEvaluator;

std::vector<Assignment> AssignDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::vector<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::ASSIGN) {
            Assignment assignment(statement->get_id());
            domain.push_back(assignment);
        }
    }
    return domain;
}
