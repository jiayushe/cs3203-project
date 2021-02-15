#include "ParentConstraint.h"

using namespace QueryEvaluator;

ParentConstraint::ParentConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   Parser::StatementRef lhs, Parser::StatementRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool ParentConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto& statement : statements) {
            switch (statement->get_type()) {
            case KnowledgeBase::StatementType::WHILE:
            case KnowledgeBase::StatementType::IF:
                if (!statement->get_children().empty()) {
                    return true;
                }
                break;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement_id = get_statement_id(assignments, lhs);
        auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
        switch (lhs_statement->get_type()) {
        case KnowledgeBase::StatementType::WHILE:
        case KnowledgeBase::StatementType::IF:
            if (!lhs_statement->get_children().empty()) {
                return true;
            }
            break;
        }

        return false;
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement_id = get_statement_id(assignments, rhs);
        auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
        return rhs_statement->get_parent() != -1;
    }

    auto rhs_statement_id = get_statement_id(assignments, rhs);
    auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    return rhs_statement->get_parent() == lhs_statement_id;
}
