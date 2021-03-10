#pragma once

#include "KnowledgeBase/PKB.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/Common.h"
#include "QueryEvaluator/constraints/UnaryConstraint.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace QueryEvaluator {

class ProcedureDomain {
public:
    static Domain get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                             const std::vector<UnaryConstraint>& constraints);
};

} // namespace QueryEvaluator
