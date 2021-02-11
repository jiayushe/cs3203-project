#pragma once

#include "KnowledgeBase/PKB.h"
#include "QueryEvaluator/Assignment.h"
#include <list>
#include <memory>

namespace QueryEvaluator {

class VariableDomain {
public:
    static std::list<Assignment> get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb);
};

} // namespace QueryEvaluator
