#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Common.h"
#include <memory>

namespace QueryEvaluator {

// Constraint logic for describing/enforcing a Follows* relationship.
class FollowsTConstraintLogic : public BaseConstraintLogic {
public:
    FollowsTConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                            const Parser::StatementRef& lhs, const Parser::StatementRef& rhs);

    bool is_valid(const AssignmentMap& assignment) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    Parser::StatementRef lhs;
    Parser::StatementRef rhs;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
