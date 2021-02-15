#include "DesignExtractor.h"
#include "KnowledgeBase/Procedure.h"
#include "Parser/SimpleParser/SimpleNode.h"

using namespace SimpleExtractor;

void DesignExtractor::extract_modify_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    auto proc_node = assert_node_type(ast->get_child(0), Parser::SimpleNodeType::PROCEDURE);
    auto proc = extract_procedure(pkb, proc_node);
    auto stmt_list_node =
        assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
    extract_modify_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
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
        // CALL statements do not have direct modify relationship
        // Assume no nested procedure calls
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
        throw "Unhandled statement type";
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
    auto proc_node = assert_node_type(ast->get_child(0), Parser::SimpleNodeType::PROCEDURE);
    auto proc = extract_procedure(pkb, proc_node);
    auto stmt_list_node =
        assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
    extract_use_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
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
        // CALL statements do not have direct use relationship
        // Assume no nested procedure calls
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
        throw "Unhandled statement type";
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
        throw "Unhandled simple node type";
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
            throw "Unhandled simple node type";
        }
    }
}

void DesignExtractor::extract_follow_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    auto proc_node = assert_node_type(ast->get_child(0), Parser::SimpleNodeType::PROCEDURE);
    auto proc = extract_procedure(pkb, proc_node);
    auto stmt_list_node =
        assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
    extract_follow_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node);
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
        break;
    }
}

void DesignExtractor::extract_parent_relationship(std::shared_ptr<KnowledgeBase::PKB> pkb) {
    auto ast = assert_node_type(pkb->get_ast(), Parser::SimpleNodeType::PROGRAM);
    auto proc_node = assert_node_type(ast->get_child(0), Parser::SimpleNodeType::PROCEDURE);
    auto proc = extract_procedure(pkb, proc_node);
    auto stmt_list_node =
        assert_node_type(proc_node->get_child(1), Parser::SimpleNodeType::STMT_LST);
    extract_parent_relationship_from_stmt_list(pkb, proc->get_name(), stmt_list_node, -1);
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
        break;
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
        throw "The AST node is not a statement node.";
    }
}

std::shared_ptr<Parser::SimpleNode>
DesignExtractor::assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                                  Parser::SimpleNodeType expected_type) {
    Parser::SimpleNodeType actual_type = node->get_type();
    if (actual_type != expected_type) {
        throw "Expected " + to_string(expected_type) + ", but encountered " +
            to_string(actual_type);
    }
    return node;
}
