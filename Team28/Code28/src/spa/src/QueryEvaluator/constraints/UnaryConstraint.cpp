#include "UnaryConstraint.h"

using namespace QueryEvaluator;

UnaryConstraint::UnaryConstraint(std::shared_ptr<BaseConstraintLogic> constraint_logic) {
    auto synonyms = constraint_logic->get_synonyms();
    if (synonyms.size() != 1) {
        throw std::runtime_error("Unary constraint must involve exactly 1 synonym");
    }
    this->constraint_logic = constraint_logic;
}

bool UnaryConstraint::is_valid(const Assignment& assignment) const {
    auto synonym = get_synonym();
    AssignmentMap unary_assignment{{synonym, assignment}};
    return constraint_logic->is_valid(unary_assignment);
}

std::string UnaryConstraint::get_synonym() const {
    return *(constraint_logic->get_synonyms().begin());
}
