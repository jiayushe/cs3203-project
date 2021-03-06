#include "ParentConstraintLogic.h"
#include <stdexcept>

using namespace QueryEvaluator;

ParentConstraintLogic::ParentConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                             const Parser::StatementRef& lhs,
                                             const Parser::StatementRef& rhs)
    : BaseConstraintLogic(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (rhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool ParentConstraintLogic::is_valid(const AssignmentMap& assignments) const {

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto& statement : statements) {
            if (statement->get_parent() != -1) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement = get_statement(assignments, lhs);

        switch (lhs_statement->get_type()) {
        case KnowledgeBase::StatementType::WHILE:
        case KnowledgeBase::StatementType::IF:
            return !lhs_statement->get_children()->empty();
        case KnowledgeBase::StatementType::ASSIGN:
        case KnowledgeBase::StatementType::READ:
        case KnowledgeBase::StatementType::PRINT:
        case KnowledgeBase::StatementType::CALL:
            return false;
            break;
        default:
            throw std::runtime_error("Unhandled statement type");
        }
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement = get_statement(assignments, rhs);
        return rhs_statement->get_parent() != -1;
    }

    auto rhs_statement = get_statement(assignments, rhs);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    return rhs_statement->get_parent() == lhs_statement_id;
}

std::unordered_set<std::string> ParentConstraintLogic::get_synonyms() const { return synonyms; }
