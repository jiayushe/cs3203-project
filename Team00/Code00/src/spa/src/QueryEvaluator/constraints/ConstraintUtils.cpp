#include "ConstraintUtils.h"

using namespace QueryEvaluator;

std::list<std::shared_ptr<BaseConstraint>>
ConstraintUtils::get_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                 Parser::QueryObject& query_object) {
    std::list<std::shared_ptr<BaseConstraint>> constraints;

    if (query_object.has_such_that()) {
        auto such_that = query_object.get_such_that();
        auto left_ref = such_that.get_left_ref();
        auto right_ref = such_that.get_right_ref();

        switch (such_that.get_type()) {
        case Parser::SuchThatType::MODIFIES_S:
            // TODO: Implement this
            break;
        case Parser::SuchThatType::USES_S:
            // TODO: Implement this
            break;
        case Parser::SuchThatType::PARENT:
            constraints.push_back(std::make_shared<ParentConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::PARENT_T:
            // TODO: Implement this
            break;
        case Parser::SuchThatType::FOLLOWS:
            // TODO: Uncomment this
            constraints.push_back(std::make_shared<FollowsConstraint>(
                pkb, left_ref.get_statement_ref(), right_ref.get_statement_ref()));
            break;
        case Parser::SuchThatType::FOLLOWS_T:
            // TODO: Implement this
            break;
        default:
            throw "Unknown such that type";
        }
    }

    if (query_object.has_pattern()) {
        // TODO: Implement this
    }

    return constraints;
}
