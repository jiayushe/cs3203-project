#include "BruteForceEvaluator.h"

using namespace QueryEvaluator;

void BruteForceEvaluator::evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   Parser::QueryObject& query_object,
                                   std::list<std::string>& results) {

    // Compute all assignment groups + constraints for the given query_object
    auto constraints = ConstraintUtils::get_constraints(pkb, query_object);
    auto assignment_groups = get_assignment_groups(pkb, query_object);

    // Narrow down the assignments in the assignment groups via the computed constraints
    std::vector<std::pair<std::unordered_set<std::string>, std::vector<AssignmentMap>>>
        constrained_assignment_groups;
    for (const auto& assignment_group : assignment_groups) {
        auto curr_synonyms = assignment_group.first;
        auto curr_assignments = assignment_group.second;

        // Filter out constraints which are not applicable to the current assignment group
        std::vector<std::shared_ptr<BaseConstraint>> curr_constraints;
        for (auto const& constraint : constraints) {
            if (constraint->can_validate(curr_synonyms)) {
                curr_constraints.push_back(constraint);
            }
        }

        // Filter out assignments which do not satisfy the constraints
        std::vector<AssignmentMap> curr_constrained_assignments;
        for (auto const& curr_assignment : curr_assignments) {
            if (std::all_of(curr_constraints.begin(), curr_constraints.end(),
                            [&](std::shared_ptr<BaseConstraint> constraint) {
                                return constraint->is_valid(curr_assignment);
                            })) {
                curr_constrained_assignments.push_back(curr_assignment);
            }
        }

        // If there is no satisfying assignment, we should return an empty result
        if (curr_constrained_assignments.empty()) {
            return;
        }

        constrained_assignment_groups.emplace_back(curr_synonyms, curr_constrained_assignments);
    }

    // Find the assignment group containing the selection synonym
    auto selection_synonym = query_object.get_selection();
    auto selection_assignment_group =
        std::find_if(constrained_assignment_groups.begin(), constrained_assignment_groups.end(),
                     [&](std::pair<std::unordered_set<std::string>, std::vector<AssignmentMap>>
                             assignment_group) {
                         auto synonyms = assignment_group.first;
                         return synonyms.find(selection_synonym) != synonyms.end();
                     });
    if (selection_assignment_group == constrained_assignment_groups.end()) {
        // This should never happen
        throw "Selection not found in any of the assignment groups";
    }

    // Get and dedupe the selection results
    std::unordered_set<std::string> unique_selections;
    for (const auto& assignment : selection_assignment_group->second) {
        auto selection = assignment.at(selection_synonym);
        unique_selections.insert(selection.value_as_string());
    }
    for (const auto& selection : unique_selections) {
        results.push_back(selection);
    }
}

std::vector<std::pair<std::unordered_set<std::string>, std::vector<AssignmentMap>>>
BruteForceEvaluator::get_assignment_groups(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                           Parser::QueryObject& query_object) {

    auto design_entities = get_design_entities(query_object);
    auto dependencies = get_dependencies(query_object);

    // Find all the connected components (see graph theory) in the dependency graph.
    // Each of these connected components would form an assignment group.
    std::unordered_set<std::string> synonyms;
    for (auto const& entry : design_entities) {
        synonyms.insert(entry.first);
    }
    std::vector<std::pair<std::unordered_set<std::string>, std::vector<AssignmentMap>>>
        assignment_groups;
    while (!synonyms.empty()) {
        std::unordered_set<std::string> curr_synonyms;

        // Do a BFS over the dependency graph to find the connected components
        std::list<std::string> queue;
        queue.push_back(*synonyms.begin());
        while (!queue.empty()) {
            auto curr_synonym = queue.front();
            queue.pop_front();

            if (curr_synonyms.find(curr_synonym) != curr_synonyms.end()) {
                continue;
            }
            curr_synonyms.insert(curr_synonym);
            synonyms.erase(curr_synonym);

            for (auto const& dependency : dependencies[curr_synonym]) {
                queue.push_back(dependency);
            }
        }

        // Once the connected components are found, compute the assignments for
        // the current connected components and create a new assignment group
        std::vector<Parser::DesignEntity> curr_design_entities;
        for (auto const& curr_synonym : curr_synonyms) {
            curr_design_entities.push_back(design_entities[curr_synonym]);
        }
        auto curr_assignments = get_assignments(pkb, curr_design_entities);
        assignment_groups.emplace_back(curr_synonyms, curr_assignments);
    }

    return assignment_groups;
}

std::vector<AssignmentMap>
BruteForceEvaluator::get_assignments(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                     std::vector<Parser::DesignEntity>& design_entities) {

    std::list<AssignmentMap> assignments;
    assignments.emplace_back();

    for (auto const& design_entity : design_entities) {
        auto synonym = design_entity.get_synonym();
        auto domain = DomainUtils::get_domain(pkb, design_entity.get_type());

        if (domain.empty()) {
            return std::vector<AssignmentMap>();
        }

        std::list<AssignmentMap> new_assignments;
        while (!assignments.empty()) {
            auto assign_map = assignments.front();
            assignments.pop_front();

            for (const auto& value : domain) {
                AssignmentMap new_assign_map(assign_map);
                new_assign_map[synonym] = value;
                new_assignments.push_back(new_assign_map);
            }
        }

        assignments = new_assignments;
    }

    return std::vector<AssignmentMap>(assignments.begin(), assignments.end());
}

std::unordered_map<std::string, Parser::DesignEntity>
BruteForceEvaluator::get_design_entities(Parser::QueryObject& query_object) {
    auto declarations = query_object.get_declarations();

    std::unordered_map<std::string, Parser::DesignEntity> design_entities;

    auto selection_synonym = query_object.get_selection();
    auto selection_design_entity = declarations.get(selection_synonym);
    design_entities[selection_synonym] = selection_design_entity;

    if (query_object.has_such_that()) {
        auto such_that = query_object.get_such_that();

        auto left_ref = such_that.get_left_ref();
        if (has_synonym(left_ref)) {
            auto left_ref_synonym = get_synonym(left_ref);
            auto left_ref_design_entity = declarations.get(left_ref_synonym);
            design_entities[left_ref_synonym] = left_ref_design_entity;
        }

        auto right_ref = such_that.get_right_ref();
        if (has_synonym(right_ref)) {
            auto right_ref_synonym = get_synonym(right_ref);
            auto right_ref_design_entity = declarations.get(right_ref_synonym);
            design_entities[right_ref_synonym] = right_ref_design_entity;
        }
    }

    if (query_object.has_pattern()) {
        auto pattern = query_object.get_pattern();

        auto assign_synonym = pattern.get_assigned_synonym();
        auto assign_design_entity = declarations.get(assign_synonym);
        design_entities[assign_synonym] = assign_design_entity;

        auto entity_ref = pattern.get_entity_ref();
        if (has_synonym(entity_ref)) {
            auto entity_ref_synonym = get_synonym(entity_ref);
            auto entity_ref_design_entity = declarations.get(entity_ref_synonym);
            design_entities[entity_ref_synonym] = entity_ref_design_entity;
        }
    }

    return design_entities;
}

std::unordered_map<std::string, std::vector<std::string>>
BruteForceEvaluator::get_dependencies(Parser::QueryObject& query_object) {
    std::unordered_map<std::string, std::vector<std::string>> dependencies;

    if (query_object.has_such_that()) {
        auto such_that = query_object.get_such_that();
        auto left_ref = such_that.get_left_ref();
        auto right_ref = such_that.get_right_ref();
        if (has_synonym(left_ref) && has_synonym(right_ref)) {
            auto left_ref_synonym = get_synonym(left_ref);
            auto right_ref_synonym = get_synonym(right_ref);
            dependencies[left_ref_synonym].push_back(right_ref_synonym);
            dependencies[right_ref_synonym].push_back(left_ref_synonym);
        }
    }

    if (query_object.has_pattern()) {
        auto pattern = query_object.get_pattern();
        auto assign_synonym = pattern.get_assigned_synonym();
        auto entity_ref = pattern.get_entity_ref();
        if (has_synonym(entity_ref)) {
            auto entity_ref_synonym = get_synonym(entity_ref);
            dependencies[assign_synonym].push_back(entity_ref_synonym);
            dependencies[entity_ref_synonym].push_back(assign_synonym);
        }
    }

    return dependencies;
}

bool BruteForceEvaluator::has_synonym(Parser::Ref& ref) {
    switch (ref.get_type()) {
    case Parser::RefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return has_synonym(entity_ref);
    }
    case Parser::RefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return has_synonym(statement_ref);
    }
    default:
        throw "Unknown ref type";
    }
}

bool BruteForceEvaluator::has_synonym(Parser::EntityRef& entity_ref) {
    return entity_ref.get_type() == Parser::EntityRefType::SYNONYM;
}

bool BruteForceEvaluator::has_synonym(Parser::StatementRef& statement_ref) {
    return statement_ref.get_type() == Parser::StatementRefType::SYNONYM;
}

std::string BruteForceEvaluator::get_synonym(Parser::Ref& ref) {
    switch (ref.get_type()) {
    case Parser::RefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return get_synonym(entity_ref);
    }
    case Parser::RefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return get_synonym(statement_ref);
    }
    default:
        throw "Unknown ref type";
    }
}

std::string BruteForceEvaluator::get_synonym(Parser::EntityRef& entity_ref) {
    if (!has_synonym(entity_ref)) {
        throw "No synonym in entity ref";
    }
    return entity_ref.get_synonym();
}

std::string BruteForceEvaluator::get_synonym(Parser::StatementRef& statement_ref) {
    if (!has_synonym(statement_ref)) {
        throw "No synonym in statement ref";
    }
    return statement_ref.get_synonym();
}
