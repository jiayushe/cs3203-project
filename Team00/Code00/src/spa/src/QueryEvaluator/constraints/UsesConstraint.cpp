#include "UsesConstraint.h"

using namespace QueryEvaluator;

UsesConstraint::UsesConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::StatementRef lhs,
                               Parser::EntityRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool UsesConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY) {
        return false;
    }

    if (rhs.get_type() == Parser::EntityRefType::ANY) {
        auto lhs_statement_id = get_statement_id(assignments, lhs);
        auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);

        switch (lhs_statement->get_type()) {
        case KnowledgeBase::StatementType::WHILE:
        case KnowledgeBase::StatementType::IF:
        case KnowledgeBase::StatementType::ASSIGN:
        case KnowledgeBase::StatementType::PRINT:
            return !lhs_statement->get_uses().empty();
        default:
            return false;
        }
    }

    auto rhs_string = get_variable_name(assignments, rhs);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto lhs_statement = pkb->get_statement_by_id(lhs_statement_id);

    switch (lhs_statement->get_type()) {
    case KnowledgeBase::StatementType::WHILE:
    case KnowledgeBase::StatementType::IF:
    case KnowledgeBase::StatementType::ASSIGN:
    case KnowledgeBase::StatementType::PRINT: {
        auto lhs_statement_uses = lhs_statement->get_uses();
        return lhs_statement_uses.find(rhs_string) != lhs_statement_uses.end();
    }
    default:
        return false;
    }
}
