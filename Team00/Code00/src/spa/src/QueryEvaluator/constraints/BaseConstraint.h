#pragma once

#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"

namespace QueryEvaluator {

// Base class which all constraints should implement. Constraint refers to an object
// that would return true/false based on whether the given assignment is valid/not.
class BaseConstraint {
public:
    virtual bool is_valid(const AssignmentMap& assignment) = 0;

protected:
    int get_statement_id(const AssignmentMap& assignments, Parser::StatementRef& statement_ref);
    std::string get_variable_name(const AssignmentMap& assignments, Parser::EntityRef& entity_ref);
};
} // namespace QueryEvaluator
