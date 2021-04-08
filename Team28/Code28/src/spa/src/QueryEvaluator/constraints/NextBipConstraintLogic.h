#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/StatementRef.h"
#include <memory>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

class NextBipConstraintLogic : public BaseConstraintLogic {
public:
    NextBipConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::StatementRef& lhs,
                        const Parser::StatementRef& rhs);

    bool is_valid(const AssignmentMap& assignment_map) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    Parser::StatementRef lhs;
    Parser::StatementRef rhs;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
