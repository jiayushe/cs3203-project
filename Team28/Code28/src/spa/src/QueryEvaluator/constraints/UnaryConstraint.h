#pragma once

#include "BaseConstraintLogic.h"
#include "QueryEvaluator/Assignment.h"
#include <memory>
#include <stdexcept>

namespace QueryEvaluator {

// A constraint which involves one synonym.
// For instance, Follows(s, 10) forms a unary constraint.
class UnaryConstraint {
public:
    explicit UnaryConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic);

    bool is_valid(const Assignment& assignment) const;
    std::string get_synonym() const;

private:
    std::shared_ptr<BaseConstraintLogic> constraint_logic;
};

} // namespace QueryEvaluator
