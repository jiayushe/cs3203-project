#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/Pattern.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

class PatternRConstraintLogic : public BaseConstraintLogic {
public:
    PatternRConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                            const Parser::PatternAssign& pattern_assign);

    bool is_valid(const AssignmentMap& assignment_map) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    std::string assign_synonym;
    Parser::ExpressionSpec expression_spec;
};

} // namespace QueryEvaluator
