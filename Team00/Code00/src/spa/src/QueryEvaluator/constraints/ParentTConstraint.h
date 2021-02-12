#pragma once

#include "BaseConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "KnowledgeBase/Statement.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>

namespace QueryEvaluator {

// Constraint for describing/enforcing a Parent* relationship.
class ParentTConstraint : public BaseConstraint {
public:
    ParentTConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::StatementRef lhs,
                     Parser::StatementRef rhs);

    bool is_valid(const AssignmentMap& assignment) override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::StatementRef lhs;
    Parser::StatementRef rhs;
};

} // namespace QueryEvaluator
