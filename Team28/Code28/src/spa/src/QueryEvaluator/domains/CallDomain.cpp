#include "CallDomain.h"

using namespace QueryEvaluator;

std::vector<Assignment> CallDomain::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    std::vector<Assignment> domain;
    for (auto& statement : pkb->get_statements()) {
        if (statement->get_type() == KnowledgeBase::StatementType::CALL) {
            Assignment assignment(statement->get_id(), statement->get_procedure_called());
            domain.push_back(assignment);
        }
    }
    return domain;
}
