#pragma once

#include "Parser/PQLParser/AttrRef.h"
#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/Common.h"
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

// An abstract class which all constraint logics must implement. A constraint logic refers to the
// logic behind every BinaryConstraint, UnaryConstraint, and EmptyConstraint.
// For instance, Follows(1, s) forms a UnaryConstraint which contains a FollowsConstraintLogic.
class BaseConstraintLogic {
public:
    virtual bool is_valid(const AssignmentMap& assignment) const = 0;
    virtual std::unordered_set<std::string> get_synonyms() const = 0;

protected:
    static int get_statement_id(const AssignmentMap& assignments,
                                const Parser::StatementRef& statement_ref);
    static std::string get_variable_name(const AssignmentMap& assignments,
                                         const Parser::EntityRef& entity_ref);
    static std::string get_procedure_name(const AssignmentMap& assignments,
                                          const Parser::EntityRef& entity_ref);

    static int get_statement_id(const AssignmentMap& assignments, const Parser::AttrRef& attr_ref);
    static int get_constant_value(const AssignmentMap& assignments,
                                  const Parser::AttrRef& attr_ref);
    static std::string get_variable_name(const AssignmentMap& assignments,
                                         const Parser::AttrRef& attr_ref);
    static std::string get_procedure_name(const AssignmentMap& assignments,
                                          const Parser::AttrRef& attr_ref);
};

} // namespace QueryEvaluator
