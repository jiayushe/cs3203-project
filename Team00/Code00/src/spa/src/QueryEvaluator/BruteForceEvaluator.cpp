#include "BruteForceEvaluator.h"
#include "QueryEvaluator/constraints/ConstraintUtils.h"
#include "QueryEvaluator/domains/DomainUtils.h"
#include <algorithm>
#include <unordered_set>

using namespace QueryEvaluator;

void BruteForceEvaluator::evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   Parser::QueryObject& query_object,
                                   std::list<std::string>& results) {
    // Compute all possible assignments + all constraints for the given query_object
    auto constraints = ConstraintUtils::get_constraints(pkb, query_object);
    auto assignments = get_assignments(pkb, query_object);

    // Narrow down the possible assignments via the computed constraints
    std::list<AssignmentMap> constrained_assignments;
    for (const auto& assignment : assignments) {
        if (std::all_of(constraints.begin(), constraints.end(),
                        [&](std::shared_ptr<BaseConstraint> constraint) {
                            return constraint->is_valid(assignment);
                        })) {
            constrained_assignments.push_back(assignment);
        }
    }

    // Get the selection answer and deduplicate the result
    std::unordered_set<std::string> unique_results;
    for (const auto& assignment : constrained_assignments) {
        auto selection = assignment.at(query_object.get_selection());
        unique_results.insert(selection.value_as_string());
    }
    for (const auto& result : unique_results) {
        results.push_back(result);
    }
}

std::list<AssignmentMap>
BruteForceEvaluator::get_assignments(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                     Parser::QueryObject& query_object) {
    std::list<AssignmentMap> assignments;
    assignments.emplace_back();

    auto design_entities = get_design_entities(query_object);
    for (auto design_entity : design_entities) {
        auto synonym = design_entity.get_synonym();
        auto domain = DomainUtils::get_domain(pkb, design_entity.get_type());

        if (domain.empty()) {
            return std::list<AssignmentMap>();
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

    return assignments;
}

std::list<Parser::DesignEntity>
BruteForceEvaluator::get_design_entities(Parser::QueryObject& query_object) {

    auto declarations = query_object.get_declarations();
    std::unordered_set<Parser::DesignEntity, Parser::DesignEntityHash> design_entities;

    auto selection_synonym = declarations.get(query_object.get_selection());
    design_entities.insert(selection_synonym);

    if (query_object.has_such_that()) {
        auto such_that = query_object.get_such_that();

        auto left_ref = such_that.get_left_ref();
        if (has_design_entity_in_ref(left_ref)) {
            design_entities.insert(get_design_entity_in_ref(declarations, left_ref));
        }

        auto right_ref = such_that.get_right_ref();
        if (has_design_entity_in_ref(right_ref)) {
            design_entities.insert(get_design_entity_in_ref(declarations, right_ref));
        }
    }

    if (query_object.has_pattern()) {
        auto pattern = query_object.get_pattern();

        auto pattern_synonym = declarations.get(pattern.get_assigned_synonym());
        design_entities.insert(pattern_synonym);

        auto entity_ref = pattern.get_entity_ref();
        if (has_design_entity_in_entity_ref(entity_ref)) {
            design_entities.insert(get_design_entity_in_entity_ref(declarations, entity_ref));
        }
    }

    return std::list<Parser::DesignEntity>(design_entities.begin(), design_entities.end());
}

bool BruteForceEvaluator::has_design_entity_in_ref(Parser::Ref& ref) {
    switch (ref.get_type()) {
    case Parser::RefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return has_design_entity_in_entity_ref(entity_ref);
    }
    case Parser::RefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return has_design_entity_in_statement_ref(statement_ref);
    }
    default:
        throw "Unknown ref type";
    }
}

Parser::DesignEntity
BruteForceEvaluator::get_design_entity_in_ref(Parser::DeclarationMap& declarations,
                                              Parser::Ref& ref) {
    switch (ref.get_type()) {
    case Parser::RefType::ENTITY: {
        auto entity_ref = ref.get_entity_ref();
        return get_design_entity_in_entity_ref(declarations, entity_ref);
    }
    case Parser::RefType::STATEMENT: {
        auto statement_ref = ref.get_statement_ref();
        return get_design_entity_in_statement_ref(declarations, statement_ref);
    }
    default:
        throw "Unknown ref type";
    }
}

bool BruteForceEvaluator::has_design_entity_in_entity_ref(Parser::EntityRef& entity_ref) {
    return entity_ref.get_type() == Parser::EntityRefType::SYNONYM;
}

Parser::DesignEntity
BruteForceEvaluator::get_design_entity_in_entity_ref(Parser::DeclarationMap& declarations,
                                                     Parser::EntityRef& entity_ref) {
    switch (entity_ref.get_type()) {
    case Parser::EntityRefType::SYNONYM:
        return declarations.get(entity_ref.get_synonym());
    case Parser::EntityRefType::ANY:
    case Parser::EntityRefType::NAME:
        throw "No design entity in entity ref";
    default:
        throw "Unknown entity ref type";
    }
}

bool BruteForceEvaluator::has_design_entity_in_statement_ref(Parser::StatementRef& statement_ref) {
    return statement_ref.get_type() == Parser::StatementRefType::SYNONYM;
}

Parser::DesignEntity
BruteForceEvaluator::get_design_entity_in_statement_ref(Parser::DeclarationMap& declarations,
                                                        Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::SYNONYM:
        return declarations.get(statement_ref.get_synonym());
    case Parser::StatementRefType::ANY:
    case Parser::StatementRefType::STATEMENT_ID:
        throw "No design entity in statement ref";
    default:
        throw "Unknown entity ref type";
    }
}
