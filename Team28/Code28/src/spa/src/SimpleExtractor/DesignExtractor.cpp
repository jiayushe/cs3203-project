#include "DesignExtractor.h"
#include "KnowledgeBase/Procedure.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include <stdexcept>

using namespace SimpleExtractor;

void DesignExtractor::extract_modify_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_modify_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
    }
}

void DesignExtractor::extract_modify_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        extract_modify_relationship_from_stmt(pkb, proc_name, stmts[i]);
    }
}

void DesignExtractor::extract_modify_relationship_from_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto curr_stmt = extract_statement(pkb, proc_name, stmt);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt->get_type()) {
    case KnowledgeBase::StatementType::READ:
        extract_modify_relationship_from_read_stmt(pkb, proc_name, stmt);
        break;
    case KnowledgeBase::StatementType::PRINT:
        // PRINT statements do not have modify relationship
        break;
    case KnowledgeBase::StatementType::CALL:
        // Handled in PKB
        break;
    case KnowledgeBase::StatementType::WHILE:
        // WHILE statements do not have direct modify relationship
        stmt_list = stmt->get_child(1);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        // IF statements do not have direct modify relationship
        stmt_list = stmt->get_child(1);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        stmt_list = stmt->get_child(2);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::ASSIGN:
        extract_modify_relationship_from_assign_stmt(pkb, proc_name, stmt);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void DesignExtractor::extract_modify_relationship_from_read_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    auto var = extract_variable(pkb, var_node);
    pkb->add_modify_relationship(stmt->get_statement_id(), var->get_name());
}

void DesignExtractor::extract_modify_relationship_from_assign_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    auto var = extract_variable(pkb, var_node);
    pkb->add_modify_relationship(stmt->get_statement_id(), var->get_name());
}

void DesignExtractor::extract_use_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_use_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
    }
}

void DesignExtractor::extract_use_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        extract_use_relationship_from_stmt(pkb, proc_name, stmts[i]);
    }
}

void DesignExtractor::extract_use_relationship_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                                         std::string proc_name,
                                                         std::shared_ptr<Parser::SimpleNode> stmt) {
    auto curr_stmt = extract_statement(pkb, proc_name, stmt);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt->get_type()) {
    case KnowledgeBase::StatementType::READ:
        // READ statements do not have use relationship
        break;
    case KnowledgeBase::StatementType::PRINT:
        extract_use_relationship_from_print_stmt(pkb, proc_name, stmt);
        break;
    case KnowledgeBase::StatementType::CALL:
        // Handled in PKB
        break;
    case KnowledgeBase::StatementType::WHILE:
        extract_use_relationship_from_arithmetic_or_conditional(
            pkb, proc_name, stmt->get_statement_id(), stmt->get_child(0));
        stmt_list = stmt->get_child(1);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        extract_use_relationship_from_arithmetic_or_conditional(
            pkb, proc_name, stmt->get_statement_id(), stmt->get_child(0));
        stmt_list = stmt->get_child(1);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        stmt_list = stmt->get_child(2);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::ASSIGN:
        extract_use_relationship_from_assign_stmt(pkb, proc_name, stmt);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void DesignExtractor::extract_use_relationship_from_print_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    auto var = extract_variable(pkb, var_node);
    pkb->add_use_relationship(stmt->get_statement_id(), var->get_name());
}

void DesignExtractor::extract_use_relationship_from_assign_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto child_node = stmt->get_child(1);
    switch (child_node->get_type()) {
    case Parser::SimpleNodeType::ARITHMETIC:
        extract_use_relationship_from_arithmetic_or_conditional(
            pkb, proc_name, stmt->get_statement_id(), child_node);
        break;
    case Parser::SimpleNodeType::VAR_NAME: {
        auto var = extract_variable(pkb, child_node);
        pkb->add_use_relationship(stmt->get_statement_id(), var->get_name());
        break;
    }
    case Parser::SimpleNodeType::CONST_VALUE:
        extract_constant(pkb, child_node);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void DesignExtractor::extract_use_relationship_from_arithmetic_or_conditional(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name, int statement_id,
    std::shared_ptr<Parser::SimpleNode> node) {
    auto children_nodes = node->get_children();
    int num_children = children_nodes.size();
    for (int i = 0; i < num_children; i++) {
        auto child_node = children_nodes[i];
        switch (child_node->get_type()) {
        case Parser::SimpleNodeType::ARITHMETIC:
        case Parser::SimpleNodeType::CONDITIONAL:
            extract_use_relationship_from_arithmetic_or_conditional(pkb, proc_name, statement_id,
                                                                    child_node);
            break;
        case Parser::SimpleNodeType::CONST_VALUE:
            extract_constant(pkb, child_node);
            break;
        case Parser::SimpleNodeType::VAR_NAME: {
            auto var = extract_variable(pkb, child_node);
            pkb->add_use_relationship(statement_id, var->get_name());
            break;
        }
        default:
            throw std::runtime_error("Unhandled statement type");
        }
    }
}

void DesignExtractor::extract_follow_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_follow_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
    }
}

void SimpleExtractor::DesignExtractor::extract_follow_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        int prev_stmt_id = i > 0 ? stmts.at(i - 1)->get_statement_id() : -1;
        extract_follow_relationship_from_stmt(pkb, proc_name, curr_stmt_node, prev_stmt_id);
    }
}

void SimpleExtractor::DesignExtractor::extract_follow_relationship_from_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt, int prev_stmt_id) {
    auto curr_stmt = extract_statement(pkb, proc_name, stmt);
    pkb->add_follow_relationship(prev_stmt_id, curr_stmt->get_id());
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt->get_type()) {
    case KnowledgeBase::StatementType::ASSIGN:
    case KnowledgeBase::StatementType::CALL:
    case KnowledgeBase::StatementType::PRINT:
    case KnowledgeBase::StatementType::READ:
        break;
    case KnowledgeBase::StatementType::WHILE:
        // Process nested statements in WHILE
        stmt_list = stmt->get_child(1);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        // Process then branch statements in IF
        stmt_list = stmt->get_child(1);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        // Process else branch statements in IF
        stmt_list = stmt->get_child(2);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void DesignExtractor::extract_parent_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_parent_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node, -1);
    }
}

void SimpleExtractor::DesignExtractor::extract_parent_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list, int parent_stmt_id) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        extract_parent_relationship_from_stmt(pkb, proc_name, curr_stmt_node, parent_stmt_id);
    }
}

void SimpleExtractor::DesignExtractor::extract_parent_relationship_from_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt, int parent_stmt_id) {
    auto curr_stmt = extract_statement(pkb, proc_name, stmt);
    int curr_stmt_id = curr_stmt->get_id();
    pkb->add_parent_relationship(parent_stmt_id, curr_stmt_id);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt->get_type()) {
    case KnowledgeBase::StatementType::ASSIGN:
    case KnowledgeBase::StatementType::CALL:
    case KnowledgeBase::StatementType::PRINT:
    case KnowledgeBase::StatementType::READ:
        break;
    case KnowledgeBase::StatementType::WHILE:
        // Process nested statements in WHILE
        stmt_list = stmt->get_child(1);
        extract_parent_relationship_from_stmt_list(pkb, proc_name, stmt_list, curr_stmt_id);
        break;
    case KnowledgeBase::StatementType::IF:
        // Process then branch statements in IF
        stmt_list = stmt->get_child(1);
        extract_parent_relationship_from_stmt_list(pkb, proc_name, stmt_list, curr_stmt_id);
        // Process else branch statements in IF
        stmt_list = stmt->get_child(2);
        extract_parent_relationship_from_stmt_list(pkb, proc_name, stmt_list, curr_stmt_id);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void SimpleExtractor::DesignExtractor::extract_call_relationship(
    std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_call_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
    }
}

void SimpleExtractor::DesignExtractor::extract_call_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        extract_call_relationship_from_stmt(pkb, proc_name, curr_stmt_node);
    }
}

void SimpleExtractor::DesignExtractor::extract_call_relationship_from_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt) {
    auto curr_stmt = extract_statement(pkb, proc_name, stmt);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    std::string callee_name;
    switch (curr_stmt->get_type()) {
    case KnowledgeBase::StatementType::ASSIGN:
    case KnowledgeBase::StatementType::PRINT:
    case KnowledgeBase::StatementType::READ:
        break;
    case KnowledgeBase::StatementType::CALL:
        callee_name = curr_stmt->get_procedure_called();
        pkb->add_call_relationship(proc_name, callee_name);
        break;
    case KnowledgeBase::StatementType::WHILE:
        // Process nested statements in WHILE
        stmt_list = stmt->get_child(1);
        extract_call_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        // Process then branch statements in IF
        stmt_list = stmt->get_child(1);
        extract_call_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        // Process else branch statements in IF
        stmt_list = stmt->get_child(2);
        extract_call_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    default:
        throw std::runtime_error("Unhandled statement type");
    }
}

void SimpleExtractor::DesignExtractor::extract_cfg(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    std::unordered_map<int, std::unordered_set<int>> cfg;
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_cfg_from_stmt_list(cfg, stmt_list_node);
    }
    pkb->set_cfg(cfg);
}

void SimpleExtractor::DesignExtractor::extract_cfg_from_stmt_list(
    std::unordered_map<int, std::unordered_set<int>> cfg,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);

        int next_stmt_id = i == num_stmt - 1 ? -1 : stmts.at(i + 1)->get_statement_id();
        extract_cfg_from_stmt(cfg, curr_stmt_node, next_stmt_id);
    }
}

void SimpleExtractor::DesignExtractor::extract_cfg_from_stmt(
    std::unordered_map<int, std::unordered_set<int>> cfg, std::shared_ptr<Parser::SimpleNode> stmt,
    int next_stmt_id) {
    int curr_stmt_id = stmt->get_statement_id();

    switch (stmt->get_type()) {
    case Parser::SimpleNodeType::WHILE:
        if (next_stmt_id != -1) {
            cfg[curr_stmt_id].insert(next_stmt_id);
        }
        extract_cfg_from_while_stmt(cfg, stmt);
        break;
    case Parser::SimpleNodeType::IF:
        extract_cfg_from_if_stmt(cfg, stmt, next_stmt_id);
        break;
    default:
        if (next_stmt_id != -1) {
            cfg[curr_stmt_id].insert(next_stmt_id);
        }
        break;
    }
}

void SimpleExtractor::DesignExtractor::extract_cfg_from_while_stmt(
    std::unordered_map<int, std::unordered_set<int>> cfg,
    std::shared_ptr<Parser::SimpleNode> while_stmt) {
    int parent_stmt_id = while_stmt->get_statement_id();
    auto child_stmt_list = while_stmt->get_child(1);
    int first_stmt_id = child_stmt_list->get_child(0)->get_statement_id();
    cfg[parent_stmt_id].insert(first_stmt_id);

    extract_cfg_from_stmt_list(cfg, child_stmt_list);

    int child_stmt_list_size = child_stmt_list->get_children().size();
    auto last_stmt = child_stmt_list->get_child(child_stmt_list_size - 1);
    extract_cfg_from_stmt(cfg, last_stmt, parent_stmt_id);
}

void SimpleExtractor::DesignExtractor::extract_cfg_from_if_stmt(
    std::unordered_map<int, std::unordered_set<int>> cfg,
    std::shared_ptr<Parser::SimpleNode> if_stmt, int next_block_stmt_id) {
    int parent_stmt_id = if_stmt->get_statement_id();
    for (int i = 1; i <= 2; i++) {
        auto branch_stmt_list = if_stmt->get_child(i);
        int first_stmt_id = branch_stmt_list->get_child(0)->get_statement_id();
        cfg[parent_stmt_id].insert(first_stmt_id);

        extract_cfg_from_stmt_list(cfg, branch_stmt_list);

        int branch_stmt_list_size = branch_stmt_list->get_children().size();
        auto last_stmt = branch_stmt_list->get_child(branch_stmt_list_size - 1);
        extract_cfg_from_stmt(cfg, last_stmt, next_block_stmt_id);
    }
}

std::shared_ptr<KnowledgeBase::Procedure>
DesignExtractor::extract_procedure(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                   std::shared_ptr<Parser::SimpleNode> proc_node) {
    std::string proc_name = proc_node->get_child(0)->get_value();
    return pkb->add_procedure(proc_name);
}

std::shared_ptr<KnowledgeBase::Statement>
DesignExtractor::extract_statement(std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
                                   std::shared_ptr<Parser::SimpleNode> stmt_node) {
    int stmt_id = stmt_node->get_statement_id();
    KnowledgeBase::StatementType statement_type =
        convert_node_type_to_stmt_type(stmt_node->get_type());
    return pkb->add_statement(statement_type, stmt_id, proc_name, stmt_node);
}

std::shared_ptr<KnowledgeBase::Variable>
DesignExtractor::extract_variable(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                  std::shared_ptr<Parser::SimpleNode> var_node) {
    std::string var_name = var_node->get_value();
    return pkb->add_variable(var_name);
}

std::shared_ptr<KnowledgeBase::Constant>
DesignExtractor::extract_constant(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                  std::shared_ptr<Parser::SimpleNode> cst_node) {
    int value = stoi(cst_node->get_value());
    return pkb->add_constant(value);
}

KnowledgeBase::StatementType
DesignExtractor::convert_node_type_to_stmt_type(Parser::SimpleNodeType node_type) {
    switch (node_type) {
    case Parser::SimpleNodeType::ASSIGN:
        return KnowledgeBase::StatementType::ASSIGN;
    case Parser::SimpleNodeType::CALL:
        return KnowledgeBase::StatementType::CALL;
    case Parser::SimpleNodeType::IF:
        return KnowledgeBase::StatementType::IF;
    case Parser::SimpleNodeType::PRINT:
        return KnowledgeBase::StatementType::PRINT;
    case Parser::SimpleNodeType::READ:
        return KnowledgeBase::StatementType::READ;
    case Parser::SimpleNodeType::WHILE:
        return KnowledgeBase::StatementType::WHILE;
    default:
        throw std::runtime_error("The AST node is not a statement node.");
    }
}

std::shared_ptr<Parser::SimpleNode>
DesignExtractor::assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                                  Parser::SimpleNodeType expected_type) {
    Parser::SimpleNodeType actual_type = node->get_type();
    if (actual_type != expected_type) {
        throw std::runtime_error("Expected " + to_string(expected_type) + ", but encountered " +
                                 to_string(actual_type));
    }
    return node;
}
