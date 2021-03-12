#include "ConstraintUtils.h"

using namespace QueryEvaluator;

std::vector<EmptyConstraint>
ConstraintUtils::get_empty_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                       const Parser::QueryObject& query_object) {

    std::vector<EmptyConstraint> empty_constraints;

    auto constraint_logics = get_constraint_logics(pkb, query_object);
    for (auto const& constraint_logic : constraint_logics) {
        auto logic_synonyms = constraint_logic->get_synonyms();
        if (logic_synonyms.empty()) {
            empty_constraints.emplace_back(constraint_logic);
        }
    }

    return empty_constraints;
}

std::pair<std::vector<UnaryConstraint>, std::vector<BinaryConstraint>>
ConstraintUtils::get_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                 const Parser::QueryObject& query_object,
                                 const std::unordered_set<std::string>& synonyms) {

    std::vector<UnaryConstraint> unary_constraints;
    std::vector<BinaryConstraint> binary_constraints;

    auto constraint_logics = get_constraint_logics(pkb, query_object);
    for (auto const& constraint_logic : constraint_logics) {
        auto logic_synonyms = constraint_logic->get_synonyms();
        if (is_subset(logic_synonyms, synonyms)) {
            switch (logic_synonyms.size()) {
            case 0:
                break;
            case 1:
                unary_constraints.emplace_back(constraint_logic);
                break;
            case 2:
                binary_constraints.emplace_back(constraint_logic);
                break;
            default:
                throw std::runtime_error("Unhandled constraint logic synonym size");
            }
        }
    }

    return std::make_pair(unary_constraints, binary_constraints);
}

std::vector<std::shared_ptr<BaseConstraintLogic>>
ConstraintUtils::get_constraint_logics(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                       const Parser::QueryObject& query_object) {

    std::vector<std::shared_ptr<BaseConstraintLogic>> constraint_logics;

    for (auto const& such_that : query_object.get_all_such_that()) {
        auto left_ref = such_that.get_left_ref();
        auto right_ref = such_that.get_right_ref();

        switch (such_that.get_type()) {
        case Parser::SuchThatType::MODIFIES_S:
            constraint_logics.push_back(std::make_shared<ModifiesSConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::MODIFIES_P:
            constraint_logics.push_back(std::make_shared<ModifiesPConstraintLogic>(
                pkb, left_ref.get_entity_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::USES_S:
            constraint_logics.push_back(std::make_shared<UsesSConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::USES_P:
            constraint_logics.push_back(std::make_shared<UsesPConstraintLogic>(
                pkb, left_ref.get_entity_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::PARENT:
            constraint_logics.push_back(std::make_shared<ParentConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::PARENT_T:
            constraint_logics.push_back(std::make_shared<ParentTConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::FOLLOWS:
            constraint_logics.push_back(std::make_shared<FollowsConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::FOLLOWS_T:
            constraint_logics.push_back(std::make_shared<FollowsTConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::NEXT:
            constraint_logics.push_back(std::make_shared<NextConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::NEXT_T:
            constraint_logics.push_back(std::make_shared<NextTConstraintLogic>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
        case Parser::SuchThatType::CALLS:
            constraint_logics.push_back(std::make_shared<CallsConstraintLogic>(
                pkb, left_ref.get_entity_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::CALLS_T:
            constraint_logics.push_back(std::make_shared<CallsTConstraintLogic>(
                pkb, left_ref.get_entity_ref(), right_ref.get_entity_ref()));
            break;
        default:
            throw std::runtime_error("Unknown such that type");
        }
    }

    for (auto const& pattern : query_object.get_all_pattern()) {
        switch (pattern.get_type()) {
        case Parser::PatternType::ASSIGN:
            constraint_logics.push_back(std::make_shared<PatternLConstraintLogic>(pkb, pattern));
            constraint_logics.push_back(
                std::make_shared<PatternRConstraintLogic>(pkb, pattern.get_pattern_assign()));
            break;
        case Parser::PatternType::IF:
        case Parser::PatternType::WHILE:
            constraint_logics.push_back(std::make_shared<PatternLConstraintLogic>(pkb, pattern));
            break;
        default:
            throw std::runtime_error("Unknown pattern type");
        }
    }

    return constraint_logics;
}

bool ConstraintUtils::is_subset(const std::unordered_set<std::string>& subset,
                                const std::unordered_set<std::string>& superset) {
    if (subset.size() > superset.size()) {
        return false;
    }
    for (auto const& element : subset) {
        if (superset.find(element) == superset.end()) {
            return false;
        }
    }
    return true;
}
