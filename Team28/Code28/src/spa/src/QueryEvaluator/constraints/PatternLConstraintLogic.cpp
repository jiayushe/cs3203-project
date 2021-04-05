#include "PatternLConstraintLogic.h"

using namespace QueryEvaluator;

PatternLConstraintLogic::PatternLConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                 const Parser::Pattern& pattern)
    : BaseConstraintLogic(std::move(pkb)), type(pattern.get_type()) {

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

    synonyms.insert(pattern_synonym);
    if (entity_ref.get_type() == Parser::EntityRefType::SYNONYM) {
        synonyms.insert(entity_ref.get_synonym());
    }
}

bool PatternLConstraintLogic::is_valid(const AssignmentMap& assignment_map) const {

    auto pattern_statement = get_statement(assignment_map, pattern_synonym);

    switch (type) {
    case Parser::PatternType::ASSIGN: {
        if (entity_ref.get_type() == Parser::EntityRefType::ANY) {
            return true;
        }
        auto variable_name = get_variable_name(assignment_map, entity_ref);
        auto direct_modifies = pattern_statement->get_direct_modifies();
        return direct_modifies->find(variable_name) != direct_modifies->end();
    }
    case Parser::PatternType::IF:
    case Parser::PatternType::WHILE: {
        if (entity_ref.get_type() == Parser::EntityRefType::ANY) {
            return !pattern_statement->get_direct_uses()->empty();
        }
        auto variable_name = get_variable_name(assignment_map, entity_ref);
        auto direct_uses = pattern_statement->get_direct_uses();
        return direct_uses->find(variable_name) != direct_uses->end();
    }
    default:
        throw std::runtime_error("Unknown pattern type");
    }
}

std::unordered_set<std::string> PatternLConstraintLogic::get_synonyms() const { return synonyms; }
