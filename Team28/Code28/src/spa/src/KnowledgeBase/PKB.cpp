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
        std::shared_ptr<Procedure> proc_called;
        try {
            proc_called = this->get_procedure_by_name(proc_called_name);
        } catch (std::runtime_error&) {
            // Assume validation has been checked in simple parser.
            proc_called = this->add_procedure(proc_called_name);
        }
        proc_called->add_called_by_statement(id);
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

void PKB::add_modify_relationship(int stmt_id, std::string var_name, bool direct) {
    auto stmt = this->get_statement_by_id(stmt_id);
    auto var = this->get_variable_by_name(var_name);
    if (direct) {
        stmt->add_direct_modifies(var_name);
        var->add_direct_modified_by(stmt_id);
    }
    stmt->add_modifies(var_name);
    var->add_modified_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt->get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        auto curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt->add_modifies(var_name);
        var->add_modified_by(curr_ancestor_id);
    }
    if (stmt->get_type() == KnowledgeBase::StatementType::CALL) {
        // No need to backtrack procedures for CALL statements
        return;
    }
    // Extract callers of the current procedure.
    // Assume all call relationships have been extracted and stored.
    std::string proc_name = stmt->get_procedure_name();
    auto proc = this->get_procedure_by_name(proc_name);
    std::unordered_set<std::string> caller_names = proc->get_callers();
    caller_names.insert(proc_name);
    for (const auto& curr_caller_name : caller_names) {
        auto curr_caller_proc = this->get_procedure_by_name(curr_caller_name);
        curr_caller_proc->add_modifies(var_name);
        var->add_modified_by_procedure(curr_caller_name);
        auto curr_called_by_ids = curr_caller_proc->get_called_by_statements();
        for (const auto& curr_called_by_id : curr_called_by_ids) {
            this->add_modify_relationship(curr_called_by_id, var_name, false);
        }
    }
}

void PKB::add_use_relationship(int stmt_id, std::string var_name, bool direct) {
    auto stmt = this->get_statement_by_id(stmt_id);
    auto var = this->get_variable_by_name(var_name);
    if (direct) {
        stmt->add_direct_uses(var_name);
        var->add_direct_used_by(stmt_id);
    }
    stmt->add_uses(var_name);
    var->add_used_by(stmt_id);
    // Extract ancestors of the current stmt.
    // Assume all parent-child relationships have been extracted and stored.
    std::unordered_set<int> ancestor_ids = stmt->get_ancestors();
    for (const auto& curr_ancestor_id : ancestor_ids) {
        auto curr_ancestor_stmt = this->get_statement_by_id(curr_ancestor_id);
        curr_ancestor_stmt->add_uses(var_name);
        var->add_used_by(curr_ancestor_id);
    }
    if (stmt->get_type() == KnowledgeBase::StatementType::CALL) {
        // No need to backtrack procedures for CALL statements
        return;
    }
    // Extract callers of the current procedure.
    // Assume all call relationships have been extracted and stored.
    std::string proc_name = stmt->get_procedure_name();
    auto proc = this->get_procedure_by_name(proc_name);
    std::unordered_set<std::string> caller_names = proc->get_callers();
    caller_names.insert(proc_name);
    for (const auto& curr_caller_name : caller_names) {
        auto curr_caller_proc = this->get_procedure_by_name(curr_caller_name);
        curr_caller_proc->add_uses(var_name);
        var->add_used_by_procedure(curr_caller_name);
        auto curr_called_by_ids = curr_caller_proc->get_called_by_statements();
        for (const auto& curr_called_by_id : curr_called_by_ids) {
            this->add_use_relationship(curr_called_by_id, var_name, false);
        }
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

void PKB::add_next_relationship(int prev_id, int next_id) {
    auto prev_stmt = this->get_statement_by_id(prev_id);
    auto next_stmt = this->get_statement_by_id(next_id);
    prev_stmt->add_direct_next(next_id);
    next_stmt->add_direct_previous(prev_id);
    // Extract all previous and next.
    std::unordered_set<int> prev_ids = prev_stmt->get_previous();
    prev_ids.insert(prev_id);
    std::unordered_set<int> next_ids = next_stmt->get_next();
    next_ids.insert(next_id);
    for (const auto& curr_prev_id : prev_ids) {
        auto curr_prev_stmt = this->get_statement_by_id(curr_prev_id);
        for (const auto& curr_next_id : next_ids) {
            auto curr_next_stmt = this->get_statement_by_id(curr_next_id);
            curr_prev_stmt->add_next(curr_next_id);
            curr_next_stmt->add_previous(curr_prev_id);
        }
    }
}

void PKB::add_call_relationship(std::string caller_name, std::string callee_name) {
    auto caller_proc = this->get_procedure_by_name(caller_name);
    auto callee_proc = this->get_procedure_by_name(callee_name);
    caller_proc->add_direct_callee(callee_name);
    callee_proc->add_direct_caller(caller_name);
    // Extract all callers and callees.
    std::unordered_set<std::string> caller_names = caller_proc->get_callers();
    caller_names.insert(caller_name);
    std::unordered_set<std::string> callee_names = callee_proc->get_callees();
    callee_names.insert(callee_name);
    for (const auto& curr_caller_name : caller_names) {
        auto curr_caller_proc = this->get_procedure_by_name(curr_caller_name);
        for (const auto& curr_callee_name : callee_names) {
            auto curr_callee_proc = this->get_procedure_by_name(curr_callee_name);
            curr_caller_proc->add_callee(curr_callee_name);
            curr_callee_proc->add_caller(curr_caller_name);
        }
    }
}

void PKB::add_affect_relationship(int affects_id, int affected_id) {
    auto affects_stmt = this->get_statement_by_id(affects_id);
    auto affected_stmt = this->get_statement_by_id(affected_id);
    affects_stmt->add_direct_affects(affected_id);
    affected_stmt->add_direct_affected_by(affects_id);
    // Extract all nested affect relationship.
    std::unordered_set<int> affects_ids = affects_stmt->get_affected_by();
    affects_ids.insert(affects_id);
    std::unordered_set<int> affected_ids = affected_stmt->get_affects();
    affected_ids.insert(affected_id);
    for (const auto& curr_affects_id : affects_ids) {
        auto curr_affects_stmt = this->get_statement_by_id(curr_affects_id);
        for (const auto& curr_affected_id : affected_ids) {
            auto curr_affected_stmt = this->get_statement_by_id(curr_affected_id);
            curr_affects_stmt->add_affects(curr_affected_id);
            curr_affected_stmt->add_affected_by(curr_affects_id);
        }
    }
}
