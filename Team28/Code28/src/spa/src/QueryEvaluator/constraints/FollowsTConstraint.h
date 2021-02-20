#pragma once

#include "BaseConstraint.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>

namespace QueryEvaluator {

// Constraint for describing/enforcing a Follows* relationship.
class FollowsTConstraint : public BaseConstraint {
public:
    FollowsTConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::StatementRef& lhs,
                       const Parser::StatementRef& rhs);

    bool is_valid(const AssignmentMap& assignment) override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::StatementRef lhs;
    Parser::StatementRef rhs;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
