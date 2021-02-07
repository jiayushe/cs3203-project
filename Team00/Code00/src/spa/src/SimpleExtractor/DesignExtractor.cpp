#include "DesignExtractor.h"
#include "KnowledgeBase/Procedure.h"
#include "Parser/SimpleParser/SimpleNode.h"

using namespace SimpleExtractor;

void DesignExtractor::extract_modify_relationship(KnowledgeBase::PKB pkb) {
    auto ast = pkb.get_ast();
    assert_node_type(ast, Parser::SimpleNodeType::PROGRAM);
    KnowledgeBase::Procedure proc = extract_procedure(pkb, ast);
    auto proc_node = ast->get_child(0);
    std::shared_ptr<Parser::SimpleNode> stmt_list_node = proc_node->get_child(1);
    extract_modify_relationship_from_stmt_list(pkb, proc.get_name(), stmt_list_node);
}

void DesignExtractor::extract_modify_relationship_from_stmt_list(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        extract_modify_relationship_from_stmt(pkb, proc_name, stmts[i]);
    }
}

void DesignExtractor::extract_modify_relationship_from_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    KnowledgeBase::Statement curr_stmt = extract_statement(pkb, proc_name, stmt);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt.get_type()) {
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
        curr_stmt.set_pattern(stmt->get_child(1));
        extract_modify_relationship_from_assign_stmt(pkb, proc_name, stmt);
        break;
    }
}

void DesignExtractor::extract_modify_relationship_from_read_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    KnowledgeBase::Variable var = extract_variable(pkb, var_node);
    pkb.add_modify_relationship(stmt->get_statement_id(), var.get_name());
}

void DesignExtractor::extract_modify_relationship_from_assign_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    KnowledgeBase::Variable var = extract_variable(pkb, var_node);
    pkb.add_modify_relationship(stmt->get_statement_id(), var.get_name());
}

void DesignExtractor::extract_use_relationship(KnowledgeBase::PKB pkb) {
    auto ast = pkb.get_ast();
    assert_node_type(ast, Parser::SimpleNodeType::PROGRAM);
    KnowledgeBase::Procedure proc = extract_procedure(pkb, ast);
    auto proc_node = ast->get_child(0);
    std::shared_ptr<Parser::SimpleNode> stmt_list_node = proc_node->get_child(1);
    extract_use_relationship_from_stmt_list(pkb, proc.get_name(), stmt_list_node);
}

void DesignExtractor::extract_use_relationship_from_stmt(KnowledgeBase::PKB pkb,
                                                         std::string proc_name,
                                                         std::shared_ptr<Parser::SimpleNode> stmt) {
    KnowledgeBase::Statement curr_stmt = extract_statement(pkb, proc_name, stmt);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt.get_type()) {
    case KnowledgeBase::StatementType::READ:
        // READ statements do not have use relationship
        extract_modify_relationship_from_read_stmt(pkb, proc_name, stmt);
        break;
    case KnowledgeBase::StatementType::PRINT:
        extract_use_relationship_from_print_stmt(pkb, proc_name, stmt);
        break;
    case KnowledgeBase::StatementType::CALL:
        // CALL statements do not have direct use relationship
        // Assume no nested procedure calls
        break;
    case KnowledgeBase::StatementType::WHILE:
        extract_use_relationship_from_arithmetic_or_conditional_stmt(pkb, proc_name,
                                                                     stmt->get_child(0));
        stmt_list = stmt->get_child(1);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        extract_use_relationship_from_arithmetic_or_conditional_stmt(pkb, proc_name,
                                                                     stmt->get_child(0));
        stmt_list = stmt->get_child(1);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        stmt_list = stmt->get_child(2);
        extract_use_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::ASSIGN:
        curr_stmt.set_pattern(stmt->get_child(1));
        extract_use_relationship_from_assign_stmt(pkb, proc_name, stmt);
        break;
    }
}

void DesignExtractor::extract_use_relationship_from_print_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    KnowledgeBase::Variable var = extract_variable(pkb, var_node);
    pkb.add_use_relationship(stmt->get_statement_id(), var.get_name());
}

void DesignExtractor::extract_use_relationship_from_assign_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    extract_use_relationship_from_arithmetic_or_conditional_stmt(pkb, proc_name,
                                                                 stmt->get_child(1));
}

void DesignExtractor::extract_use_relationship_from_arithmetic_or_conditional_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto children_nodes = stmt->get_children();
    int num_children = children_nodes.size();
    for (int i = 0; i < num_children; i++) {
        auto child_node = children_nodes[i];
        switch (child_node->get_type()) {
        case Parser::SimpleNodeType::ARITHMETIC:
        case Parser::SimpleNodeType::CONDITIONAL:
            extract_use_relationship_from_arithmetic_or_conditional_stmt(pkb, proc_name,
                                                                         child_node);
            break;
        case Parser::SimpleNodeType::CONST_VALUE:
            break;
        case Parser::SimpleNodeType::VAR_NAME:
            KnowledgeBase::Variable var = extract_variable(pkb, child_node);
            pkb.add_use_relationship(stmt->get_statement_id(), var.get_name());
            break;
        }
    }
}

void DesignExtractor::extract_follow_relationship(KnowledgeBase::PKB pkb) {
    std::shared_ptr<Parser::SimpleNode> ast = pkb.get_ast();
    assert_node_type(ast, Parser::SimpleNodeType::PROGRAM);
    KnowledgeBase::Procedure proc = extract_procedure(pkb, ast);
    auto proc_node = ast->get_child(0);
    std::shared_ptr<Parser::SimpleNode> stmt_list_node = proc_node->get_child(1);
    extract_follow_relationship_from_stmt_list(pkb, proc.get_name(), stmt_list_node);
}

void SimpleExtractor::DesignExtractor::extract_follow_relationship_from_stmt_list(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt_list) {
    auto stmts = stmt_list->get_children();
    int num_stmt = stmts.size();
    for (int i = 0; i < num_stmt; i++) {
        std::shared_ptr<Parser::SimpleNode> curr_stmt_node = stmts.at(i);
        int next_stmt_id = i < num_stmt - 1 ? stmts.at(i + 1)->get_statement_id() : -1;
        extract_follow_relationship_from_stmt(pkb, proc_name, curr_stmt_node, next_stmt_id);
    }
}

void SimpleExtractor::DesignExtractor::extract_follow_relationship_from_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt,
    int next_stmt_id) {
    KnowledgeBase::Statement curr_stmt = extract_statement(pkb, proc_name, stmt);
    pkb.add_follow_relationship(curr_stmt.get_id(), next_stmt_id);
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (curr_stmt.get_type()) {
    case KnowledgeBase::StatementType::WHILE:
        // Process nested statements in WHILE
        stmt_list = stmt->get_child(1);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::IF:
        // Process then and else branch statements in IF
        stmt_list = stmt->get_child(1);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        stmt_list = stmt->get_child(2);
        extract_follow_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case KnowledgeBase::StatementType::ASSIGN:
        curr_stmt.set_pattern(stmt->get_child(1));
        break;
    default:
        break;
    }
}

void DesignExtractor::extract_parent_relationship(KnowledgeBase::PKB pkb) {}

KnowledgeBase::Procedure
DesignExtractor::extract_procedure(KnowledgeBase::PKB pkb,
                                   std::shared_ptr<Parser::SimpleNode> ast) {
    auto proc_node = ast->get_child(0);
    std::string proc_name = proc_node->get_child(0)->get_value();
    return pkb.add_procedure(proc_name);
}

KnowledgeBase::Statement
DesignExtractor::extract_statement(KnowledgeBase::PKB pkb, std::string proc_name,
                                   std::shared_ptr<Parser::SimpleNode> stmt_node) {
    int stmt_id = stmt_node->get_statement_id();
    KnowledgeBase::StatementType statement_type =
        convert_node_type_to_stmt_type(stmt_node->get_type());
    return pkb.add_statement(statement_type, stmt_id, proc_name, nullptr);
}

KnowledgeBase::Variable
DesignExtractor::extract_variable(KnowledgeBase::PKB pkb,
                                  std::shared_ptr<Parser::SimpleNode> var_node) {
    std::string var_name = var_node->get_value();
    return pkb.add_variable(var_name);
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

void DesignExtractor::assert_node_type(std::shared_ptr<Parser::SimpleNode> node,
                                       Parser::SimpleNodeType expected_type) {
    Parser::SimpleNodeType actual_type = node->get_type();
    if (actual_type != expected_type) {
        throw "Expected " + to_string(expected_type) + ", but encountered " +
            to_string(actual_type);
    }
}
