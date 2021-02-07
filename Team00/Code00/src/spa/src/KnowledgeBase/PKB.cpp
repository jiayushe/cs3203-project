#include "PKB.h"

using namespace KnowledgeBase;

PKB::PKB(std::shared_ptr<Parser::SimpleNode>) : ast(ast) {
    procedures = new std::unordered_map<std::string, Procedure>();
    statements = new std::unordered_map<int, Statement>();
    variables = new std::unordered_map<std::string, Variable>();
    constants = new std::unordered_map<int, Constant>();
}

PKB::~PKB() {
    delete procedures;
    delete statements;
    delete variables;
    delete constants;
}

std::shared_ptr<Parser::SimpleNode> PKB::get_ast() { return ast; }

std::vector<Procedure> PKB::get_procedures() {
    std::vector<Procedure> res;
    for (auto kv : *procedures) {
        res.push_back(kv.second);
    }
    return res;
}

Procedure PKB::get_procedure_by_name(std::string name) {
    if (procedures->find(name) == procedures->end()) {
        throw "Procedure " + name + " does not exist";
    }
    return procedures->at(name);
}

Procedure PKB::add_procedure(std::string name) {
    std::unordered_map<std::string, Procedure>::const_iterator it = procedures->find(name);
    if (it != procedures->end()) {
        return it->second;
    }
    Procedure proc(name);
    procedures->insert({name, proc});
    return proc;
}

std::vector<Statement> PKB::get_statements() {
    std::vector<Statement> res;
    for (auto kv : *statements) {
        res.push_back(kv.second);
    }
    return res;
}

Statement PKB::get_statement_by_id(int id) {
    if (statements->find(id) == statements->end()) {
        throw "Statement " + std::to_string(id) + " does not exist";
    }
    return statements->at(id);
}

Statement PKB::add_statement(StatementType type, int id, std::string procedure_name,
                             std::shared_ptr<Parser::SimpleNode> pattern) {
    std::unordered_map<int, Statement>::const_iterator it = statements->find(id);
    if (it != statements->end()) {
        return it->second;
    }
    Statement stmt(type, id, procedure_name, pattern);
    statements->insert({id, stmt});
    return stmt;
}

std::vector<Variable> PKB::get_variables() {
    std::vector<Variable> res;
    for (auto kv : *variables) {
        res.push_back(kv.second);
    }
    return res;
}

Variable PKB::get_variable_by_name(std::string name) {
    if (variables->find(name) == variables->end()) {
        throw "Variable " + name + " does not exist";
    }
    return variables->at(name);
}

Variable PKB::add_variable(std::string name) {
    std::unordered_map<std::string, Variable>::const_iterator it = variables->find(name);
    if (it != variables->end()) {
        return it->second;
    }
    Variable var(name);
    variables->insert({name, var});
    return var;
}

std::vector<Constant> PKB::get_constants() {
    std::vector<Constant> res;
    for (auto kv : *constants) {
        res.push_back(kv.second);
    }
    return res;
}

void PKB::add_modify_relationship(int stmt_id, std::string var_name) {
    Statement stmt = this->get_statement_by_id(stmt_id);
    Variable var = this->get_variable_by_name(var_name);
    stmt.add_modifies(var_name);
    var.add_modified_by(stmt_id);
}

void PKB::add_use_relationship(int stmt_id, std::string var_name) {
    Statement stmt = this->get_statement_by_id(stmt_id);
    Variable var = this->get_variable_by_name(var_name);
    stmt.add_uses(var_name);
    var.add_used_by(stmt_id);
}

void PKB::add_follow_relationship(int following_id, int follower_id) {
    if (follower_id == -1) {
        return;
    }
    Statement following_stmt = this->get_statement_by_id(following_id);
    Statement follower_stmt = this->get_statement_by_id(follower_id);
    following_stmt.set_follower(follower_id);
    follower_stmt.set_following(following_id);
}

void PKB::add_parent_relationship(int parent_id, int child_id) {
    Statement parent_stmt = this->get_statement_by_id(parent_id);
    Statement child_stmt = this->get_statement_by_id(child_id);
    parent_stmt.add_child(child_id);
    child_stmt.set_parent(parent_id);
}
