#include "ForwardCheckingSolver.h"

using namespace QueryEvaluator;

AssignmentMapSet ForwardCheckingSolver::solve(const std::unordered_set<std::string>& targets,
                                              DomainMap domain_map,
                                              const std::vector<BinaryConstraint>& constraints) {

    AssignmentMapSet results;
    AssignmentMap empty_assignment_map;
    search(results, empty_assignment_map, targets, domain_map, constraints);
    return results;
}

bool ForwardCheckingSolver::search(AssignmentMapSet& results, AssignmentMap& assignment_map,
                                   const std::unordered_set<std::string>& targets,
                                   DomainMap& domain_map,
                                   const std::vector<BinaryConstraint>& constraints) {

    auto synonym = choose_unassigned_synonym(targets, assignment_map, domain_map);
    auto can_short = targets.find(synonym) == targets.end();

    auto domain = domain_map[synonym];
    for (auto const& assignment : domain) {
        assignment_map[synonym] = assignment;

        if (assignment_map.size() == domain_map.size()) {
            results.insert(pick(assignment_map, targets));
            assignment_map.erase(synonym);
            if (can_short) {
                return true;
            }
            continue;
        }

        auto invalidated_domain_map =
            invalidate_domain_map(synonym, assignment_map, constraints, domain_map);

        auto is_shorted = search(results, assignment_map, targets, domain_map, constraints);

        assignment_map.erase(synonym);
        restore_domain_map(domain_map, invalidated_domain_map);

        if (is_shorted && can_short) {
            return true;
        }
    }

    return false;
}

std::string
ForwardCheckingSolver::choose_unassigned_synonym(const std::unordered_set<std::string>& targets,
                                                 const AssignmentMap& assignment_map,
                                                 const DomainMap& domain_map) {

    std::string min_synonym;
    auto min_domain_size = (int)1e9;

    for (auto const& entry : domain_map) {
        auto is_unassigned = assignment_map.find(entry.first) == assignment_map.end();
        auto is_domain_smaller = entry.second.size() < min_domain_size;
        if (is_unassigned && is_domain_smaller) {
            min_synonym = entry.first;
            min_domain_size = entry.second.size();
        }
    }

    return min_synonym;
}

// Create a global AssignmentMap instance that is frequently used in invalidate_domain_map.
// Ideally, this amounts to a bad coding practice, but we do it anyway because the performance
// benefit is significant (as opposed to creating a new AssignmentMap instance at every run).
AssignmentMap binary_assignment_map;

DomainMap ForwardCheckingSolver::invalidate_domain_map(
    const std::string& synonym, AssignmentMap& assignment_map,
    const std::vector<BinaryConstraint>& constraints, DomainMap& domain_map) {

    DomainMap invalidated_domain_map;

    auto assignment = assignment_map[synonym];
    for (auto const& constraint : constraints) {
        auto constraint_synonyms = constraint.get_synonyms();
        if (constraint_synonyms.find(synonym) == constraint_synonyms.end()) {
            continue;
        }

        constraint_synonyms.erase(synonym);
        auto other_synonym = *(constraint_synonyms.begin());
        if (assignment_map.find(other_synonym) != assignment_map.end()) {
            continue;
        }

        auto other_domain = domain_map[other_synonym];
        for (auto const& other_assignment : other_domain) {
            binary_assignment_map[synonym] = assignment;
            binary_assignment_map[other_synonym] = other_assignment;
            if (!constraint.is_valid(binary_assignment_map)) {
                domain_map[other_synonym].erase(other_assignment);
                invalidated_domain_map[other_synonym].insert(other_assignment);
            }
        }
    }

    return invalidated_domain_map;
}

void ForwardCheckingSolver::restore_domain_map(DomainMap& domain_map,
                                               const DomainMap& invalidated_domain_map) {
    for (auto const& entry : invalidated_domain_map) {
        for (auto const& assignment : entry.second) {
            domain_map[entry.first].insert(assignment);
        }
    }
}

AssignmentMap ForwardCheckingSolver::pick(const AssignmentMap& assignment_map,
                                          const std::unordered_set<std::string>& keys) {
    AssignmentMap picked;
    for (auto const& key : keys) {
        picked[key] = assignment_map.at(key);
    }
    return picked;
}
