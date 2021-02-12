#include "ParentTConstraint.h"

using namespace QueryEvaluator;

ParentTConstraint::ParentTConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   Parser::StatementRef lhs, Parser::StatementRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool ParentTConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto& statement : statements) {
            switch (statement.get_type()) {
            case KnowledgeBase::StatementType::WHILE:
            case KnowledgeBase::StatementType::IF:
                if (!statement.get_descendants().empty()) {
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
        switch (lhs_statement.get_type()) {
        case KnowledgeBase::StatementType::WHILE:
        case KnowledgeBase::StatementType::IF:
            if (!lhs_statement.get_descendants().empty()) {
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
        return !rhs_statement.get_ancestors().empty();
    }

    auto rhs_statement_id = get_statement_id(assignments, rhs);
    auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto rhs_statement_ancestors = rhs_statement.get_ancestors();
    return rhs_statement_ancestors.find(lhs_statement_id) != rhs_statement_ancestors.end();
}
