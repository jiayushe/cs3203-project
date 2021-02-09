#pragma once

#include "BaseConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>

namespace QueryEvaluator {

// Constraint for describing/enforcing a Follows relationship.
class FollowsConstraint : public BaseConstraint {
public:
    FollowsConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::StatementRef lhs,
                      Parser::StatementRef rhs);

    bool is_valid(const AssignmentMap& assignment) override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::StatementRef lhs;
    Parser::StatementRef rhs;
};

} // namespace QueryEvaluator
