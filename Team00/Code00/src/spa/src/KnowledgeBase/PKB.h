#pragma once

#include "Constant.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Procedure.h"
#include "Statement.h"
#include "Variable.h"
#include <string>
#include <unordered_map>

namespace KnowledgeBase {

class PKB {
public:
    PKB(std::shared_ptr<Parser::SimpleNode> ast);

    ~PKB();

    // Gets the base AST.
    std::shared_ptr<Parser::SimpleNode> get_ast();

    // Gets the list of all procedures.
    std::vector<Procedure> get_procedures();

    // Gets the procedure specified by the given name.
    // Throws if the given procedure name does not exist.
    Procedure get_procedure_by_name(std::string name);

    // Creates a procedure with the given name and adds it to the table.
    Procedure add_procedure(std::string name);

    // Gets the list of all statements.
    std::vector<Statement> get_statements();

    // Gets the statement specified by the given id.
    // Throws if the given id is invalid.
    Statement get_statement_by_id(int id);

    // Creates a statement with the given id and adds it to the table.
    Statement add_statement(StatementType type, int id, std::string proc_name,
                            std::shared_ptr<Parser::SimpleNode> pattern);

    // Gets the list of all variables.
    std::vector<Variable> get_variables();

    // Gets the variable specified by the given name.
    // Throws if the given variable name does not exist.
    Variable get_variable_by_name(std::string name);

    // Creates a variable with the given name and adds it to the table.
    Variable add_variable(std::string name);

    // Gets the list of all constants.
    std::vector<Constant> get_constants();

    // Gets the constant with the given value.
    // Throws if the given constant value does not exist.
    Constant get_constant_by_value(int value);

    // Creates a constant with the given value and adds it to the table.
    Constant add_constant(int value);

    // Adds a modify relationship between a statement id and a variable name.
    void add_modify_relationship(int stmt_id, std::string var_name);

    // Adds a use relationship between a statement id and a variable name.
    void add_use_relationship(int stmt_id, std::string var_name);

    // Adds a follow relationship between two statement ids.
    void add_follow_relationship(int following_id, int follower_id);

    // Adds a parent-child relationship between two statement ids.
    void add_parent_relationship(int parent_id, int child_id);

private:
    // Base AST
    std::shared_ptr<Parser::SimpleNode> ast;
    // Procedure table which maps procedure names to procedures
    std::unordered_map<std::string, Procedure>* procedures;
    // Statement table which maps statement ids to statements
    std::unordered_map<int, Statement>* statements;
    // Variable table which maps variable names to variables
    std::unordered_map<std::string, Variable>* variables;
    // Constant table which maps values to constants
    std::unordered_map<int, Constant>* constants;
};

} // namespace KnowledgeBase
