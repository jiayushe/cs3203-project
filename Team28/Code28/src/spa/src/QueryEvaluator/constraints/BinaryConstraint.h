#pragma once

#include "BaseConstraintLogic.h"
#include "QueryEvaluator/Common.h"
#include <memory>
#include <stdexcept>
#include <unordered_set>

namespace QueryEvaluator {

// A constraint which involve two synonyms.
// For instance, Follows(a, b) forms a binary constraint.
class BinaryConstraint {
public:
    explicit BinaryConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic);

    bool is_valid(const AssignmentMap& assignment) const;
    std::unordered_set<std::string> get_synonyms() const;

private:
    std::shared_ptr<BaseConstraintLogic> constraint_logic;
};

} // namespace QueryEvaluator
