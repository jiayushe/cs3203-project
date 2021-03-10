#include "ForwardCheckingSolver.h"

using namespace QueryEvaluator;

AssignmentMaps ForwardCheckingSolver::solve(const std::unordered_set<std::string>& targets,
                                            DomainMap domain_map,
                                            const std::vector<BinaryConstraint>& constraints) {

    AssignmentMaps results;
    AssignmentMap empty_assignment_map;
    std::unordered_set<std::string> remaining_targets;
    search(results, empty_assignment_map, targets, remaining_targets, domain_map, constraints);
    return results;
}

bool ForwardCheckingSolver::search(AssignmentMaps& results, AssignmentMap& assignment_map,
                                   const std::unordered_set<std::string>& targets,
                                   std::unordered_set<std::string>& remaining_targets,
                                   DomainMap& domain_map,
                                   const std::vector<BinaryConstraint>& constraints) {

    auto synonym = get_unassigned_synonym(targets, assignment_map, domain_map);
    remaining_targets.erase(synonym);

    auto is_target = targets.find(synonym) != targets.end();
    auto can_short = remaining_targets.empty() && !is_target;

    auto domain = domain_map[synonym];
    for (auto const& assignment : domain) {
        assignment_map[synonym] = assignment;

        if (are_all_synonyms_assigned(assignment_map, domain_map)) {
            results.insert(pick(assignment_map, targets));
            assignment_map.erase(synonym);
            if (can_short) {
                return true;
            }
            continue;
        }

        auto removed_domain_map =
            remove_invalid_domains(synonym, assignment_map, domain_map, constraints);

        auto is_shorted =
            search(results, assignment_map, targets, remaining_targets, domain_map, constraints);

        assignment_map.erase(synonym);
        merge_domains(domain_map, removed_domain_map);

        if (is_shorted && can_short) {
            return true;
        }
    }

    if (is_target) {
        remaining_targets.insert(synonym);
    }

    return false;
}

std::string
ForwardCheckingSolver::get_unassigned_synonym(const std::unordered_set<std::string>& targets,
                                              const AssignmentMap& assignment_map,
                                              const DomainMap& domain_map) {

    std::string min_synonym = "";
    auto min_domain_size = (int)1e9;
    auto min_is_target = false;

    for (auto const& entry : domain_map) {
        auto synonym = entry.first;
        auto domain = entry.second;

        // TODO: Verify empirically whether it is better to always choose target over non-target
        auto is_target = targets.find(synonym) != targets.end();
        auto is_unassigned = assignment_map.find(synonym) == assignment_map.end();
        auto is_domain_smaller = domain.size() < min_domain_size;
        auto is_better = is_unassigned && ((is_target && !min_is_target) ||
                                           (is_target && min_is_target && is_domain_smaller) ||
                                           (!is_target && !min_is_target && is_domain_smaller));

        if (is_better) {
            min_synonym = synonym;
            min_domain_size = domain.size();
            min_is_target = is_target;
        }
    }

    return min_synonym;
}

DomainMap
ForwardCheckingSolver::remove_invalid_domains(const std::string& synonym,
                                              AssignmentMap& assignment_map, DomainMap& domain_map,
                                              const std::vector<BinaryConstraint>& constraints) {

    DomainMap removed_domain_map;

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
            AssignmentMap binary_assignment;
            binary_assignment[synonym] = assignment;
            binary_assignment[other_synonym] = other_assignment;
            if (!constraint.is_valid(binary_assignment)) {
                domain_map[other_synonym].erase(other_assignment);
                removed_domain_map[other_synonym].insert(other_assignment);
            }
        }
    }

    return removed_domain_map;
}

void ForwardCheckingSolver::merge_domains(DomainMap& dest, const DomainMap& src) {
    for (auto const& entry : src) {
        auto synonym = entry.first;
        auto domain = entry.second;
        for (auto const& assignment : domain) {
            dest[synonym].insert(assignment);
        }
    }
}

bool ForwardCheckingSolver::are_all_synonyms_assigned(const AssignmentMap& assignments,
                                                      const DomainMap& domains) {
    return assignments.size() == domains.size();
}

AssignmentMap ForwardCheckingSolver::pick(const AssignmentMap& assignment_map,
                                          const std::unordered_set<std::string>& keys) {
    AssignmentMap picked;
    for (auto const& key : keys) {
        picked[key] = assignment_map.at(key);
    }
    return picked;
}
