#include "FollowsTConstraintLogic.h"

using namespace QueryEvaluator;

FollowsTConstraintLogic::FollowsTConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                 const Parser::StatementRef& lhs,
                                                 const Parser::StatementRef& rhs)
    : pkb(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (rhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool FollowsTConstraintLogic::is_valid(const AssignmentMap& assignments) const {
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

std::unordered_set<std::string> FollowsTConstraintLogic::get_synonyms() const { return synonyms; }
