#pragma once

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/DesignEntity.h"
#include "QueryEvaluator/Assignment.h"
#include <list>
#include <memory>

namespace QueryEvaluator {

class DomainUtils {
public:
    // Get all domains for a given design entity type. For instance, if the given
    // design entity type is `DesignEntityType::STMT`, this method would return
    // assignments representing all the statements in the SIMPLE source.
    static std::list<Assignment> get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                            Parser::DesignEntityType design_entity_type);
};

} // namespace QueryEvaluator
