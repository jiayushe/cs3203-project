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
    /* Modify */
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

    /* Use */
    static void
    extract_use_relationship_from_stmt_list(KnowledgeBase::PKB pkb, std::string proc_name,
                                            std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_use_relationship_from_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                   std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_print_stmt(KnowledgeBase::PKB pkb,
                                                         std::string proc_name,
                                                         std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_assign_stmt(KnowledgeBase::PKB pkb,
                                                          std::string proc_name,
                                                          std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_arithmetic_or_conditional_stmt(
        KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt);

    /* Follow */
    static void
    extract_follow_relationship_from_stmt_list(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_follow_relationship_from_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt,
                                                      int next_stmt_id);

    /* Util */
    static KnowledgeBase::Procedure
    extract_procedure(KnowledgeBase::PKB pkb, std::shared_ptr<Parser::SimpleNode> proc_node);
    static KnowledgeBase::Statement
    extract_statement(KnowledgeBase::PKB pkb, std::string proc_name,
                      std::shared_ptr<Parser::SimpleNode> stmt_node);
    static KnowledgeBase::Variable extract_variable(KnowledgeBase::PKB pkb,
                                                    std::shared_ptr<Parser::SimpleNode> var_node);
    static KnowledgeBase::Constant extract_constant(KnowledgeBase::PKB pkb,
                                                    std::shared_ptr<Parser::SimpleNode> cst_node);
    static KnowledgeBase::StatementType
    convert_node_type_to_stmt_type(Parser::SimpleNodeType node_type);
    static std::shared_ptr<Parser::SimpleNode>
    assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                     Parser::SimpleNodeType expected_type);
};

} // namespace SimpleExtractor
