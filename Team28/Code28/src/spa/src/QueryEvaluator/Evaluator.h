#pragma once

#include "ForwardCheckingSolver.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/Elem.h"
#include "Parser/PQLParser/QueryObject.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/Common.h"
#include "QueryEvaluator/constraints/BinaryConstraint.h"
#include "QueryEvaluator/constraints/ConstraintUtils.h"
#include "QueryEvaluator/domains/DomainUtils.h"
#include <algorithm>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace QueryEvaluator {

typedef std::unordered_map<std::string, std::vector<std::string>> DependencyMap;
typedef std::unordered_map<std::string, Parser::DesignEntity> DesignEntityMap;

// The main entry point for query evaluator users.
class Evaluator {
public:
    // Get all the results of a query evaluation by adding them to the `output` list parameter.
    static void evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb,
                         const Parser::QueryObject& query_object, std::list<std::string>& output);

private:
    // Get all the assignment groups for a given query object. All synonyms in an assignment group
    // are related to one another.
    //
    // An assignment group consists of:
    // 1. All synonyms from the group which are part of the result tuple (a.k.a targets).
    // 1. All domains for the synonyms from the group.
    // 2. All constraints for the synonyms from the group.
    //
    // For instance, in the query `Select s1 such that Follows(s1, s2) pattern a(v, _)`
    // there exists 2 assignment groups:
    // - Group 1: targets = {s1}, domains = {s1: [...], s2: [...]}, constraints = [Follows(s1, s2)]
    // - Group 2: targets = {}, domains = {a: [...], v: [...]}, constraints = [pattern a(v, _)]
    static std::vector<AssignmentGroup>
    get_assignment_groups(std::shared_ptr<KnowledgeBase::PKB> pkb,
                          const Parser::QueryObject& query_object);

    // Choose an assignment group to evaluate next. In some cases, this will speed up evaluation
    // process since we might be able to terminate early if the evaluation of an assignment group
    // yields no result.
    static AssignmentGroup choose_assignment_group(std::vector<AssignmentGroup>& assignment_groups);
    static long score_assignment_group(const AssignmentGroup& assignment_group);

    // Get all the synonyms connected to the given starting synonym.
    static std::unordered_set<std::string>
    get_connected_synonyms(const std::string& starting_synonym, DependencyMap& dependency_map);

    // Get all the "useful" design entities for a query object. A design entity is useful if it
    // is either (1) part of the selection result or (2) involved in some clauses.
    static DesignEntityMap get_design_entity_map(const Parser::QueryObject& query_object);
    static void get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Result& result);
    static void get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::SuchThat& such_that);
    static void get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Pattern& pattern);
    static void get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::With& with);
    static void get_design_entity_map(DesignEntityMap& design_entity_map,
                                      const Parser::DeclarationMap& declaration_map,
                                      const Parser::Elem& elem);

    // Get all the dependencies between synonyms for a query object.
    // For instance, if we receive a such-that clause `Follows(s1, s2)`, we say that
    // the synonyms s1 and s2 are dependent on one another.
    static DependencyMap get_dependency_map(const Parser::QueryObject& query_object);
    static void get_dependency_map(DependencyMap& dependency_map,
                                   const Parser::SuchThat& such_that);
    static void get_dependency_map(DependencyMap& dependency_map, const Parser::Pattern& pattern);
    static void get_dependency_map(DependencyMap& dependency_map, const Parser::With& with);

    // Check whether the query object classes contain any synonym.
    static bool has_synonym(const Parser::SuchThatRef& ref);
    static bool has_synonym(const Parser::WithRef& ref);
    static bool has_synonym(const Parser::EntityRef& entity_ref);
    static bool has_synonym(const Parser::StatementRef& entity_ref);

    // Get the synonyms contained within the query object classes.
    static std::unordered_set<std::string> get_synonyms(const Parser::Result& result);
    static std::string get_synonym(const Parser::Elem& elem);
    static std::string get_synonym(const Parser::SuchThatRef& ref);
    static std::string get_synonym(const Parser::WithRef& ref);
    static std::string get_synonym(const Parser::EntityRef& ref);
    static std::string get_synonym(const Parser::StatementRef& ref);

    // Merge a list of AssignmentMaps together (via a cross-product).
    static AssignmentMaps
    merge_assignment_maps(const std::vector<AssignmentMaps>& all_assignment_maps);

    // Merge two AssignmentMap together.
    static AssignmentMap merge_assignment_map(const AssignmentMap& first,
                                              const AssignmentMap& second);

    // Get the formatted output to a query.
    static std::list<std::string> get_formatted_output(const Parser::Result& result,
                                                       const AssignmentMaps& assignment_maps);
};

} // namespace QueryEvaluator
