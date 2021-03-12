#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/EntityRef.h"
#include "QueryEvaluator/AssignmentMap.h"
#include <memory>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

class CallsConstraintLogic : public BaseConstraintLogic {
public:
    CallsConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::EntityRef& lhs,
                         const Parser::EntityRef& rhs);

    bool is_valid(const AssignmentMap& assignment) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    Parser::EntityRef lhs;
    Parser::EntityRef rhs;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
