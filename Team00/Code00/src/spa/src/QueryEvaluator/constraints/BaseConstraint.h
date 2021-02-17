#pragma once

#include "Parser/PQLParser/EntityRef.h"
#include "Parser/PQLParser/StatementRef.h"
#include "QueryEvaluator/Assignment.h"
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

// Base class which all constraints should implement. Constraint refers to an object
// that would return true/false based on whether the given assignment is valid/not.
class BaseConstraint {
public:
    virtual bool is_valid(const AssignmentMap& assignment) = 0;
    virtual std::unordered_set<std::string> get_synonyms() const = 0;

    bool can_validate(std::unordered_set<std::string>& synonyms) const;

protected:
    static int get_statement_id(const AssignmentMap& assignments,
                                Parser::StatementRef& statement_ref);
    static std::string get_variable_name(const AssignmentMap& assignments,
                                         Parser::EntityRef& entity_ref);
};
} // namespace QueryEvaluator
