#include "ParentTConstraint.h"

using namespace QueryEvaluator;

ParentTConstraint::ParentTConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool ParentTConstraint::is_valid(const AssignmentMap& assignments) {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto& statement : statements) {
            if (!statement->get_ancestors().empty()) {
                return true;
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
            return !lhs_statement->get_descendants().empty();
        case KnowledgeBase::StatementType::ASSIGN:
        case KnowledgeBase::StatementType::READ:
        case KnowledgeBase::StatementType::PRINT:
        case KnowledgeBase::StatementType::CALL:
            return false;
        default:
            throw "Unhandled statement type";
        }
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement_id = get_statement_id(assignments, rhs);
        auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
        return !rhs_statement->get_ancestors().empty();
    }

    auto rhs_statement_id = get_statement_id(assignments, rhs);
    auto rhs_statement = pkb->get_statement_by_id(rhs_statement_id);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto rhs_statement_ancestors = rhs_statement->get_ancestors();
    return rhs_statement_ancestors.find(lhs_statement_id) != rhs_statement_ancestors.end();
}

std::unordered_set<std::string> ParentTConstraint::get_synonyms() const { return synonyms; }
