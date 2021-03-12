#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DeclarationMap.h"
#include "Parser/PQLParser/EntityRef.h"
#include "QueryEvaluator/Common.h"
#include <memory>

namespace QueryEvaluator {

// Constraint logic for describing/enforcing a ModifiesS relationship.
class ModifiesPConstraintLogic : public BaseConstraintLogic {
public:
    ModifiesPConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::EntityRef& lhs,
                             const Parser::EntityRef& rhs);

    bool is_valid(const AssignmentMap& assignment) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    Parser::EntityRef lhs;
    Parser::EntityRef rhs;
    std::unordered_set<std::string> synonyms;
};

} // namespace QueryEvaluator
