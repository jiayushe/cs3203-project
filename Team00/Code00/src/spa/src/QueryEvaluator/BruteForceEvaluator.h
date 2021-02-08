#pragma once

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/QueryObject.h"
#include "QueryEvaluator/Assignment.h"
#include <list>
#include <memory>
#include <string>

namespace QueryEvaluator {

class BruteForceEvaluator {
public:
    // Gets all the results of a query by adding them to the `results` list parameter.
    static void evaluate(std::shared_ptr<KnowledgeBase::PKB> pkb, Parser::QueryObject& query_object,
                         std::list<std::string>& results);

private:
    // Gets all the possible assignments for a given query.
    static std::list<AssignmentMap> get_assignments(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                    Parser::QueryObject& query_object);

    // Gets all the design entities involved in a given query. For example, the design entities
    // involved in the query `stmt s1, s2, s3, s4; Select s1` are `s1` only.
    static std::list<Parser::DesignEntity> get_design_entities(Parser::QueryObject& query_object);

    // Checks whether the given ref points to any design entity.
    static bool has_design_entity_in_ref(Parser::Ref& ref);

    // Gets the design entity that is contained within the given ref.
    static Parser::DesignEntity get_design_entity_in_ref(Parser::DeclarationMap& declarations,
                                                         Parser::Ref& ref);

    // Checks whether the given entity ref points to any design entity.
    static bool has_design_entity_in_entity_ref(Parser::EntityRef& entity_ref);

    // Gets the design entity that is contained within the given entity ref.
    static Parser::DesignEntity
    get_design_entity_in_entity_ref(Parser::DeclarationMap& declarations,
                                    Parser::EntityRef& entity_ref);

    // Checks whether the given statement ref points to any design entity.
    static bool has_design_entity_in_statement_ref(Parser::StatementRef& statement_ref);

    // Gets the design entity that is contained within the given statement ref.
    static Parser::DesignEntity
    get_design_entity_in_statement_ref(Parser::DeclarationMap& declarations,
                                       Parser::StatementRef& statement_ref);
};

} // namespace QueryEvaluator
