#pragma once

#include "Common.h"
#include "Profiler.h"
#include "constraints/BinaryConstraint.h"
#include <chrono>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace QueryEvaluator {

// A forward-checking constraint solver implementation.
// References:
// - http://www.cs.toronto.edu/~fbacchus/Papers/liu.pdf
// - http://pages.cs.wisc.edu/~bgibson/cs540/handouts/csp.pdf
class ForwardCheckingSolver {
public:
    // Main constraint solving routine. Returns a set of assignments (containing the synonyms
    // in `targets`) which are consistent with the constraints given.
    //
    // Parameters:
    // - targets: A set of synonyms that we are solving for.
    // - domain_map: Domain for each synonym.
    // - constraints: A list of constraints involving the synonyms.
    //
    // Suppose we have the following inputs:
    // - targets = {"a", "b"}
    // - domain_map = {"a": [1, 2, 3], "b": [1, 2, 3], "c": [1, 2, 3]}
    // - constraints = [AB(1, 2), AB(1, 3), BC(2, 2), BC(2, 3)] (suppose AB(A, B) is consistent if
    //   a = A and b = B and BC(B, C) is consistent if b = B and c = C
    //
    // Then, we expect the output to be {{"a": 1, "b": 2}}.
    static AssignmentMapSet solve(const std::unordered_set<std::string>& targets,
                                  DomainMap& domain_map, ConstraintMap& constraint_map);

private:
    // Similar to `solve`, `search` is the main constraint solving routine (in fact `solve` simply
    // wraps around `search`). The algorithm implemented is forward-checking backtracking search.
    static bool search(AssignmentMapSet& results, AssignmentMap& assignment_map,
                       std::unordered_set<std::string>& unassigned_synonyms,
                       const std::unordered_set<std::string>& targets, DomainMap& domain_map,
                       ConstraintMap& constraint_map);

    // Choose a synonym to evaluate next during the search process. A good choice heuristic will
    // speed up the constraint solving process significantly.
    static std::string
    choose_unassigned_synonym(const std::unordered_set<std::string>& unassigned_synonyms,
                              const std::unordered_set<std::string>& targets, DomainMap& domain_map,
                              ConstraintMap& constraint_map);
    static double score_synonym(const std::string& synonym,
                                const std::unordered_set<std::string>& unassigned_synonyms,
                                const std::unordered_set<std::string>& targets,
                                DomainMap& domain_map, ConstraintMap& constraint_map);

    // Execute the forward-checking step, i.e. invalidate all values in the domain which are
    // inconsistent with the current assignment.
    static DomainMap
    invalidate_domain_map(const std::string& synonym, const Assignment& assignment,
                          const std::unordered_set<std::string>& unassigned_synonyms,
                          DomainMap& domain_map, ConstraintMap& constraint_map);

    // Part of the backtracking step, i.e. used to restore all domains that were invalidated when
    // `invalidate_domain_map` is called.
    static void restore_domain_map(DomainMap& domain_map, const DomainMap& invalidated_domain_map);

    // Helper to construct an assignment map consisting of the given keys.
    static AssignmentMap pick(const AssignmentMap& assignment_map,
                              const std::unordered_set<std::string>& keys);
};

} // namespace QueryEvaluator
