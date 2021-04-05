#include "Evaluator.h"

using namespace QueryEvaluator;

void Evaluator::evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb,
                         const Parser::QueryObject& query_object, std::list<std::string>& output) {

    auto result = query_object.get_result();

    auto empty_constraints = ConstraintUtils::get_empty_constraints(pkb, query_object);
    for (auto const& empty_constraint : empty_constraints) {
        if (!empty_constraint.is_valid()) {
            switch (result.get_type()) {
            case Parser::ResultType::BOOLEAN:
                output.emplace_back("FALSE");
                return;
            case Parser::ResultType::TUPLE:
                return;
            default:
                throw std::runtime_error("Unknown result type");
            }
        }
    }

    auto assignment_groups = get_assignment_groups(pkb, query_object);
    if (assignment_groups.empty()) {
        // This happens when result type = BOOLEAN and either (1) all empty
        // constraints pass or (2) there are no constraints at all
        switch (result.get_type()) {
        case Parser::ResultType::BOOLEAN:
            output.emplace_back("TRUE");
            return;
        case Parser::ResultType::TUPLE:
            throw std::runtime_error("This should never happen");
        default:
            throw std::runtime_error("Unknown result type");
        }
    }

    std::vector<AssignmentMapSet> assignment_map_sets;
    while (!assignment_groups.empty()) {
        auto assignment_group = choose_assignment_group(assignment_groups);
        auto targets = std::get<0>(assignment_group);
        auto domain_map = std::get<1>(assignment_group);
        auto constraints = std::get<2>(assignment_group);

        auto assignment_map_set = ForwardCheckingSolver::solve(targets, domain_map, constraints);
        if (assignment_map_set.empty()) {
            // Terminate early if any group yields an empty result
            // (no consistent assignment for the group)
            switch (result.get_type()) {
            case Parser::ResultType::BOOLEAN:
                output.emplace_back("FALSE");
                return;
            case Parser::ResultType::TUPLE:
                return;
            default:
                throw std::runtime_error("Unknown result type");
            }
        }

        assignment_map_sets.push_back(assignment_map_set);
    }

    auto merged_assignment_maps = merge_assignment_map_sets(assignment_map_sets);
    auto formatted_output = get_formatted_output(query_object.get_result(), merged_assignment_maps);
    output.insert(output.end(), formatted_output.begin(), formatted_output.end());
}

std::vector<AssignmentGroup>
Evaluator::get_assignment_groups(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                 const Parser::QueryObject& query_object) {

    auto design_entity_map = get_design_entity_map(query_object);
    auto dependency_map = get_dependency_map(query_object);
    auto result_synonyms = get_synonyms(query_object.get_result());

    // Find all the connected components (see graph theory) in the dependency graph.
    // Each of these connected components would form an assignment group.
    std::vector<AssignmentGroup> assignment_groups;
    std::unordered_set<std::string> synonyms;
    for (auto const& entry : design_entity_map) {
        synonyms.insert(entry.first);
    }
    while (!synonyms.empty()) {
        auto connected_synonyms = get_connected_synonyms(*synonyms.begin(), dependency_map);
        for (const auto& synonym : connected_synonyms) {
            synonyms.erase(synonym);
        }

        auto constraints = ConstraintUtils::get_constraints(pkb, query_object, connected_synonyms);
        auto unary_constraints = constraints.first;
        auto binary_constraints = constraints.second;

        DomainMap domains;
        for (const auto& synonym : connected_synonyms) {
            auto design_entity = design_entity_map[synonym];

            std::vector<UnaryConstraint> domain_constraints;
            for (const auto& unary_constraint : unary_constraints) {
                if (unary_constraint.get_synonym() == synonym) {
                    domain_constraints.push_back(unary_constraint);
                }
            }

            domains[synonym] =
                DomainUtils::get_domain(pkb, design_entity.get_type(), domain_constraints);
        }

        std::unordered_set<std::string> targets;
        for (auto const& synonym : connected_synonyms) {
            if (result_synonyms.find(synonym) != result_synonyms.end()) {
                targets.insert(synonym);
            }
        }

        assignment_groups.emplace_back(targets, domains, binary_constraints);
    }

    return assignment_groups;
}

AssignmentGroup
Evaluator::choose_assignment_group(std::vector<AssignmentGroup>& assignment_groups) {
    if (assignment_groups.empty()) {
        throw std::runtime_error("Assignment group vector must not be empty");
    }

    auto min_index = 0;
    auto min_score = score_assignment_group(assignment_groups[0]);
    for (auto i = 1; i < assignment_groups.size(); i++) {
        auto curr_score = score_assignment_group(assignment_groups[i]);
        if (curr_score < min_score) {
            min_index = i;
            min_score = curr_score;
        }
    }

    auto chosen = assignment_groups[min_index];
    assignment_groups.erase(assignment_groups.begin() + min_index);

    return chosen;
}

long Evaluator::score_assignment_group(const AssignmentGroup& assignment_group) {
    // We score assignment groups based on the expected number of steps our constraint solver
    // component needs to take before terminating. This can be approximated by computing the
    // product of the size of the domains in the assignment group.
    auto score = 1L;
    auto targets = std::get<0>(assignment_group);
    auto domain_map = std::get<1>(assignment_group);
    for (auto const& entry : domain_map) {
        if (targets.find(entry.first) != targets.end()) {
            score *= entry.second.size();
        }
    }
    return score;
}

std::unordered_set<std::string>
Evaluator::get_connected_synonyms(const std::string& starting_synonym,
                                  DependencyMap& dependency_map) {

    std::unordered_set<std::string> connected_synonyms;

    std::list<std::string> queue;
    queue.push_back(starting_synonym);
    while (!queue.empty()) {
        auto curr = queue.front();
        queue.pop_front();

        if (connected_synonyms.find(curr) != connected_synonyms.end()) {
            continue;
        }
        connected_synonyms.insert(curr);

        for (auto const& dependency : dependency_map[curr]) {
            queue.push_back(dependency);
        }
    }

    return connected_synonyms;
}

DesignEntityMap Evaluator::get_design_entity_map(const Parser::QueryObject& query_object) {
    DesignEntityMap design_entity_map;
    auto declaration_map = query_object.get_declarations();
    auto result = query_object.get_result();
    get_design_entity_map(design_entity_map, declaration_map, result);
    for (auto const& such_that : query_object.get_all_such_that()) {
        get_design_entity_map(design_entity_map, declaration_map, such_that);
    }
    for (auto const& pattern : query_object.get_all_pattern()) {
        get_design_entity_map(design_entity_map, declaration_map, pattern);
    }
    for (auto const& with : query_object.get_all_with()) {
        get_design_entity_map(design_entity_map, declaration_map, with);
    }
    return design_entity_map;
}

void Evaluator::get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Result& result) {

    switch (result.get_type()) {
    case Parser::ResultType::TUPLE:
        for (auto const& elem : result.get_tuple()) {
            get_design_entity_map(design_entity_map, declaration_map, elem);
        }
        break;
    case Parser::ResultType::BOOLEAN:
        break;
    default:
        throw std::runtime_error("Unknown result type");
    }
}

void Evaluator::get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::SuchThat& such_that) {

    auto left_ref = such_that.get_left_ref();
    if (has_synonym(left_ref)) {
        auto left_ref_synonym = get_synonym(left_ref);
        auto left_ref_design_entity = declaration_map.get(left_ref_synonym);
        design_entity_map[left_ref_synonym] = left_ref_design_entity;
    }

    auto right_ref = such_that.get_right_ref();
    if (has_synonym(right_ref)) {
        auto right_ref_synonym = get_synonym(right_ref);
        auto right_ref_design_entity = declaration_map.get(right_ref_synonym);
        design_entity_map[right_ref_synonym] = right_ref_design_entity;
    }
}

void Evaluator::get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Pattern& pattern) {

    std::string pattern_synonym;
    Parser::EntityRef entity_ref;

    switch (pattern.get_type()) {
    case Parser::PatternType::ASSIGN: {
        auto pattern_assign = pattern.get_pattern_assign();
        pattern_synonym = pattern_assign.get_assign_synonym();
        entity_ref = pattern_assign.get_entity_ref();
        break;
    }
    case Parser::PatternType::WHILE: {
        auto pattern_while = pattern.get_pattern_while();
        pattern_synonym = pattern_while.get_while_synonym();
        entity_ref = pattern_while.get_entity_ref();
        break;
    }
    case Parser::PatternType::IF: {
        auto pattern_if = pattern.get_pattern_if();
        pattern_synonym = pattern_if.get_if_synonym();
        entity_ref = pattern_if.get_entity_ref();
        break;
    }
    default:
        throw std::runtime_error("Unknown pattern type");
    }

    auto pattern_design_entity = declaration_map.get(pattern_synonym);
    design_entity_map[pattern_synonym] = pattern_design_entity;

    if (has_synonym(entity_ref)) {
        auto entity_ref_synonym = get_synonym(entity_ref);
        auto entity_ref_design_entity = declaration_map.get(entity_ref_synonym);
        design_entity_map[entity_ref_synonym] = entity_ref_design_entity;
    }
}

void Evaluator::get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::With& with) {

    auto left_ref = with.get_left_ref();
    if (has_synonym(left_ref)) {
        auto left_ref_synonym = get_synonym(left_ref);
        auto left_ref_design_entity = declaration_map.get(left_ref_synonym);
        design_entity_map[left_ref_synonym] = left_ref_design_entity;
    }

    auto right_ref = with.get_right_ref();
    if (has_synonym(right_ref)) {
        auto right_ref_synonym = get_synonym(right_ref);
        auto right_ref_design_entity = declaration_map.get(right_ref_synonym);
        design_entity_map[right_ref_synonym] = right_ref_design_entity;
    }
}

void Evaluator::get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Elem& elem) {

    switch (elem.get_type()) {
    case Parser::ElemType::SYNONYM: {
        auto elem_synonym = elem.get_synonym();
        auto elem_design_entity = declaration_map.get(elem_synonym);
        design_entity_map[elem_synonym] = elem_design_entity;
        break;
    }
    case Parser::ElemType::ATTR_REF: {
        auto attr_ref_synonym = elem.get_attr_ref().get_synonym();
        auto attr_ref_design_entity = declaration_map.get(attr_ref_synonym);
        design_entity_map[attr_ref_synonym] = attr_ref_design_entity;
        break;
    }
    default:
        throw std::runtime_error("Unknown elem type");
    }
}

DependencyMap Evaluator::get_dependency_map(const Parser::QueryObject& query_object) {
    DependencyMap dependency_map;
    for (auto const& such_that : query_object.get_all_such_that()) {
        get_dependency_map(dependency_map, such_that);
    }
    for (auto const& pattern : query_object.get_all_pattern()) {
        get_dependency_map(dependency_map, pattern);
    }
    for (auto const& with : query_object.get_all_with()) {
        get_dependency_map(dependency_map, with);
    }
    return dependency_map;
}

void Evaluator::get_dependency_map(DependencyMap& dependency_map,
                                   const Parser::SuchThat& such_that) {

    auto left_ref = such_that.get_left_ref();
    auto right_ref = such_that.get_right_ref();
    if (has_synonym(left_ref) && has_synonym(right_ref)) {
        auto left_ref_synonym = get_synonym(left_ref);
        auto right_ref_synonym = get_synonym(right_ref);
        dependency_map[left_ref_synonym].push_back(right_ref_synonym);
        dependency_map[right_ref_synonym].push_back(left_ref_synonym);
    }
}

void Evaluator::get_dependency_map(DependencyMap& dependency_map, const Parser::Pattern& pattern) {
    std::string pattern_synonym;
    Parser::EntityRef entity_ref;

    switch (pattern.get_type()) {
    case Parser::PatternType::ASSIGN: {
        auto pattern_assign = pattern.get_pattern_assign();
        pattern_synonym = pattern_assign.get_assign_synonym();
        entity_ref = pattern_assign.get_entity_ref();
        break;
    }
    case Parser::PatternType::WHILE: {
        auto pattern_while = pattern.get_pattern_while();
        pattern_synonym = pattern_while.get_while_synonym();
        entity_ref = pattern_while.get_entity_ref();
        break;
    }
    case Parser::PatternType::IF: {
        auto pattern_if = pattern.get_pattern_if();
        pattern_synonym = pattern_if.get_if_synonym();
        entity_ref = pattern_if.get_entity_ref();
        break;
    }
    default:
        throw std::runtime_error("Unknown pattern type");
    }

    if (has_synonym(entity_ref)) {
        auto entity_ref_synonym = get_synonym(entity_ref);
        dependency_map[pattern_synonym].push_back(entity_ref_synonym);
        dependency_map[entity_ref_synonym].push_back(pattern_synonym);
    }
}

void Evaluator::get_dependency_map(DependencyMap& dependency_map, const Parser::With& with) {
    auto left_ref = with.get_left_ref();
    auto right_ref = with.get_right_ref();
    if (has_synonym(left_ref) && has_synonym(right_ref)) {
        auto left_ref_synonym = get_synonym(left_ref);
        auto right_ref_synonym = get_synonym(right_ref);
        dependency_map[left_ref_synonym].push_back(right_ref_synonym);
        dependency_map[right_ref_synonym].push_back(left_ref_synonym);
    }
}

bool Evaluator::has_synonym(const Parser::SuchThatRef& ref) {
    switch (ref.get_type()) {
    case Parser::SuchThatRefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return has_synonym(entity_ref);
    }
    case Parser::SuchThatRefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return has_synonym(statement_ref);
    }
    default:
        throw std::runtime_error("Unknown such that ref type");
    }
}

bool Evaluator::has_synonym(const Parser::WithRef& ref) {
    return ref.get_type() == Parser::WithRefType::SYNONYM ||
           ref.get_type() == Parser::WithRefType::ATTR_REF;
}

bool Evaluator::has_synonym(const Parser::EntityRef& entity_ref) {
    return entity_ref.get_type() == Parser::EntityRefType::SYNONYM;
}

bool Evaluator::has_synonym(const Parser::StatementRef& statement_ref) {
    return statement_ref.get_type() == Parser::StatementRefType::SYNONYM;
}

std::unordered_set<std::string> Evaluator::get_synonyms(const Parser::Result& result) {
    std::unordered_set<std::string> synonyms;

    switch (result.get_type()) {
    case Parser::ResultType::TUPLE:
        for (auto const& elem : result.get_tuple()) {
            synonyms.insert(get_synonym(elem));
        }
        break;
    case Parser::ResultType::BOOLEAN:
        // Do nothing
        break;
    default:
        throw std::runtime_error("Unknown result type");
    }

    return synonyms;
}

std::string Evaluator::get_synonym(const Parser::Elem& elem) {
    switch (elem.get_type()) {
    case Parser::ElemType::SYNONYM:
        return elem.get_synonym();
    case Parser::ElemType::ATTR_REF:
        return elem.get_attr_ref().get_synonym();
    default:
        throw std::runtime_error("Unknown elem type");
    }
}

std::string Evaluator::get_synonym(const Parser::SuchThatRef& ref) {
    switch (ref.get_type()) {
    case Parser::SuchThatRefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return get_synonym(entity_ref);
    }
    case Parser::SuchThatRefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return get_synonym(statement_ref);
    }
    default:
        throw std::runtime_error("Unknown ref type");
    }
}

std::string Evaluator::get_synonym(const Parser::WithRef& ref) {
    switch (ref.get_type()) {
    case Parser::WithRefType::SYNONYM:
        return ref.get_synonym();
    case Parser::WithRefType::ATTR_REF:
        return ref.get_attr_ref().get_synonym();
    case Parser::WithRefType::NAME:
    case Parser::WithRefType::STATEMENT_ID:
        throw std::runtime_error("No synonym in with ref");
    default:
        throw std::runtime_error("Unknown with ref type");
    }
}

std::string Evaluator::get_synonym(const Parser::EntityRef& entity_ref) {
    if (!has_synonym(entity_ref)) {
        throw std::runtime_error("No synonym in entity ref");
    }
    return entity_ref.get_synonym();
}

std::string Evaluator::get_synonym(const Parser::StatementRef& statement_ref) {
    if (!has_synonym(statement_ref)) {
        throw std::runtime_error("No synonym in statement ref");
    }
    return statement_ref.get_synonym();
}

AssignmentMapVector
Evaluator::merge_assignment_map_sets(const std::vector<AssignmentMapSet>& all_assignment_maps) {
    AssignmentMapVector merged;
    AssignmentMap empty_assignment_map;
    merge_assignment_map_sets(merged, empty_assignment_map, all_assignment_maps, 0);
    return merged;
}

void Evaluator::merge_assignment_map_sets(AssignmentMapVector& results,
                                          AssignmentMap& assignment_map,
                                          const std::vector<AssignmentMapSet>& all_assignment_maps,
                                          int pos) {
    if (pos == all_assignment_maps.size()) {
        results.push_back(assignment_map);
        return;
    }
    for (auto const& new_assignment_map : all_assignment_maps.at(pos)) {
        merge_assignment_map(assignment_map, new_assignment_map);
        merge_assignment_map_sets(results, assignment_map, all_assignment_maps, pos + 1);
    }
}

void Evaluator::merge_assignment_map(AssignmentMap& dest, const AssignmentMap& src) {
    for (auto const& entry : src) {
        dest[entry.first] = entry.second;
    }
}

std::list<std::string> Evaluator::get_formatted_output(const Parser::Result& result,
                                                       const AssignmentMapVector& assignment_maps) {

    std::list<std::string> formatted_output;

    switch (result.get_type()) {
    case Parser::ResultType::TUPLE: {
        auto tuple = result.get_tuple();
        for (auto const& assignment_map : assignment_maps) {
            std::string answer;
            for (auto const& elem : tuple) {
                if (!answer.empty()) {
                    answer += " ";
                }

                switch (elem.get_type()) {
                case Parser::ElemType::ATTR_REF: {
                    auto synonym = elem.get_attr_ref().get_synonym();
                    auto const& assignment = assignment_map.at(synonym);

                    if (elem.get_attr_ref().get_attr_name() == "stmt#" ||
                        elem.get_attr_ref().get_attr_name() == "value") {
                        answer += std::to_string(assignment.get_int_value());
                    } else if (elem.get_attr_ref().get_attr_name() == "procName" ||
                               elem.get_attr_ref().get_attr_name() == "varName") {
                        answer += assignment.get_string_value();
                    } else {
                        throw std::runtime_error("Unknown attribute name");
                    }

                    break;
                }
                case Parser::ElemType::SYNONYM: {
                    auto synonym = elem.get_synonym();
                    auto const& assignment = assignment_map.at(synonym);

                    switch (assignment.get_type()) {
                    case AssignmentType::INTEGER:
                        answer += std::to_string(assignment.get_int_value());
                        break;
                    case AssignmentType::STRING:
                        answer += assignment.get_string_value();
                        break;
                    case AssignmentType::BOTH:
                        answer += std::to_string(assignment.get_int_value());
                        break;
                    default:
                        throw std::runtime_error("Unknown assignment type");
                    }

                    break;
                }
                default:
                    throw std::runtime_error("Unknown elem type");
                }
            }

            formatted_output.push_back(answer);
        }
        break;
    }
    case Parser::ResultType::BOOLEAN:
        if (!assignment_maps.empty()) {
            formatted_output.emplace_back("TRUE");
        } else {
            formatted_output.emplace_back("FALSE");
        }
        break;
    default:
        throw std::runtime_error("Unknown result type");
    }

    return formatted_output;
}
