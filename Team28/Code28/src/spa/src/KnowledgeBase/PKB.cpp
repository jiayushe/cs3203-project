#include "PKB.h"
#include <stdexcept>

using namespace KnowledgeBase;

PKB::PKB(std::shared_ptr<Parser::SimpleNode> ast) : ast(ast) {}

PKB::~PKB() {}

std::shared_ptr<Parser::SimpleNode> PKB::get_ast() { return ast; }

std::unordered_map<int, std::unordered_set<int>> PKB::get_cfg() { return cfg; }

std::vector<std::shared_ptr<Procedure>> PKB::get_procedures() {
    std::vector<std::shared_ptr<Procedure>> res;
    for (auto kv : procedures) {
        res.push_back(kv.second);
    }
    return res;
}

std::shared_ptr<Procedure> PKB::get_procedure_by_name(std::string name) {
    if (procedures.find(name) == procedures.end()) {
        throw std::runtime_error("Procedure " + name + " does not exist");
    }
    return procedures.at(name);
}

std::shared_ptr<Procedure> PKB::add_procedure(std::string name) {
    auto it = procedures.find(name);
    if (it != procedures.end()) {
        return it->second;
    }
    auto proc = std::make_shared<Procedure>(name);
    procedures[name] = proc;
    return proc;
}

std::vector<std::shared_ptr<Statement>> PKB::get_statements() {
    std::vector<std::shared_ptr<Statement>> res;
    for (auto kv : statements) {
        res.push_back(kv.second);
    }
    return res;
}

std::shared_ptr<Statement> PKB::get_statement_by_id(int id) {
    if (statements.find(id) == statements.end()) {
        throw std::runtime_error("Statement " + std::to_string(id) + " does not exist");
    }
    return statements.at(id);
}

std::shared_ptr<Statement> PKB::add_statement(StatementType type, int id, std::string proc_name,
                                              std::shared_ptr<Parser::SimpleNode> stmt_node) {
    auto it = statements.find(id);
    if (it != statements.end()) {
        return it->second;
    }
    auto stmt = std::make_shared<Statement>(type, id, proc_name);
    statements[id] = stmt;
    auto proc = this->get_procedure_by_name(proc_name);
    proc->add_statement(id);
    switch (type) {
    case KnowledgeBase::StatementType::ASSIGN:
        stmt->set_pattern(stmt_node->get_child(1));
        break;
    case KnowledgeBase::StatementType::CALL: {
        auto proc_called_name = stmt_node->get_child(0)->get_value();
        stmt->set_procedure_called(proc_called_name);
        auto proc_called = this->get_procedure_by_name(proc_called_name);
        proc_called->add_called_by(id);
        break;
    }
    }
    return stmt;
}

std::vector<std::shared_ptr<Variable>> PKB::get_variables() {
    std::vector<std::shared_ptr<Variable>> res;
    for (auto kv : variables) {
        res.push_back(kv.second);
    }
    return res;
}

std::shared_ptr<Variable> PKB::get_variable_by_name(std::string name) {
    if (variables.find(name) == variables.end()) {
        throw std::runtime_error("Variable " + name + " does not exist");
    }
    return variables.at(name);
}

std::shared_ptr<Variable> PKB::add_variable(std::string name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    auto var = std::make_shared<Variable>(name);
    variables[name] = var;
    return var;
}

std::vector<std::shared_ptr<Constant>> PKB::get_constants() {
    std::vector<std::shared_ptr<Constant>> res;
    for (auto kv : constants) {
        res.push_back(kv.second);
    }
    return res;
}

std::shared_ptr<Constant> PKB::get_constant_by_value(int value) {
    if (constants.find(value) == constants.end()) {
        throw std::runtime_error("Constant " + std::to_string(value) + " does not exist");
    }
    return constants.at(value);
}

void PKB::set_cfg(std::unordered_map<int, std::unordered_set<int>> input_cfg) { cfg = input_cfg; }

std::shared_ptr<Constant> PKB::add_constant(int value) {
    auto it = constants.find(value);
    if (it != constants.end()) {
        return it->second;
    }
    auto cst = std::make_shared<Constant>(value);
    constants[value] = cst;
    return cst;
}

void PKB::add_modify_relationship(int stmt_id, std::string var_name) {
    auto stmt = this->get_statement_by_id(stmt_id);
    auto var = this->get_variable_by_name(var_name);
    stmt->add_direct_modifies(var_name);
    stmt->add_modifies(var_name);
    var->add_direct_modified_by(stmt_id);
    var->add_modified_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt->get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        auto curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt->add_modifies(var_name);
        var->add_modified_by(curr_ancestor_id);
    }
}

void PKB::add_use_relationship(int stmt_id, std::string var_name) {
    auto stmt = this->get_statement_by_id(stmt_id);
    auto var = this->get_variable_by_name(var_name);
    stmt->add_direct_uses(var_name);
    stmt->add_uses(var_name);
    var->add_direct_used_by(stmt_id);
    var->add_used_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt->get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        auto curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt->add_uses(var_name);
        var->add_used_by(curr_ancestor_id);
    }
}

void PKB::add_follow_relationship(int following_id, int follower_id) {
    if (following_id == -1) {
        return;
    }
    auto following_stmt = this->get_statement_by_id(following_id);
    auto follower_stmt = this->get_statement_by_id(follower_id);
    following_stmt->set_direct_follower(follower_id);
    follower_stmt->set_direct_following(following_id);
    // Extract all followings and followers.
    std::unordered_set<int> following_ids = following_stmt->get_followings();
    following_ids.insert(following_id);
    std::unordered_set<int> follower_ids = follower_stmt->get_followers();
    follower_ids.insert(follower_id);
    for (const auto& curr_following_id : following_ids) {
        auto curr_following_stmt = this->get_statement_by_id(curr_following_id);
        for (const auto& curr_follower_id : follower_ids) {
            auto curr_follower_stmt = this->get_statement_by_id(curr_follower_id);
            curr_following_stmt->add_follower(curr_follower_id);
            curr_follower_stmt->add_following(curr_following_id);
        }
    }
}

void PKB::add_parent_relationship(int parent_id, int child_id) {
    if (parent_id == -1) {
        return;
    }
    auto parent_stmt = this->get_statement_by_id(parent_id);
    auto child_stmt = this->get_statement_by_id(child_id);
    parent_stmt->add_child(child_id);
    child_stmt->set_parent(parent_id);
    // Extract all ancestors and descendants.
    std::unordered_set<int> ancestor_ids = parent_stmt->get_ancestors();
    ancestor_ids.insert(parent_id);
    std::unordered_set<int> descendant_ids = child_stmt->get_descendants();
    descendant_ids.insert(child_id);
    for (const auto& curr_ancestor_id : ancestor_ids) {
        auto curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        for (const auto& curr_descendant_id : descendant_ids) {
            auto curr_descendant_stmt = this->get_statement_by_id(curr_descendant_id);
            curr_ancestor_stmt->add_descendant(curr_descendant_id);
            curr_descendant_stmt->add_ancestor(curr_ancestor_id);
        }
    }
}
