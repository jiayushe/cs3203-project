#pragma once

#include "KnowledgeBase/PKB.h"

namespace SimpleExtractor {

class DesignExtractor {
public:
    // Extracts modify relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_modify_relationship(KnowledgeBase::PKB pkb);

    // Extracts use relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_use_relationship(KnowledgeBase::PKB pkb);

    // Extracts follow relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_follow_relationship(KnowledgeBase::PKB pkb);

    // Extracts parent relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_parent_relationship(KnowledgeBase::PKB pkb);

private:
    static KnowledgeBase::Procedure extract_procedure(KnowledgeBase::PKB pkb,
                                                      std::shared_ptr<Parser::SimpleNode> ast);

    static KnowledgeBase::Statement
    extract_statement(KnowledgeBase::PKB pkb, std::string proc_name,
                      std::shared_ptr<Parser::SimpleNode> statement_node);

    static KnowledgeBase::Variable extract_variable(KnowledgeBase::PKB pkb,
                                                    std::shared_ptr<Parser::SimpleNode> ast);

    static void
    extract_modify_relationship_from_stmt_list(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_modify_relationship_from_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_read_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_assign_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                 std::shared_ptr<Parser::SimpleNode> stmt);

    static void
    extract_follow_relationship_from_stmt_list(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_follow_relationship_from_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt,
                                                      int prev_stmt_id, int next_stmt_id);

    static KnowledgeBase::StatementType
    convert_node_type_to_stmt_type(Parser::SimpleNodeType node_type);

    static void assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                                 Parser::SimpleNodeType expected_type);
};

} // namespace SimpleExtractor
