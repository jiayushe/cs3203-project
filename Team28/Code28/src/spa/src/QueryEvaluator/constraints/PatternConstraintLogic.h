#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/ExpressionSpec.h"
#include "Parser/PQLParser/Pattern.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Common.h"
#include <memory>
#include <stdexcept>

namespace QueryEvaluator {

// Constraint logic for describing/enforcing a pattern clause.
class PatternConstraintLogic : public BaseConstraintLogic {
public:
    PatternConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::Pattern& pattern);

    bool is_valid(const AssignmentMap& assignment) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
    Parser::Pattern pattern;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
