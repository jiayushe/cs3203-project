#include "AffectsBipConstraintLogic.h"

using namespace QueryEvaluator;

AffectsBipConstraintLogic::AffectsBipConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
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

bool AffectsBipConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {
    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto statements = pkb->get_statements();
        for (auto const& statement : statements) {
            if (!statement->get_direct_affects_bip()->empty()) {
                return true;
            }
        }
        return false;
    }

    if (lhs.get_type() != Parser::StatementRefType::ANY &&
        rhs.get_type() == Parser::StatementRefType::ANY) {
        auto lhs_statement = get_statement(assignment_map, lhs);
        return !lhs_statement->get_direct_affects_bip()->empty();
    }

    if (lhs.get_type() == Parser::StatementRefType::ANY &&
        rhs.get_type() != Parser::StatementRefType::ANY) {
        auto rhs_statement = get_statement(assignment_map, rhs);
        return !rhs_statement->get_direct_affected_bip_by()->empty();
    }

    auto lhs_statement = get_statement(assignment_map, lhs);
    auto rhs_statement_id = get_statement_id(assignment_map, rhs);

    auto lhs_statement_direct_affects_bip = lhs_statement->get_direct_affects_bip();
    return lhs_statement_direct_affects_bip->find(rhs_statement_id) !=
           lhs_statement_direct_affects_bip->end();
}

std::unordered_set<std::string> AffectsBipConstraintLogic::get_synonyms() const { return synonyms; }
