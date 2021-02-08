#include "FollowsConstraint.h"

using namespace QueryEvaluator;

FollowsConstraint::FollowsConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                     Parser::StatementRef lhs, Parser::StatementRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool FollowsConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        return true;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement_id = get_statement_id(assignments, lhs);
        auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
        return lhs_statement.get_follower() != -1;
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement_id = get_statement_id(assignments, rhs);
        auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
        return rhs_statement.get_following() != -1;
    }

    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
    auto rhs_statement_id = get_statement_id(assignments, rhs);
    return lhs_statement.get_follower() == rhs_statement_id;
}

int FollowsConstraint::get_statement_id(const AssignmentMap& assignments,
                                        Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM:
        return assignments.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw "Cannot get statement id for ANY statement ref";
    default:
        throw "Unknown statement ref type";
    }
}
