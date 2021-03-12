#pragma once

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/AttrRef.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/Common.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

// An abstract class which all constraint logics must implement. A constraint logic refers to the
// logic behind every BinaryConstraint, UnaryConstraint, and EmptyConstraint.
// For instance, Follows(1, s) forms a UnaryConstraint which contains a FollowsConstraintLogic.
class BaseConstraintLogic {
public:
    virtual bool is_valid(const AssignmentMap& assignment_map) const = 0;
    virtual std::unordered_set<std::string> get_synonyms() const = 0;

protected:
    std::shared_ptr<KnowledgeBase::PKB> pkb;

    explicit BaseConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb);

    std::shared_ptr<KnowledgeBase::Statement> get_statement(const AssignmentMap& assignment_map,
                                                            const std::string& synonym) const;
    std::shared_ptr<KnowledgeBase::Statement>
    get_statement(const AssignmentMap& assignment_map,
                  const Parser::StatementRef& statement_ref) const;
    std::shared_ptr<KnowledgeBase::Variable> get_variable(const AssignmentMap& assignment_map,
                                                          const std::string& synonym) const;
    std::shared_ptr<KnowledgeBase::Variable>
    get_variable(const AssignmentMap& assignment_map, const Parser::EntityRef& entity_ref) const;
    std::shared_ptr<KnowledgeBase::Procedure> get_procedure(const AssignmentMap& assignment_map,
                                                            const std::string& synonym) const;
    std::shared_ptr<KnowledgeBase::Procedure>
    get_procedure(const AssignmentMap& assignment_map, const Parser::EntityRef& entity_ref) const;

    static int get_statement_id(const AssignmentMap& assignment_map,
                                const Parser::AttrRef& attr_ref);
    static int get_statement_id(const AssignmentMap& assignment_map,
                                const Parser::StatementRef& statement_ref);
    static int get_constant_value(const AssignmentMap& assignment_map,
                                  const Parser::AttrRef& attr_ref);
    static std::string get_variable_name(const AssignmentMap& assignment_map,
                                         const Parser::EntityRef& entity_ref);
    static std::string get_variable_name(const AssignmentMap& assignment_map,
                                         const Parser::AttrRef& attr_ref);
    static std::string get_procedure_name(const AssignmentMap& assignment_map,
                                          const Parser::EntityRef& entity_ref);
    static std::string get_procedure_name(const AssignmentMap& assignment_map,
                                          const Parser::AttrRef& attr_ref);
};

} // namespace QueryEvaluator
