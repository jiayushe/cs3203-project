#include "EmptyConstraint.h"

using namespace QueryEvaluator;

EmptyConstraint::EmptyConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic) {
    auto synonyms = constraint_logic->get_synonyms();
    if (synonyms.size() != 0) {
        throw std::runtime_error("Empty constraint must not involve any synonym");
    }
    this->constraint_logic = constraint_logic;
}

bool EmptyConstraint::is_valid() const { return constraint_logic->is_valid(AssignmentMap()); }
