#pragma once

#include "Common.h"
#include "constraints/BinaryConstraint.h"
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
    static AssignmentMaps solve(const std::unordered_set<std::string>& targets,
                                DomainMap domain_map,
                                const std::vector<BinaryConstraint>& constraints);

private:
    // Similar to `solve`, `search` is the main constraint solving routine (in fact `solve` simply
    // wraps around `search`). The algorithm implemented is forward-checking backtracking search.
    static bool search(AssignmentMaps& results, AssignmentMap& assignment_map,
                       const std::unordered_set<std::string>& targets, DomainMap& domain_map,
                       const std::vector<BinaryConstraint>& constraints);

    // Choose a synonym to evaluate next during the search process. A good choice heuristic will
    // speed up the constraint solving process significantly.
    static std::string choose_unassigned_synonym(const std::unordered_set<std::string>& targets,
                                                 const AssignmentMap& assignment_map,
                                                 const DomainMap& domain_map);

    // Execute the forward-checking step, i.e. remove all values in the domain which are
    // inconsistent with the current assignment.
    static DomainMap remove_invalid_domains(const std::string& synonym,
                                            AssignmentMap& assignment_map, DomainMap& domain_map,
                                            const std::vector<BinaryConstraint>& constraints);

    // Part of the backtracking step, i.e. used to restore all domains that were removed when
    // `remove_invalid_domains` is called.
    static void merge_domains(DomainMap& dest_domain_map, const DomainMap& src_domain_map);

    // Check whether all synonyms have been assigned (indicate whether we've found a consistent
    // assignment).
    static bool are_all_synonyms_assigned(const AssignmentMap& assignment_map,
                                          const DomainMap& domain_map);

    // Helper to construct an assignment map consisting of the given keys.
    static AssignmentMap pick(const AssignmentMap& assignment_map,
                              const std::unordered_set<std::string>& keys);
};

} // namespace QueryEvaluator
