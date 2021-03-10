#pragma once

#include "AssignDomain.h"
#include "CallDomain.h"
#include "ConstantDomain.h"
#include "IfDomain.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DesignEntity.h"
#include "PrintDomain.h"
#include "ProcedureDomain.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/Common.h"
#include "QueryEvaluator/constraints/UnaryConstraint.h"
#include "ReadDomain.h"
#include "StatementDomain.h"
#include "VariableDomain.h"
#include "WhileDomain.h"
#include <memory>
#include <stdexcept>
#include <vector>

namespace QueryEvaluator {

class DomainUtils {
public:
    // Get all domains for a given design entity type. For instance, if the given
    // design entity type is `DesignEntityType::STMT`, this method would return
    // assignments representing all the statements in the SIMPLE source. Additionally,
    // filter out the domain using the given list of constraints.
    static Domain get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                             Parser::DesignEntityType design_entity_type,
                             const std::vector<UnaryConstraint>& constraints);
};

} // namespace QueryEvaluator
