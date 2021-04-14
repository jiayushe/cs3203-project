#include "ForwardCheckingSolver.h"

using namespace QueryEvaluator;

AssignmentMapSet ForwardCheckingSolver::solve(const std::unordered_set<std::string>& targets,
                                              DomainMap& domain_map,
                                              ConstraintMap& constraint_map) {

    AssignmentMapSet results;
    AssignmentMap empty_assignment_map;
    std::unordered_set<std::string> unassigned_synonyms;
    for (const auto& entry : domain_map) {
        unassigned_synonyms.insert(entry.first);
    }
    search(results, empty_assignment_map, unassigned_synonyms, targets, domain_map, constraint_map);
    return results;
}

bool ForwardCheckingSolver::search(AssignmentMapSet& results, AssignmentMap& assignment_map,
                                   std::unordered_set<std::string>& unassigned_synonyms,
                                   const std::unordered_set<std::string>& targets,
                                   DomainMap& domain_map, ConstraintMap& constraint_map) {

    auto synonym =
        choose_unassigned_synonym(unassigned_synonyms, targets, domain_map, constraint_map);
    unassigned_synonyms.erase(synonym);

    auto can_short = targets.find(synonym) == targets.end();

    auto domain = domain_map[synonym];
    for (auto const& assignment : domain) {
        assignment_map[synonym] = assignment;

        if (unassigned_synonyms.empty()) {
            results.insert(pick(assignment_map, targets));
            if (can_short) {
                unassigned_synonyms.insert(synonym);
                return true;
            }
            continue;
        }

        auto invalidated_domain_map = invalidate_domain_map(
            synonym, assignment, unassigned_synonyms, domain_map, constraint_map);

        auto is_shorted = search(results, assignment_map, unassigned_synonyms, targets, domain_map,
                                 constraint_map);

        restore_domain_map(domain_map, invalidated_domain_map);

        if (is_shorted && can_short) {
            unassigned_synonyms.insert(synonym);
            return true;
        }
    }

    unassigned_synonyms.insert(synonym);

    return false;
}

// Create a global random uniform distribution to be used in choose_unassigned_synonym. Ideally, we
// shouldn't be using a global instance yet this is done as such for optimisation purposes.
auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<int> distribution(0, 9);

std::string ForwardCheckingSolver::choose_unassigned_synonym(
    const std::unordered_set<std::string>& unassigned_synonyms,
    const std::unordered_set<std::string>& targets, DomainMap& domain_map,
    ConstraintMap& constraint_map) {

    std::string min_synonym;
    double min_score = 1e9;

    for (const auto& synonym : unassigned_synonyms) {
        auto score =
            score_synonym(synonym, unassigned_synonyms, targets, domain_map, constraint_map);
        if (score < min_score) {
            min_synonym = synonym;
            min_score = score;
        } else if (score == min_score && distribution(generator) >= 5) {
            min_synonym = synonym;
            min_score = score;
        }
    }

    return min_synonym;
}

double
ForwardCheckingSolver::score_synonym(const std::string& synonym,
                                     const std::unordered_set<std::string>& unassigned_synonyms,
                                     const std::unordered_set<std::string>& targets,
                                     DomainMap& domain_map, ConstraintMap& constraint_map) {

    double score = domain_map[synonym].size();

    int num_remaining_constraints = 0;
    for (auto const& constraint : constraint_map[synonym]) {
        auto constraint_synonyms = constraint.get_synonyms();
        constraint_synonyms.erase(synonym);
        auto other_synonym = *(constraint_synonyms.begin());
        if (unassigned_synonyms.find(other_synonym) != unassigned_synonyms.end()) {
            num_remaining_constraints += 1;
        }
    }
    score /= (3.5 * num_remaining_constraints) + 1;

    if (targets.find(synonym) == targets.end()) {
        score *= 2;
    }

    return score;
}

// Create a global AssignmentMap instance that is frequently used in invalidate_domain_map.
// Ideally, this amounts to a bad coding practice, but we do it anyway because the performance
// benefit is significant (as opposed to creating a new AssignmentMap instance at every run).
AssignmentMap binary_assignment_map;

DomainMap ForwardCheckingSolver::invalidate_domain_map(
    const std::string& synonym, const Assignment& assignment,
    const std::unordered_set<std::string>& unassigned_synonyms, DomainMap& domain_map,
    ConstraintMap& constraint_map) {

    DomainMap invalidated_domain_map;

    for (auto const& constraint : constraint_map[synonym]) {
        auto constraint_synonyms = constraint.get_synonyms();
        constraint_synonyms.erase(synonym);
        auto other_synonym = *(constraint_synonyms.begin());
        if (unassigned_synonyms.find(other_synonym) == unassigned_synonyms.end()) {
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
