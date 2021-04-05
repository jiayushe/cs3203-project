#include "ParentTConstraintLogic.h"

using namespace QueryEvaluator;

ParentTConstraintLogic::ParentTConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool ParentTConstraintLogic::is_valid(const AssignmentMap& assignments) const {

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto& statement : statements) {
            if (!statement->get_ancestors()->empty()) {
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
            return !lhs_statement->get_descendants()->empty();
        case KnowledgeBase::StatementType::ASSIGN:
        case KnowledgeBase::StatementType::READ:
        case KnowledgeBase::StatementType::PRINT:
        case KnowledgeBase::StatementType::CALL:
            return false;
        default:
            throw std::runtime_error("Unhandled statement type");
        }
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement = get_statement(assignments, rhs);
        return !rhs_statement->get_ancestors()->empty();
    }

    auto rhs_statement = get_statement(assignments, rhs);
    auto lhs_statement_id = get_statement_id(assignments, lhs);
    auto rhs_statement_ancestors = rhs_statement->get_ancestors();
    return rhs_statement_ancestors->find(lhs_statement_id) != rhs_statement_ancestors->end();
}

std::unordered_set<std::string> ParentTConstraintLogic::get_synonyms() const { return synonyms; }
