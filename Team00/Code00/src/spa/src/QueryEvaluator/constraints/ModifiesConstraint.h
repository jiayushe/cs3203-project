#pragma once

#include "BaseConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>

namespace QueryEvaluator {

// Constraint for describing/enforcing a Modifies relationship.
class ModifiesConstraint : public BaseConstraint {
public:
    ModifiesConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::StatementRef lhs,
                       Parser::EntityRef rhs);

    bool is_valid(const AssignmentMap& assignment) override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::StatementRef lhs;
    Parser::EntityRef rhs;
};

} // namespace QueryEvaluator
