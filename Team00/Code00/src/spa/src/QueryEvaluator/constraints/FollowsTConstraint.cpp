#include "FollowsTConstraint.h"

using namespace QueryEvaluator;

FollowsTConstraint::FollowsTConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                     Parser::StatementRef lhs, Parser::StatementRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool FollowsTConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto statement : statements) {
            if (!statement->get_followers().empty()) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement_id = get_statement_id(assignments, lhs);
        auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
        return !lhs_statement->get_followers().empty();
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement_id = get_statement_id(assignments, rhs);
        auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
        return !rhs_statement->get_followings().empty();
    }

    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
    auto rhs_statement_id = get_statement_id(assignments, rhs);
    auto lhs_statement_followers = lhs_statement->get_followers();
    return lhs_statement_followers.find(rhs_statement_id) != lhs_statement_followers.end();
}
