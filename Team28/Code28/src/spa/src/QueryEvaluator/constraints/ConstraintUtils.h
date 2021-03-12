#pragma once

#include "BaseConstraintLogic.h"
#include "BinaryConstraint.h"
#include "EmptyConstraint.h"
#include "FollowsConstraintLogic.h"
#include "FollowsTConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "ModifiesSConstraintLogic.h"
#include "ModifiesPConstraintLogic.h"
#include "ParentConstraintLogic.h"
#include "ParentTConstraintLogic.h"
#include "Parser/PQLParser/QueryObject.h"
#include "PatternLConstraintLogic.h"
#include "PatternRConstraintLogic.h"
#include "UnaryConstraint.h"
#include "UsesSConstraintLogic.h"
#include "UsesPConstraintLogic.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace QueryEvaluator {

class ConstraintUtils {
public:
    // Get all empty constraints from the given query object.
    static std::vector<EmptyConstraint>
    get_empty_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                          const Parser::QueryObject& query_object);

    // Get all unary and binary constraints from the given query object.
    // The `synonyms` set is used to filter constraints such that only those
    // which involve the synonyms from `synonyms` are included in the result.
    static std::pair<std::vector<UnaryConstraint>, std::vector<BinaryConstraint>>
    get_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb,
                    const Parser::QueryObject& query_object,
                    const std::unordered_set<std::string>& synonyms);

private:
    // Get all constraint logics from the given query object.
    static std::vector<std::shared_ptr<BaseConstraintLogic>>
    get_constraint_logics(std::shared_ptr<KnowledgeBase::PKB> pkb,
                          const Parser::QueryObject& query_object);

    // Check whether a string set is a subset of another.
    static bool is_subset(const std::unordered_set<std::string>& subset,
                          const std::unordered_set<std::string>& superset);
};

} // namespace QueryEvaluator
