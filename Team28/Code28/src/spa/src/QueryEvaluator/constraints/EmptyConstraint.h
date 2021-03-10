#pragma once

#include "BaseConstraintLogic.h"
#include "QueryEvaluator/Common.h"
#include <memory>
#include <stdexcept>

namespace QueryEvaluator {

// A constraint which involve zero synonyms.
// For instance, Follows(1, 2) forms an empty constraint.
class EmptyConstraint {
public:
    explicit EmptyConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic);

    bool is_valid() const;

private:
    std::shared_ptr<BaseConstraintLogic> constraint_logic;
};

} // namespace QueryEvaluator
