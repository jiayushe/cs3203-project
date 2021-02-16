#pragma once

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/QueryObject.h"
#include "QueryEvaluator/constraints/BaseConstraint.h"
#include "QueryEvaluator/constraints/FollowsConstraint.h"
#include "QueryEvaluator/constraints/FollowsTConstraint.h"
#include "QueryEvaluator/constraints/ModifiesConstraint.h"
#include "QueryEvaluator/constraints/ParentConstraint.h"
#include "QueryEvaluator/constraints/ParentTConstraint.h"
#include "QueryEvaluator/constraints/PatternConstraint.h"
#include "QueryEvaluator/constraints/UsesConstraint.h"
#include <list>
#include <memory>

namespace QueryEvaluator {

class ConstraintUtils {
public:
    // Get all constraints for a given query. For instance, the query
    // `Select s such that Follows(..., ...) pattern a(..., ...)` would have
    // two constraints associated with it, i.e. one for Follows and one for pattern.
    static std::list<std::shared_ptr<BaseConstraint>>
    get_constraints(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::QueryObject& query_object);
};

} // namespace QueryEvaluator
