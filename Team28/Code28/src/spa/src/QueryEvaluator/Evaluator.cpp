#include "Evaluator.h"

using namespace QueryEvaluator;

void Evaluator::evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb,
                         const Parser::QueryObject& query_object, std::list<std::string>& output) {

    auto empty_constraints = ConstraintUtils::get_empty_constraints(pkb, query_object);
    for (auto const& empty_constraint : empty_constraints) {
        if (!empty_constraint.is_valid()) {
            return;
        }
    }

    auto assignment_groups = get_assignment_groups(pkb, query_object);

    // TODO: Optimisation around which assignment group to evaluate first
    std::vector<AssignmentMaps> all_assignment_maps;
    for (auto const& assignment_group : assignment_groups) {
        auto targets = std::get<0>(assignment_group);
        auto domain_map = std::get<1>(assignment_group);
        auto constraints = std::get<2>(assignment_group);

        auto assignment_maps = ForwardCheckingSolver::solve(targets, domain_map, constraints);
        if (assignment_maps.empty()) {
            return;
        }

        all_assignment_maps.push_back(assignment_maps);
    }

    auto merged_assignment_maps = merge_assignment_maps(all_assignment_maps);

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

    switch (pattern.get_type()) {
    case Parser::PatternType::ASSIGN: {
        auto pattern_assign = pattern.get_pattern_assign();

        auto assign_synonym = pattern_assign.get_assign_synonym();
        auto assign_design_entity = declaration_map.get(assign_synonym);
        design_entity_map[assign_synonym] = assign_design_entity;

        auto entity_ref = pattern_assign.get_entity_ref();
        if (has_synonym(entity_ref)) {
            auto entity_ref_synonym = get_synonym(entity_ref);
            auto entity_ref_design_entity = declaration_map.get(entity_ref_synonym);
            design_entity_map[entity_ref_synonym] = entity_ref_design_entity;
        }
        break;
    }
    default:
        throw std::runtime_error("Unknown pattern type");
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
    switch (pattern.get_type()) {
    case Parser::PatternType::ASSIGN: {
        auto pattern_assign = pattern.get_pattern_assign();
        auto assign_synonym = pattern_assign.get_assign_synonym();
        auto entity_ref = pattern_assign.get_entity_ref();
        if (has_synonym(entity_ref)) {
            auto entity_ref_synonym = get_synonym(entity_ref);
            dependency_map[assign_synonym].push_back(entity_ref_synonym);
            dependency_map[entity_ref_synonym].push_back(assign_synonym);
        }
        break;
    }
    default:
        throw std::runtime_error("Unknown pattern type");
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
        throw std::runtime_error("Unknown ref type");
    }
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

AssignmentMaps
Evaluator::merge_assignment_maps(const std::vector<AssignmentMaps>& all_assignment_maps) {
    if (all_assignment_maps.empty()) {
        throw std::runtime_error("all_assignment_maps cannot be empty");
    }

    auto merged = all_assignment_maps[0];
    for (int i = 1; i < all_assignment_maps.size(); i++) {
        auto merged_copy = merged;
        auto assignment_maps = all_assignment_maps[i];
        merged.clear();
        for (auto const& first : merged_copy) {
            for (auto const& second : assignment_maps) {
                merged.insert(merge_assignment_map(first, second));
            }
        }
    }
    return merged;
}

AssignmentMap Evaluator::merge_assignment_map(const AssignmentMap& first,
                                              const AssignmentMap& second) {

    auto merged = first;
    for (auto const& entry : second) {
        merged[entry.first] = entry.second;
    }
    return merged;
}

std::list<std::string> Evaluator::get_formatted_output(const Parser::Result& result,
                                                       const AssignmentMaps& assignment_maps) {

    std::list<std::string> formatted_output;

    for (auto const& assignment_map : assignment_maps) {
        switch (result.get_type()) {
        case Parser::ResultType::TUPLE: {
            auto tuple = result.get_tuple();
            for (auto const& elem : tuple) {
                auto synonym = elem.get_synonym();
                auto assignment = assignment_map.at(synonym);
                std::string result;
                
                switch (elem.get_type()) { 
                case Parser::ElemType::ATTR_REF:
                    if (elem.get_attr_ref().get_attr_name() == "stmt#" ||
                        elem.get_attr_ref().get_attr_name() == "value") {
                        result = std::to_string(assignment.get_int_value());
                    } else if (elem.get_attr_ref().get_attr_name() == "procName" ||
                        elem.get_attr_ref().get_attr_name() == "varName") {
                        result = assignment.get_string_value();
                    } else {
                        throw std::runtime_error("Unknown attribute name");
                    }
                    break;
                case Parser::ElemType::SYNONYM : 
                    switch (assignment.get_type()) {
                    case AssignmentType::INTEGER:
                        result = std::to_string(assignment.get_int_value());
                        break;
                    case AssignmentType::STRING:
                        result = assignment.get_string_value();
                        break;
                    case AssignmentType::BOTH:
                        result = std::to_string(assignment.get_int_value());
                        break;
                    default:
                        throw std::runtime_error("Unknown assignment type");
                    }
                default:
                    std::runtime_error("Unknown elem type");
                    break;
                }
                formatted_output.push_back(result);
            }
            break;
        }
        case Parser::ResultType::BOOLEAN:
            if (assignment_map.size() > 0) {
                formatted_output.push_back("TRUE");
            } else {
                formatted_output.push_back("FALSE");
            }
        default:
            throw std::runtime_error("Unknown result type");
        }
    }

    return formatted_output;
}
