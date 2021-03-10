#include "BinaryConstraint.h"

using namespace QueryEvaluator;

BinaryConstraint::BinaryConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic) {
    auto synonyms = constraint_logic->get_synonyms();
    if (synonyms.size() != 2) {
        throw std::runtime_error("Binary constraint must involve exactly 2 synonyms");
    }
    this->constraint_logic = constraint_logic;
}

bool BinaryConstraint::is_valid(const AssignmentMap& assignment) const {
    return constraint_logic->is_valid(assignment);
}

std::unordered_set<std::string> BinaryConstraint::get_synonyms() const {
    return constraint_logic->get_synonyms();
}
