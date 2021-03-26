#include "AffectsConstraintLogic.h"

using namespace QueryEvaluator;

AffectsConstraintLogic::AffectsConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool AffectsConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto const& statement : statements) {
            if (!statement->get_direct_affects().empty()) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement = get_statement(assignment_map, lhs);
        return !lhs_statement->get_direct_affects().empty();
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement = get_statement(assignment_map, rhs);
        return !rhs_statement->get_direct_affected_by().empty();
    }

    auto lhs_statement = get_statement(assignment_map, lhs);
    auto rhs_statement_id = get_statement_id(assignment_map, rhs);

    auto lhs_statement_direct_affects = lhs_statement->get_direct_affects();
    return lhs_statement_direct_affects.find(rhs_statement_id) !=
           lhs_statement_direct_affects.end();
}

std::unordered_set<std::string> AffectsConstraintLogic::get_synonyms() const { return synonyms; }
