#include "DomainUtils.h"

#include "StatementDomain.h"

using namespace QueryEvaluator;

std::list<Assignment> DomainUtils::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                              Parser::DesignEntityType design_entity_type) {
    switch (design_entity_type) {
    case Parser::DesignEntityType::STMT:
        // TODO: Uncomment this
        // return StatementDomain::get_domain(pkb);
        return std::list<Assignment>();
    case Parser::DesignEntityType::READ:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::PRINT:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::WHILE:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::IF:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::ASSIGN:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::VARIABLE:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::CONSTANT:
        // TODO: Implement this
        return std::list<Assignment>();
    case Parser::DesignEntityType::PROCEDURE:
        // TODO: Implement this
        return std::list<Assignment>();
    default:
        throw "Unknown design entity type";
    }
}
