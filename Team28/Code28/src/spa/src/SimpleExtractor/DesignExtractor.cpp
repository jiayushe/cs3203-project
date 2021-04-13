#include "DesignExtractor.h"
#include "KnowledgeBase/Procedure.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include <stdexcept>

using namespace SimpleExtractor;

void DesignExtractor::extract_cfg(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    int num_proc = ast->get_children().size();
    std::unordered_map<int, std::unordered_set<int>> cfg;
    for (int i = 0; i < num_proc; i++) {
        auto proc_node = assert_node_type(ast->get_child(i), Parser::SimpleNodeType::PROCEDURE);
        auto proc = extract_procedure(pkb, proc_node);
        auto stmt_list_node =
            assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
        extract_cfg_from_stmt_list(pkb, proc->get_name(), cfg, stmt_list_node);
    }
    pkb->set_cfg(cfg);
}

void DesignExtractor::extract_cfg_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::unordered_map<int, std::unordered_set<int>>& cfg,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);

        int next_stmt_id = i == num_stmt - 1 ? -1 : stmts.at(i + 1)->get_statement_id();
        extract_cfg_from_stmt(pkb, proc_name, cfg, curr_stmt_node, next_stmt_id);
    }
}

void DesignExtractor::extract_cfg_from_stmt(std::shared_ptr<KnowledgeBase::PKB> pkb,
                                            std::string proc_name,
                                            std::unordered_map<int, std::unordered_set<int>>& cfg,
                                            std::shared_ptr<Parser::SimpleNode> stmt,
                                            int next_stmt_id) {
    int curr_stmt_id = stmt->get_statement_id();
    extract_statement(pkb, proc_name, stmt);

    switch (stmt->get_type()) {
    case Parser::SimpleNodeType::WHILE:
        if (next_stmt_id != -1) {
            cfg[curr_stmt_id].insert(next_stmt_id);
        }
        extract_cfg_from_while_stmt(pkb, proc_name, cfg, stmt);
        break;
    case Parser::SimpleNodeType::IF:
        extract_cfg_from_if_stmt(pkb, proc_name, cfg, stmt, next_stmt_id);
        break;
    default:
        if (next_stmt_id != -1) {
            cfg[curr_stmt_id].insert(next_stmt_id);
        }
        break;
    }
}

void DesignExtractor::extract_cfg_from_while_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::unordered_map<int, std::unordered_set<int>>& cfg,
    std::shared_ptr<Parser::SimpleNode> while_stmt) {
    int parent_stmt_id = while_stmt->get_statement_id();
    auto child_stmt_list = while_stmt->get_child(1);
    int first_stmt_id = child_stmt_list->get_child(0)->get_statement_id();
    cfg[parent_stmt_id].insert(first_stmt_id);

    extract_cfg_from_stmt_list(pkb, proc_name, cfg, child_stmt_list);

    int child_stmt_list_size = child_stmt_list->get_children().size();
    auto last_stmt = child_stmt_list->get_child(child_stmt_list_size - 1);
    extract_cfg_from_stmt(pkb, proc_name, cfg, last_stmt, parent_stmt_id);
}

void DesignExtractor::extract_cfg_from_if_stmt(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::unordered_map<int, std::unordered_set<int>>& cfg,
    std::shared_ptr<Parser::SimpleNode> if_stmt, int next_block_stmt_id) {
    int parent_stmt_id = if_stmt->get_statement_id();
    for (int i = 1; i <= 2; i++) {
        auto branch_stmt_list = if_stmt->get_child(i);
        int first_stmt_id = branch_stmt_list->get_child(0)->get_statement_id();
        cfg[parent_stmt_id].insert(first_stmt_id);

        extract_cfg_from_stmt_list(pkb, proc_name, cfg, branch_stmt_list);

        int branch_stmt_list_size = branch_stmt_list->get_children().size();
        auto last_stmt = branch_stmt_list->get_child(branch_stmt_list_size - 1);
        extract_cfg_from_stmt(pkb, proc_name, cfg, last_stmt, next_block_stmt_id);
    }
}

void DesignExtractor::extract_next_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto cfg = pkb->get_cfg();
    for (auto kv : cfg) {
        int node_id = kv.first;
        std::unordered_set<int> neighbours = kv.second;
        for (int neighbour_id : neighbours) {
            pkb->add_next_relationship(node_id, neighbour_id);
        }
    }
}

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

void DesignExtractor::extract_follow_relationship_from_stmt_list(
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

void DesignExtractor::extract_follow_relationship_from_stmt(
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

void DesignExtractor::extract_parent_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list, int parent_stmt_id) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        extract_parent_relationship_from_stmt(pkb, proc_name, curr_stmt_node, parent_stmt_id);
    }
}

void DesignExtractor::extract_parent_relationship_from_stmt(
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

void DesignExtractor::extract_call_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
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

void DesignExtractor::extract_call_relationship_from_stmt_list(
    std::shared_ptr<KnowledgeBase::PKB> pkb, std::string proc_name,
    std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        extract_call_relationship_from_stmt(pkb, proc_name, curr_stmt_node);
    }
}

void DesignExtractor::extract_call_relationship_from_stmt(
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

struct pair_hash {
    template <class T1, class T2> std::size_t operator()(std::pair<T1, T2> const& pair) const {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
        return h1 ^ h2;
    }
};

void DesignExtractor::extract_affect_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    // Assume all entities and all other abstractions have been extracted and stored
    auto statements = pkb->get_statements();
    for (auto stmt : statements) {
        if (stmt->get_type() != KnowledgeBase::StatementType::ASSIGN) {
            continue;
        }
        int root_stmt_id = stmt->get_id();
        // There should only be one directly modified variable
        // This implementation allows for more than one
        auto direct_modifies_names = stmt->get_direct_modifies();
        auto direct_next_ids = stmt->get_direct_next();
        std::queue<std::pair<int, std::string>> bfs;
        std::unordered_set<std::pair<int, std::string>, pair_hash> visited;
        for (auto direct_modifies_name : *direct_modifies_names) {
            for (auto direct_next_id : *direct_next_ids) {
                bfs.push({direct_next_id, direct_modifies_name});
                visited.insert({direct_next_id, direct_modifies_name});
            }
        }
        while (bfs.size()) {
            auto curr = bfs.front();
            bfs.pop();
            int curr_stmt_id = curr.first;
            std::string curr_var_name = curr.second;
            auto curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
            auto curr_stmt_type = curr_stmt->get_type();
            if (curr_stmt_type == KnowledgeBase::StatementType::ASSIGN &&
                curr_stmt->get_direct_uses()->count(curr_var_name)) {
                pkb->add_affect_relationship(root_stmt_id, curr_stmt_id);
            }
            if (curr_stmt_type != KnowledgeBase::StatementType::IF &&
                curr_stmt_type != KnowledgeBase::StatementType::WHILE &&
                curr_stmt->get_modifies()->count(curr_var_name)) {
                continue;
            }
            auto curr_direct_next_ids = curr_stmt->get_direct_next();
            for (auto curr_direct_next_id : *curr_direct_next_ids) {
                if (visited.count({curr_direct_next_id, curr_var_name}) == 0) {
                    bfs.push({curr_direct_next_id, curr_var_name});
                    visited.insert({curr_direct_next_id, curr_var_name});
                }
            }
        }
    }
}

void DesignExtractor::extract_next_bip_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    // Assume all entities and all other abstractions have been extracted and stored
    std::queue<std::tuple<int, std::stack<int>, std::vector<int>>> bfs;
    auto procedures = pkb->get_procedures();
    for (auto proc : procedures) {
        if (proc->get_callers()->size()) {
            continue;
        }
        int curr_stmt_id = proc->get_first_statement();
        std::stack<int> trace;
        std::vector<int> path;
        bfs.push(std::make_tuple(curr_stmt_id, trace, path));
    }
    while (bfs.size()) {
        auto curr = bfs.front();
        bfs.pop();
        int curr_stmt_id = std::get<0>(curr);
        std::stack<int> curr_trace = std::get<1>(curr);
        std::vector<int> curr_path = std::get<2>(curr);
        auto curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
        auto curr_stmt_type = curr_stmt->get_type();
        // Populate PKB
        for (auto prev_stmt_id : curr_path) {
            pkb->add_indirect_next_bip_relationship(prev_stmt_id, curr_stmt_id);
        }
        if (curr_path.size()) {
            pkb->add_direct_next_bip_relationship(curr_path.back(), curr_stmt_id);
        }
        // Update states
        curr_path.push_back(curr_stmt_id);
        // BIP
        if (curr_stmt_type == KnowledgeBase::StatementType::CALL) {
            curr_trace.push(curr_stmt_id);
            std::string proc_called_name = curr_stmt->get_procedure_called();
            int next_stmt_id = pkb->get_procedure_by_name(proc_called_name)->get_first_statement();
            bfs.push(std::make_tuple(next_stmt_id, curr_trace, curr_path));
            continue;
        }
        if (curr_stmt_type == KnowledgeBase::StatementType::WHILE) {
            // Transitive extraction
            std::shared_ptr<const std::unordered_set<int>> loop_stmt_ids =
                extract_next_bip_relationship_from_while_loop(pkb, curr_stmt_id);
            // Populate PKB
            for (auto prev_stmt_id : curr_path) {
                for (auto next_stmt_id : *loop_stmt_ids) {
                    pkb->add_indirect_next_bip_relationship(prev_stmt_id, next_stmt_id);
                }
            }
            // Update states
            for (auto next_stmt_id : *loop_stmt_ids) {
                curr_path.push_back(next_stmt_id);
            }
            curr_path.push_back(curr_stmt_id);
        }
        while (true) {
            auto next_stmt_ids = curr_stmt->get_direct_next();
            if (next_stmt_ids->size()) {
                if (curr_stmt_type == KnowledgeBase::StatementType::WHILE) {
                    auto children_ids = curr_stmt->get_children();
                    for (auto next_stmt_id : *next_stmt_ids) {
                        // Ignore stmts inside WHILE loop
                        if (children_ids->count(next_stmt_id) == 0) {
                            bfs.push(std::make_tuple(next_stmt_id, curr_trace, curr_path));
                        }
                    }
                } else {
                    for (auto next_stmt_id : *next_stmt_ids) {
                        bfs.push(std::make_tuple(next_stmt_id, curr_trace, curr_path));
                    }
                }
                // Special check for WHILE
                // Attempt to backtrack if last statement in procedure
                if (curr_stmt_type != KnowledgeBase::StatementType::WHILE ||
                    next_stmt_ids->size() == 2) {
                    break;
                }
            }
            // Last statement in procedure
            if (curr_trace.size()) {
                // BIP
                curr_stmt_id = curr_trace.top();
                curr_trace.pop();
                curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
                curr_stmt_type = curr_stmt->get_type();
            } else {
                break;
            }
        }
    }
}

// Inspired by boost's hash_combine
// https://github.com/boostorg/container_hash/blob/171c012d4723c5e93cc7cffe42919afdf8b27dfa/include/boost/container_hash/hash.hpp#L310-L314
struct int_list_hash {
    std::size_t operator()(std::list<int> const& int_list) const {
        std::hash<int> int_hash;
        std::size_t result = 0;
        for (auto const& item : int_list) {
            result ^= int_hash(item) + 0x9e3779b9 + (result << 6) + (result >> 2);
        }
        return result;
    }
};

std::shared_ptr<std::unordered_set<int>>
DesignExtractor::extract_next_bip_relationship_from_while_loop(
    std::shared_ptr<KnowledgeBase::PKB> pkb, int while_stmt_id) {
    auto loop_stmt_ids = pkb->get_while_loop_stmt_ids(while_stmt_id);
    if (loop_stmt_ids->size()) {
        return loop_stmt_ids;
    }
    std::unordered_map<int, std::unordered_set<int>> next_map, prev_map;
    auto add_transitive_direct_next_bip_relationship = [&](int prev_stmt_id, int next_stmt_id) {
        pkb->add_direct_next_bip_relationship(prev_stmt_id, next_stmt_id);
        auto prev_set = prev_map[prev_stmt_id];
        prev_set.insert(prev_stmt_id);
        auto next_set = next_map[next_stmt_id];
        next_set.insert(next_stmt_id);
        for (auto curr_prev_id : prev_set) {
            for (auto curr_next_id : next_set) {
                next_map[curr_prev_id].insert(curr_next_id);
                prev_map[curr_next_id].insert(curr_prev_id);
            }
        }
    };
    std::unordered_map<std::list<int>, std::unordered_set<int>, int_list_hash> visited;
    std::queue<std::tuple<int, std::list<int>>> bfs;
    auto while_stmt = pkb->get_statement_by_id(while_stmt_id);
    auto next_stmt_ids = while_stmt->get_direct_next();
    auto children_ids = while_stmt->get_children();
    for (auto next_stmt_id : *next_stmt_ids) {
        // Ignore stmts outside WHILE loop
        if (children_ids->count(next_stmt_id)) {
            std::list<int> trace;
            visited[trace].insert(while_stmt_id);
            bfs.push(std::make_tuple(next_stmt_id, trace));
            add_transitive_direct_next_bip_relationship(while_stmt_id, next_stmt_id);
        }
    }
    while (bfs.size()) {
        auto curr = bfs.front();
        bfs.pop();
        int curr_stmt_id = std::get<0>(curr);
        std::list<int> curr_trace = std::get<1>(curr);
        if (visited[curr_trace].count(curr_stmt_id)) {
            continue;
        }
        visited[curr_trace].insert(curr_stmt_id);
        loop_stmt_ids->insert(curr_stmt_id);
        auto curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
        auto curr_stmt_type = curr_stmt->get_type();
        if (curr_stmt_type == KnowledgeBase::StatementType::CALL) {
            // BIP
            curr_trace.push_back(curr_stmt_id);
            std::string proc_called_name = curr_stmt->get_procedure_called();
            int next_stmt_id = pkb->get_procedure_by_name(proc_called_name)->get_first_statement();
            bfs.push(std::make_tuple(next_stmt_id, curr_trace));
            add_transitive_direct_next_bip_relationship(curr_stmt_id, next_stmt_id);
            continue;
        }
        int prev_stmt_id = curr_stmt_id;
        while (true) {
            auto next_stmt_ids = curr_stmt->get_direct_next();
            if (next_stmt_ids->size()) {
                for (auto next_stmt_id : *next_stmt_ids) {
                    bfs.push(std::make_tuple(next_stmt_id, curr_trace));
                    add_transitive_direct_next_bip_relationship(prev_stmt_id, next_stmt_id);
                }
                // Special check for WHILE
                // Attempt to backtrack if last statement in procedure
                if (curr_stmt_type != KnowledgeBase::StatementType::WHILE ||
                    next_stmt_ids->size() == 2) {
                    break;
                }
            }
            // Last statement in procedure
            if (curr_trace.size()) {
                // BIP
                curr_stmt_id = curr_trace.back();
                curr_trace.pop_back();
                curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
                curr_stmt_type = curr_stmt->get_type();
            } else {
                break;
            }
        }
    }
    // Populate PKB
    for (auto kv : next_map) {
        int prev_stmt_id = kv.first;
        for (auto next_stmt_id : kv.second) {
            pkb->add_indirect_next_bip_relationship(prev_stmt_id, next_stmt_id);
        }
    }
    return loop_stmt_ids;
}

void DesignExtractor::extract_affect_bip_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    // Assume all entities and all other abstractions have been extracted and stored
    std::queue<std::tuple<int, std::stack<int>, std::unordered_map<int, int>,
                          std::unordered_map<int, std::unordered_set<int>>,
                          std::unordered_map<std::string, int>>>
        bfs;
    auto procedures = pkb->get_procedures();
    for (auto proc : procedures) {
        if (proc->get_callers()->size()) {
            continue;
        }
        int curr_stmt_id = proc->get_first_statement();
        std::stack<int> trace;
        std::unordered_map<int, int> visited;
        std::unordered_map<int, std::unordered_set<int>> affected_by;
        std::unordered_map<std::string, int> modified_by;
        bfs.push(std::make_tuple(curr_stmt_id, trace, visited, affected_by, modified_by));
    }
    while (bfs.size()) {
        auto curr = bfs.front();
        bfs.pop();
        int curr_stmt_id = std::get<0>(curr);
        std::stack<int> curr_trace = std::get<1>(curr);
        std::unordered_map<int, int> curr_visited = std::get<2>(curr);
        std::unordered_map<int, std::unordered_set<int>> curr_affected_by = std::get<3>(curr);
        std::unordered_map<std::string, int> curr_modified_by = std::get<4>(curr);
        auto curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
        auto curr_stmt_type = curr_stmt->get_type();
        // Populate PKB and update states
        if (curr_stmt_type == KnowledgeBase::StatementType::ASSIGN) {
            auto uses = curr_stmt->get_direct_uses();
            for (auto var : *uses) {
                if (curr_modified_by.count(var)) {
                    int affects_id = curr_modified_by[var];
                    if (affects_id == -1) {
                        continue;
                    }
                    if (curr_affected_by.count(curr_stmt_id) == 0) {
                        curr_affected_by[curr_stmt_id] = std::unordered_set<int>();
                    }
                    pkb->add_direct_affect_bip_relationship(affects_id, curr_stmt_id);
                    curr_affected_by[curr_stmt_id].insert(affects_id);
                    if (curr_affected_by.count(affects_id)) {
                        for (auto indirect_affects_id : curr_affected_by[affects_id]) {
                            pkb->add_indirect_affect_bip_relationship(indirect_affects_id,
                                                                      curr_stmt_id);
                            curr_affected_by[curr_stmt_id].insert(indirect_affects_id);
                        }
                    }
                }
            }
            auto modifies = curr_stmt->get_direct_modifies();
            for (auto var : *modifies) {
                curr_modified_by[var] = curr_stmt_id;
            }
        } else if (curr_stmt_type == KnowledgeBase::StatementType::READ) {
            auto modifies = curr_stmt->get_direct_modifies();
            for (auto var : *modifies) {
                curr_modified_by[var] = -1;
            }
        }
        if (curr_visited.count(curr_stmt_id) == 0) {
            curr_visited[curr_stmt_id] = 0;
        }
        curr_visited[curr_stmt_id] += 1;
        // Traverse in CFG BIP
        if (curr_stmt_type == KnowledgeBase::StatementType::CALL) {
            // BIP
            curr_trace.push(curr_stmt_id);
            std::string proc_called_name = curr_stmt->get_procedure_called();
            int next_stmt_id = pkb->get_procedure_by_name(proc_called_name)->get_first_statement();
            bfs.push(std::make_tuple(next_stmt_id, curr_trace, curr_visited, curr_affected_by,
                                     curr_modified_by));
        } else {
            while (true) {
                auto next_stmt_ids = curr_stmt->get_direct_next();
                if (next_stmt_ids->size()) {
                    for (auto next_stmt_id : *next_stmt_ids) {
                        if (curr_visited.count(next_stmt_id) && curr_visited[next_stmt_id] > 2) {
                            auto next_stmt = pkb->get_statement_by_id(next_stmt_id);
                            auto next_stmt_type = next_stmt->get_type();
                            if (next_stmt_type == KnowledgeBase::StatementType::WHILE) {
                                continue;
                            }
                        }
                        bfs.push(std::make_tuple(next_stmt_id, curr_trace, curr_visited,
                                                 curr_affected_by, curr_modified_by));
                    }
                    // Special check for WHILE
                    // Attempt to backtrack if last statement in procedure
                    if (curr_stmt_type != KnowledgeBase::StatementType::WHILE ||
                        next_stmt_ids->size() == 2) {
                        break;
                    }
                }
                // Last statement in procedure
                if (curr_trace.size()) {
                    // BIP
                    curr_stmt_id = curr_trace.top();
                    curr_trace.pop();
                    curr_stmt = pkb->get_statement_by_id(curr_stmt_id);
                    curr_stmt_type = curr_stmt->get_type();
                } else {
                    break;
                }
            }
        }
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
