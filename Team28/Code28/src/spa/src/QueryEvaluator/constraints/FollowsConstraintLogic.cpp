#include "FollowsConstraintLogic.h"

using namespace QueryEvaluator;

FollowsConstraintLogic::FollowsConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                               const Parser::StatementRef& lhs,
                                               const Parser::StatementRef& rhs)
    : pkb(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (this->rhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool FollowsConstraintLogic::is_valid(const AssignmentMap& assignments) const {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto statement : statements) {
            if (statement->get_direct_follower() != -1) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement_id = get_statement_id(assignments, lhs);
        auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
        return lhs_statement->get_direct_follower() != -1;
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement_id = get_statement_id(assignments, rhs);
        auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
        return rhs_statement->get_direct_following() != -1;
    }

    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);
    auto rhs_statement_id = get_statement_id(assignments, rhs);
    return lhs_statement->get_direct_follower() == rhs_statement_id;
}

std::unordered_set<std::string> FollowsConstraintLogic::get_synonyms() const { return synonyms; }
