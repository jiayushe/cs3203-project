#include "ConstraintUtils.h"

using namespace QueryEvaluator;

std::vector<std::shared_ptr<BaseConstraint>>
ConstraintUtils::get_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                 Parser::QueryObject& query_object) {
    std::vector<std::shared_ptr<BaseConstraint>> constraints;

    for (auto const& such_that : query_object.get_all_such_that()) {
        auto left_ref = such_that.get_left_ref();
        auto right_ref = such_that.get_right_ref();

        switch (such_that.get_type()) {
        case Parser::SuchThatType::MODIFIES_S:
            constraints.push_back(std::make_shared<ModifiesConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::USES_S:
            constraints.push_back(std::make_shared<UsesConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_entity_ref()));
            break;
        case Parser::SuchThatType::PARENT:
            constraints.push_back(std::make_shared<ParentConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::PARENT_T:
            constraints.push_back(std::make_shared<ParentTConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::FOLLOWS:
            constraints.push_back(std::make_shared<FollowsConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::FOLLOWS_T:
            constraints.push_back(std::make_shared<FollowsTConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        default:
            throw std::runtime_error("Unknown such that type");
        }
    }

    for (auto const& pattern : query_object.get_all_pattern()) {
        constraints.push_back(std::make_shared<PatternConstraint>(pkb, pattern));
    }

    return constraints;
}
