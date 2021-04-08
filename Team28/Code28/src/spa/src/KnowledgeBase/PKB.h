#pragma once

#include "Constant.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Procedure.h"
#include "Statement.h"
#include "Variable.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace KnowledgeBase {

class PKB {
public:
    PKB(std::shared_ptr<Parser::SimpleNode> ast);

    ~PKB();

    // Gets the base AST.
    std::shared_ptr<Parser::SimpleNode> get_ast();

    // Gets the control flow graph.
    std::unordered_map<int, std::unordered_set<int>> get_cfg();

    // Sets the control flow graph.
    void set_cfg(std::unordered_map<int, std::unordered_set<int>> cfg);

    // Gets the list of all procedures.
    std::vector<std::shared_ptr<Procedure>> get_procedures();

    // Gets the procedure specified by the given name.
    // Throws if the given procedure name does not exist.
    std::shared_ptr<Procedure> get_procedure_by_name(std::string name);

    // Creates a procedure with the given name and adds it to the table.
    std::shared_ptr<Procedure> add_procedure(std::string name);

    // Gets the list of all statements.
    std::vector<std::shared_ptr<Statement>> get_statements();

    // Gets the statement specified by the given id.
    // Throws if the given id is invalid.
    std::shared_ptr<Statement> get_statement_by_id(int id);

    // Creates a statement with the given id and adds it to the table.
    std::shared_ptr<Statement> add_statement(StatementType type, int id, std::string proc_name,
                                             std::shared_ptr<Parser::SimpleNode> pattern);

    // Gets the list of all variables.
    std::vector<std::shared_ptr<Variable>> get_variables();

    // Gets the variable specified by the given name.
    // Throws if the given variable name does not exist.
    std::shared_ptr<Variable> get_variable_by_name(std::string name);

    // Creates a variable with the given name and adds it to the table.
    std::shared_ptr<Variable> add_variable(std::string name);

    // Gets the list of all constants.
    std::vector<std::shared_ptr<Constant>> get_constants();

    // Gets the constant with the given value.
    // Throws if the given constant value does not exist.
    std::shared_ptr<Constant> get_constant_by_value(int value);

    // Creates a constant with the given value and adds it to the table.
    std::shared_ptr<Constant> add_constant(int value);

    // Adds a modify relationship between a statement id and a variable name.
    void add_modify_relationship(int stmt_id, std::string var_name, bool direct = true);

    // Adds a use relationship between a statement id and a variable name.
    void add_use_relationship(int stmt_id, std::string var_name, bool direct = true);

    // Adds a follow relationship between two statement ids.
    void add_follow_relationship(int following_id, int follower_id);

    // Adds a parent-child relationship between two statement ids.
    void add_parent_relationship(int parent_id, int child_id);

    // Adds a next relationship between two prog line ids.
    void add_next_relationship(int prev_id, int next_id);

    // Adds a call relationship between two procedure names.
    void add_call_relationship(std::string caller_name, std::string callee_name);

    // Adds an affect relationship between two assignment statement ids.
    void add_affect_relationship(int affects_id, int affected_id);

    // Adds a direct next bip relationship between two prog line ids.
    void add_direct_next_bip_relationship(int prev_id, int next_id);

    // Adds an indirect next bip relationship between two prog line ids.
    void add_indirect_next_bip_relationship(int prev_id, int next_id);

    // Adds a direct affect bip relationship between two assignment statement ids.
    void add_direct_affect_bip_relationship(int affects_id, int affected_id);

    // Adds an indirect affect bip relationship between two assignment statement ids.
    void add_indirect_affect_bip_relationship(int affects_id, int affected_id);

private:
    // Base AST
    std::shared_ptr<Parser::SimpleNode> ast;
    // Procedure table which maps procedure names to procedures
    std::unordered_map<std::string, std::shared_ptr<Procedure>> procedures;
    // Statement table which maps statement ids to statements
    std::unordered_map<int, std::shared_ptr<Statement>> statements;
    // Variable table which maps variable names to variables
    std::unordered_map<std::string, std::shared_ptr<Variable>> variables;
    // Constant table which maps values to constants
    std::unordered_map<int, std::shared_ptr<Constant>> constants;
    // Control Flow Graph, stored as an adjacency list (map) from prog_line/stmt_id to a set of
    // prog_lines/stmt_ids
    std::unordered_map<int, std::unordered_set<int>> cfg;
};

} // namespace KnowledgeBase
