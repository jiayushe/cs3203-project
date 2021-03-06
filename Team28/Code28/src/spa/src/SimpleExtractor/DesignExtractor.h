#pragma once

#include "KnowledgeBase/PKB.h"
#include <chrono>
#include <list>
#include <memory>
#include <queue>
#include <stack>

namespace SimpleExtractor {

class DesignExtractor {
public:
    // Extracts control flow graph from the given PKB.
    // Throws if the given PKB has an invalid AST
    static void extract_cfg(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts next relationships from the given PKB.
    static void extract_next_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts modify relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_modify_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts use relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_use_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts follow relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_follow_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts parent relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_parent_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts call relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_call_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts affect relationships from the given PKB.
    static void extract_affect_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts next bip relationships from the given PKB.
    static void extract_next_bip_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

    // Extracts affect bip relationships from the given PKB.
    static void extract_affect_bip_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb);

private:
    /* Modify */
    static void
    extract_modify_relationship_from_stmt_list(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                               std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_modify_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                      std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_read_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                               std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_assign_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                 std::string proc_name,
                                                 std::shared_ptr<Parser::SimpleNode> stmt);

    /* Use */
    static void
    extract_use_relationship_from_stmt_list(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                            std::string proc_name,
                                            std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_use_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                   std::string proc_name,
                                                   std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_print_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                         std::string proc_name,
                                                         std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_assign_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                          std::string proc_name,
                                                          std::shared_ptr<Parser::SimpleNode> stmt);
    static void extract_use_relationship_from_arithmetic_or_conditional(
        std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name, int statement_id,
        std::shared_ptr<Parser::SimpleNode> node);

    /* Follow */
    static void
    extract_follow_relationship_from_stmt_list(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                               std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_follow_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                      std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt,
                                                      int next_stmt_id);

    /* Parent */
    static void extract_parent_relationship_from_stmt_list(
        std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
        std::shared_ptr<Parser::SimpleNode> stmt_list, int parent_stmt_id);
    static void extract_parent_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                      std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt,
                                                      int parent_stmt_id);

    /* Call */
    static void
    extract_call_relationship_from_stmt_list(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                             std::string proc_name,
                                             std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_call_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                    std::string proc_name,
                                                    std::shared_ptr<Parser::SimpleNode> stmt);

    /* CFG */
    static void extract_cfg_from_stmt_list(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                           std::string proc_name,
                                           std::unordered_map<int, std::unordered_set<int>>& cfg,
                                           std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_cfg_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                      std::string proc_name,
                                      std::unordered_map<int, std::unordered_set<int>>& cfg,
                                      std::shared_ptr<Parser::SimpleNode> stmt, int next_stmt_id);
    static void extract_cfg_from_while_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                            std::string proc_name,
                                            std::unordered_map<int, std::unordered_set<int>>& cfg,
                                            std::shared_ptr<Parser::SimpleNode> while_stmt);
    static void extract_cfg_from_if_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                         std::string proc_name,
                                         std::unordered_map<int, std::unordered_set<int>>& cfg,
                                         std::shared_ptr<Parser::SimpleNode> if_stmt,
                                         int next_block_stmt_id);

    /* NextBip */
    static void extract_next_bip_relationship_with_timeout(
        std::shared_ptr<KnowledgeBase::PKB> pkb,
        const std::chrono::time_point<std::chrono::steady_clock>& deadline);
    static std::shared_ptr<std::unordered_set<int>>
    extract_next_bip_relationship_from_while_loop_with_timeout(
        std::shared_ptr<KnowledgeBase::PKB> pkb, int while_stmt_id,
        const std::chrono::time_point<std::chrono::steady_clock>& deadline);

    /* AffectsBip */
    static void extract_affect_bip_relationship_with_timeout(
        std::shared_ptr<KnowledgeBase::PKB> pkb,
        const std::chrono::time_point<std::chrono::steady_clock>& deadline);

    /* Util */
    static std::shared_ptr<KnowledgeBase::Procedure>
    extract_procedure(std::shared_ptr<KnowledgeBase::PKB> pkb,
                      std::shared_ptr<Parser::SimpleNode> proc_node);
    static std::shared_ptr<KnowledgeBase::Statement>
    extract_statement(std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
                      std::shared_ptr<Parser::SimpleNode> stmt_node);
    static std::shared_ptr<KnowledgeBase::Variable>
    extract_variable(std::shared_ptr<KnowledgeBase::PKB> pkb,
                     std::shared_ptr<Parser::SimpleNode> var_node);
    static std::shared_ptr<KnowledgeBase::Constant>
    extract_constant(std::shared_ptr<KnowledgeBase::PKB> pkb,
                     std::shared_ptr<Parser::SimpleNode> cst_node);
    static KnowledgeBase::StatementType
    convert_node_type_to_stmt_type(Parser::SimpleNodeType node_type);
    static std::shared_ptr<Parser::SimpleNode>
    assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                     Parser::SimpleNodeType expected_type);
};

} // namespace SimpleExtractor
