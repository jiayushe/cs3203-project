#pragma once

#include "KnowledgeBase/PKB.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>
#include <vector>

namespace QueryEvaluator {

class ConstantDomain {
public:
    static std::vector<Assignment> get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb);
};

} // namespace QueryEvaluator
