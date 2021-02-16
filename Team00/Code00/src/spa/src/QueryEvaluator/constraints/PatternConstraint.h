#pragma once

#include "BaseConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/ExpressionSpec.h"
#include "Parser/PQLParser/Pattern.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>

namespace QueryEvaluator {

// Constraint for matching a pattern constraint
class PatternConstraint : public BaseConstraint {
public:
    PatternConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::Pattern pattern);

    bool is_valid(const AssignmentMap& assignment) override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::Pattern pattern;
};

} // namespace QueryEvaluator
