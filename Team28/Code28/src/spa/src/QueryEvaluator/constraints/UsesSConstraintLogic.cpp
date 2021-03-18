#include "UsesSConstraintLogic.h"
#include <stdexcept>

using namespace QueryEvaluator;

UsesSConstraintLogic::UsesSConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                           const Parser::StatementRef& lhs,
                                           const Parser::EntityRef& rhs)
    : BaseConstraintLogic(std::move(pkb)), lhs(lhs), rhs(rhs) {
    if (lhs.get_type() == Parser::StatementRefType::SYNONYM) {
        synonyms.insert(lhs.get_synonym());
    }
    if (rhs.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(rhs.get_synonym());
    }
}

bool UsesSConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {
    if (lhs.get_type() == Parser::StatementRefType::ANY) {
        return false;
    }

    if (rhs.get_type() == Parser::EntityRefType::ANY) {
        auto lhs_statement = get_statement(assignment_map, lhs);

        switch (lhs_statement->get_type()) {
        case KnowledgeBase::StatementType::WHILE:
        case KnowledgeBase::StatementType::IF:
        case KnowledgeBase::StatementType::ASSIGN:
        case KnowledgeBase::StatementType::PRINT:
        case KnowledgeBase::StatementType::CALL:
            return !lhs_statement->get_uses().empty();
        case KnowledgeBase::StatementType::READ:
            return false;
        default:
            throw std::runtime_error("Unhandled statement type");
        }
    }

    auto rhs_variable_name = get_variable_name(assignment_map, rhs);
    auto lhs_statement = get_statement(assignment_map, lhs);

    switch (lhs_statement->get_type()) {
    case KnowledgeBase::StatementType::WHILE:
    case KnowledgeBase::StatementType::IF:
    case KnowledgeBase::StatementType::ASSIGN:
    case KnowledgeBase::StatementType::PRINT:
    case KnowledgeBase::StatementType::CALL: {
        auto lhs_statement_uses = lhs_statement->get_uses();
        return lhs_statement_uses.find(rhs_variable_name) != lhs_statement_uses.end();
    }
    case KnowledgeBase::StatementType::READ:
        return false;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

std::unordered_set<std::string> UsesSConstraintLogic::get_synonyms() const { return synonyms; }
