#pragma once

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/QueryObject.h"
#include "QueryEvaluator/Assignment.h"
#include "QueryEvaluator/constraints/ConstraintUtils.h"
#include "QueryEvaluator/domains/DomainUtils.h"
#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace QueryEvaluator {

class BruteForceEvaluator {
public:
    // Gets all the results of a query evaluation by adding them to the `results` list parameter.
    static void evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::QueryObject& query_object,
                         std::list<std::string>& results);

private:
    // Get all the assignment groups for a given query object.
    //
    // An assignment group consists of:
    // 1. All synonyms that are part of the group.
    // 2. All possible assignments that are constructed from the synonyms in the group.
    // All synonyms in an assignment group are related to one another.
    //
    // For instance, in the query `Select s1 such that Follows(s1, s2) pattern a(v, _)`
    // there exists 2 assignment groups:
    // 1. A group consisting of the synonyms {s1, s2} and their possible assignments.
    // 2. A group consisting of the synonyms {a, v} and their possible assignments.
    static std::vector<std::pair<std::unordered_set<std::string>, std::vector<AssignmentMap>>>
    get_assignment_groups(std::shared_ptr<KnowledgeBase::PKB> pkb,
                          Parser::QueryObject& query_object);

    // Get all the possible assignments given the supplied design entities. We define
    // "all possible assignments" as the cross-product of all the values each of
    // the design entity can take.
    static std::vector<AssignmentMap>
    get_assignments(std::shared_ptr<KnowledgeBase::PKB> pkb,
                    std::vector<Parser::DesignEntity>& design_entities);

    // Get all the design entities for a query object.
    static std::unordered_map<std::string, Parser::DesignEntity>
    get_design_entities(Parser::QueryObject& query_object);

    // Get all the dependencies between synonyms for a query object.
    // For instance, if we receive a such-that clause `Follows(s1, s2)`, we say that
    // the synonyms s1 and s2 are dependent on one another.
    static std::unordered_map<std::string, std::vector<std::string>>
    get_dependencies(Parser::QueryObject& query_object);

    static bool has_synonym(Parser::Ref& ref);
    static bool has_synonym(Parser::EntityRef& entity_ref);
    static bool has_synonym(Parser::StatementRef& entity_ref);

    static std::string get_synonym(Parser::Ref& ref);
    static std::string get_synonym(Parser::EntityRef& ref);
    static std::string get_synonym(Parser::StatementRef& ref);
};

} // namespace QueryEvaluator
