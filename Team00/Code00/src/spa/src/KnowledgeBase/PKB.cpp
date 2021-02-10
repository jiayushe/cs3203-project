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

Statement PKB::add_statement(StatementType type, int id, std::string proc_name,
                             std::shared_ptr<Parser::SimpleNode> stmt_node) {
    std::unordered_map<int, Statement>::const_iterator it = statements->find(id);
    if (it != statements->end()) {
        return it->second;
    }
    Statement stmt(type, id, proc_name);
    statements->insert({id, stmt});
    Procedure proc = this->get_procedure_by_name(proc_name);
    proc.add_statement(id);
    std::string proc_called_name;
    Procedure proc_called;
    switch (type) {
    case KnowledgeBase::StatementType::ASSIGN:
        stmt.set_pattern(stmt_node->get_child(1));
        break;
    case KnowledgeBase::StatementType::CALL:
        proc_called_name = stmt_node->get_child(0)->get_value();
        stmt.set_procedure_called(proc_called_name);
        proc_called = this->get_procedure_by_name(proc_called_name);
        proc_called.add_called_by(id);
        break;
    }
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

Constant PKB::get_constant_by_value(int value) {
    if (constants->find(value) == constants->end()) {
        throw "Constant " + std::to_string(value) + " does not exist";
    }
    return constants->at(value);
}

Constant PKB::add_constant(int value) {
    std::unordered_map<int, Constant>::const_iterator it = constants->find(value);
    if (it != constants->end()) {
        return it->second;
    }
    Constant cst(value);
    constants->insert({value, cst});
    return cst;
}

void PKB::add_modify_relationship(int stmt_id, std::string var_name) {
    Statement stmt = this->get_statement_by_id(stmt_id);
    Variable var = this->get_variable_by_name(var_name);
    stmt.add_direct_modifies(var_name);
    stmt.add_modifies(var_name);
    var.add_direct_modified_by(stmt_id);
    var.add_modified_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt.get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        Statement curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt.add_modifies(var_name);
        var.add_modified_by(curr_ancestor_id);
    }
}

void PKB::add_use_relationship(int stmt_id, std::string var_name) {
    Statement stmt = this->get_statement_by_id(stmt_id);
    Variable var = this->get_variable_by_name(var_name);
    stmt.add_direct_uses(var_name);
    stmt.add_uses(var_name);
    var.add_direct_used_by(stmt_id);
    var.add_used_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt.get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        Statement curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt.add_uses(var_name);
        var.add_used_by(curr_ancestor_id);
    }
}

void PKB::add_follow_relationship(int following_id, int follower_id) {
    if (follower_id == -1) {
        return;
    }
    Statement following_stmt = this->get_statement_by_id(following_id);
    Statement follower_stmt = this->get_statement_by_id(follower_id);
    following_stmt.set_direct_follower(follower_id);
    follower_stmt.set_direct_following(following_id);
    // Extract all followings and followers.
    std::unordered_set<int> following_ids = following_stmt.get_followings();
    following_ids.insert(following_id);
    std::unordered_set<int> follower_ids = follower_stmt.get_followers();
    follower_ids.insert(follower_id);
    for (const auto& curr_following_id : following_ids) {
        Statement curr_following_stmt = this->get_statement_by_id(curr_following_id);
        for (const auto& curr_follower_id : follower_ids) {
            Statement curr_follower_stmt = this->get_statement_by_id(curr_follower_id);
            curr_following_stmt.add_follower(curr_follower_id);
            curr_follower_stmt.add_following(curr_following_id);
        }
    }
}

void PKB::add_parent_relationship(int parent_id, int child_id) {
    if (parent_id == -1) {
        return;
    }
    Statement parent_stmt = this->get_statement_by_id(parent_id);
    Statement child_stmt = this->get_statement_by_id(child_id);
    parent_stmt.add_child(child_id);
    child_stmt.set_parent(parent_id);
    // Extract all ancestors and descendants.
    std::unordered_set<int> ancestor_ids = parent_stmt.get_ancestors();
    ancestor_ids.insert(parent_id);
    std::unordered_set<int> descendant_ids = child_stmt.get_descendants();
    descendant_ids.insert(child_id);
    for (const auto& curr_ancestor_id : ancestor_ids) {
        Statement curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        for (const auto& curr_descendant_id : descendant_ids) {
            Statement curr_descendant_stmt = this->get_statement_by_id(curr_descendant_id);
            curr_ancestor_stmt.add_descendant(curr_descendant_id);
            curr_descendant_stmt.add_ancestor(curr_ancestor_id);
        }
    }
}
