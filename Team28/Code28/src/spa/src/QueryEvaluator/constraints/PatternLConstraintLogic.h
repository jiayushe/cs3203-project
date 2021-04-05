#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "KnowledgeBase/Statement.h"
#include "Parser/PQLParser/Pattern.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

class PatternLConstraintLogic : public BaseConstraintLogic {
public:
    PatternLConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                            const Parser::Pattern& pattern);

    bool is_valid(const AssignmentMap& assignment_map) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    std::unordered_set<std::string> synonyms;
    std::string pattern_synonym;
    Parser::EntityRef entity_ref;
    Parser::PatternType type;
};

} // namespace QueryEvaluator
