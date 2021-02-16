#include "ModifiesConstraint.h"

using namespace QueryEvaluator;

ModifiesConstraint::ModifiesConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                       Parser::StatementRef lhs, Parser::EntityRef rhs)
    : pkb(std::move(pkb)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

bool ModifiesConstraint::is_valid(const AssignmentMap& assignments) {
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
        case KnowledgeBase::StatementType::READ:
            return !lhs_statement->get_modifies().empty();
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
    case KnowledgeBase::StatementType::READ: {
        auto lhs_statement_modifies = lhs_statement->get_modifies();
        return lhs_statement_modifies.find(rhs_string) != lhs_statement_modifies.end();
    }
    default:
        return false;
    }
}
