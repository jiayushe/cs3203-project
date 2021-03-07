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
#include <stdexcept>

namespace QueryEvaluator {

// Constraint for matching a pattern constraint
class PatternConstraint : public BaseConstraint {
public:
    PatternConstraint(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::Pattern& pattern);

    bool is_valid(const AssignmentMap& assignment) override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::Pattern pattern;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
