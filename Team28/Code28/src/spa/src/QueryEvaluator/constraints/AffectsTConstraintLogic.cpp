#include "AffectsTConstraintLogic.h"

using namespace QueryEvaluator;

AffectsTConstraintLogic::AffectsTConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool AffectsTConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto const& statement : statements) {
            if (!statement->get_affects()->empty()) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement = get_statement(assignment_map, lhs);
        return !lhs_statement->get_affects()->empty();
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement = get_statement(assignment_map, rhs);
        return !rhs_statement->get_affected_by()->empty();
    }

    auto lhs_statement = get_statement(assignment_map, lhs);
    auto rhs_statement_id = get_statement_id(assignment_map, rhs);

    auto lhs_statement_affects = lhs_statement->get_affects();
    return lhs_statement_affects->find(rhs_statement_id) != lhs_statement_affects->end();
}

std::unordered_set<std::string> AffectsTConstraintLogic::get_synonyms() const { return synonyms; }
