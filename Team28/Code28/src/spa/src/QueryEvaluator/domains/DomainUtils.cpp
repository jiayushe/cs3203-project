#include "DomainUtils.h"
#include <stdexcept>

using namespace QueryEvaluator;

std::vector<Assignment> DomainUtils::get_domain(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                Parser::DesignEntityType design_entity_type) {
    switch (design_entity_type) {
    case Parser::DesignEntityType::STMT:
        return StatementDomain::get_domain(pkb);
    case Parser::DesignEntityType::READ:
        return ReadDomain::get_domain(pkb);
    case Parser::DesignEntityType::PRINT:
        return PrintDomain::get_domain(pkb);
    case Parser::DesignEntityType::WHILE:
        return WhileDomain::get_domain(pkb);
    case Parser::DesignEntityType::IF:
        return IfDomain::get_domain(pkb);
    case Parser::DesignEntityType::ASSIGN:
        return AssignDomain::get_domain(pkb);
    case Parser::DesignEntityType::VARIABLE:
        return VariableDomain::get_domain(pkb);
    case Parser::DesignEntityType::CONSTANT:
        return ConstantDomain::get_domain(pkb);
    case Parser::DesignEntityType::PROCEDURE:
        return ProcedureDomain::get_domain(pkb);
    default:
        throw std::runtime_error("Unknown design entity type");
    }
}
