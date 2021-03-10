#include "UsesSConstraintLogic.h"
#include <stdexcept>

using namespace QueryEvaluator;

UsesSConstraintLogic::UsesSConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                           const Parser::StatementRef& lhs,
                                           const Parser::EntityRef& rhs)
    : pkb(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (rhs.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool UsesSConstraintLogic::is_valid(const AssignmentMap& assignments) const {
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
        case KnowledgeBase::StatementType::READ:
        case KnowledgeBase::StatementType::CALL:
            return false;
        default:
            throw std::runtime_error("Unhandled statement type");
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
    case KnowledgeBase::StatementType::READ:
    case KnowledgeBase::StatementType::CALL:
        return false;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

std::unordered_set<std::string> UsesSConstraintLogic::get_synonyms() const { return synonyms; }
