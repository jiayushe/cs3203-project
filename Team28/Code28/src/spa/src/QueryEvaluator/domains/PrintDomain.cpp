#include "PrintDomain.h"

using namespace QueryEvaluator;

std::vector<Assignment> PrintDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::vector<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::PRINT) {
            Assignment assignment(statement->get_id(), *(statement->get_direct_uses().begin()));
            domain.push_back(assignment);
        }
    }
    return domain;
}
