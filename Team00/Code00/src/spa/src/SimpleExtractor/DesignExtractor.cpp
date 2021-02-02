#include "DesignExtractor.h"
#include "KnowledgeBase/Procedure.h"
#include "Parser/SimpleParser/SimpleNode.h"

using namespace SimpleExtractor;

void DesignExtractor::extract_modify_relationship(KnowledgeBase::PKB pkb) {
    auto ast = pkb.get_ast();
    // Assume the base node is program and contains only one procedure
    if (ast->get_type() != Parser::SimpleNodeType::PROGRAM) {
        throw "Base node of AST is not a program";
    }
    auto proc_node = ast->get_child(0);
    std::string proc_name = proc_node->get_child(0)->get_value();
    KnowledgeBase::Procedure proc = pkb.add_procedure(proc_name);
    auto stmt_list = ast->get_child(1);
    extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
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
    std::shared_ptr<Parser::SimpleNode> stmt_list;
    switch (stmt->get_type()) {
    case Parser::SimpleNodeType::READ:
        pkb.add_statement(KnowledgeBase::StatementType::READ, stmt->get_statement_id(), proc_name,
                          nullptr);
        extract_modify_relationship_from_read_stmt(pkb, proc_name, stmt);
        break;
    case Parser::SimpleNodeType::PRINT:
        pkb.add_statement(KnowledgeBase::StatementType::PRINT, stmt->get_statement_id(), proc_name,
                          nullptr);
        // PRINT statements do not have modify relationship
        break;
    case Parser::SimpleNodeType::CALL:
        pkb.add_statement(KnowledgeBase::StatementType::CALL, stmt->get_statement_id(), proc_name,
                          nullptr);
        // CALL statements do not have direct modify relationship
        // Assume no nested procedure calls
        break;
    case Parser::SimpleNodeType::WHILE:
        pkb.add_statement(KnowledgeBase::StatementType::WHILE, stmt->get_statement_id(), proc_name,
                          nullptr);
        // WHILE statements do not have direct modify relationship
        stmt_list = stmt->get_child(1);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case Parser::SimpleNodeType::IF:
        pkb.add_statement(KnowledgeBase::StatementType::IF, stmt->get_statement_id(), proc_name,
                          nullptr);
        // IF statements do not have direct modify relationship
        stmt_list = stmt->get_child(1);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        stmt_list = stmt->get_child(2);
        extract_modify_relationship_from_stmt_list(pkb, proc_name, stmt_list);
        break;
    case Parser::SimpleNodeType::ASSIGN:
        pkb.add_statement(KnowledgeBase::StatementType::ASSIGN, stmt->get_statement_id(), proc_name,
                          stmt);
        extract_modify_relationship_from_assign_stmt(pkb, proc_name, stmt);
        break;
    }
}

void DesignExtractor::extract_modify_relationship_from_read_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    std::string var_name = var_node->get_value();
    pkb.add_variable(var_name);
    int stmt_id = stmt->get_statement_id();
    pkb.add_modify_relationship(stmt_id, var_name);
}

void DesignExtractor::extract_modify_relationship_from_assign_stmt(
    KnowledgeBase::PKB pkb, std::string proc_name, std::shared_ptr<Parser::SimpleNode> stmt) {
    auto var_node = stmt->get_child(0);
    std::string var_name = var_node->get_value();
    pkb.add_variable(var_name);
    int stmt_id = stmt->get_statement_id();
    pkb.add_modify_relationship(stmt_id, var_name);
}

void DesignExtractor::extract_use_relationship(KnowledgeBase::PKB pkb) {}

void DesignExtractor::extract_follow_relationship(KnowledgeBase::PKB pkb) {}

void DesignExtractor::extract_parent_relationship(KnowledgeBase::PKB pkb) {}
