#include "Statement.h"
#include <stdexcept>

using namespace KnowledgeBase;

Statement::Statement(StatementType type, int id, std::string procedure_name)
    : type(type), id(id), procedure_name(procedure_name), parent(-1), direct_following(-1),
      direct_follower(-1), pattern(nullptr) {}

Statement::Statement()
    : type(StatementType::ASSIGN), id(0), procedure_name(""), parent(-1), direct_following(-1),
      direct_follower(-1), pattern(nullptr) {}

Statement::~Statement() {}

StatementType Statement::get_type() { return type; }

int Statement::get_id() { return id; }

std::string Statement::get_procedure_name() { return procedure_name; }

int Statement::get_parent() { return parent; }

void Statement::set_parent(int parent_id) { parent = parent_id; }

std::unordered_set<int> Statement::get_ancestors() { return ancestors; }

void Statement::add_ancestor(int ancestor_id) { ancestors.insert(ancestor_id); }

std::unordered_set<int> Statement::get_children() { return children; }

void Statement::add_child(int child_id) {
    if (this->type != StatementType::WHILE && this->type != StatementType::IF) {
        throw std::runtime_error("This statement is not of type WHILE or IF");
    }
    children.insert(child_id);
}

std::unordered_set<int> Statement::get_descendants() { return descendants; }

void Statement::add_descendant(int descendant_id) {
    if (this->type != StatementType::WHILE && this->type != StatementType::IF) {
        throw std::runtime_error("This statement is not of type WHILE or IF");
    }
    descendants.insert(descendant_id);
}

int Statement::get_direct_following() { return direct_following; }

void Statement::set_direct_following(int following_id) { direct_following = following_id; }

std::unordered_set<int> Statement::get_followings() { return followings; }

void Statement::add_following(int following_id) { followings.insert(following_id); }

int Statement::get_direct_follower() { return direct_follower; }

void Statement::set_direct_follower(int follower_id) { direct_follower = follower_id; }

std::unordered_set<int> Statement::get_followers() { return followers; }

void Statement::add_follower(int follower_id) { followers.insert(follower_id); }

std::unordered_set<int> Statement::get_direct_previous() { return direct_previous; }

void Statement::add_direct_previous(int previous_id) { direct_previous.insert(previous_id); }

std::unordered_set<int> Statement::get_previous() { return previous; }

void Statement::add_previous(int previous_id) { previous.insert(previous_id); }

std::unordered_set<int> Statement::get_direct_next() { return direct_next; }

void Statement::add_direct_next(int next_id) { direct_next.insert(next_id); }

std::unordered_set<int> Statement::get_next() { return next; }

void Statement::add_next(int next_id) { next.insert(next_id); }

std::unordered_set<std::string> Statement::get_direct_modifies() { return direct_modifies; }

void Statement::add_direct_modifies(std::string var_name) { direct_modifies.insert(var_name); }

std::unordered_set<std::string> Statement::get_modifies() { return modifies; }

void Statement::add_modifies(std::string var_name) { modifies.insert(var_name); }

std::unordered_set<std::string> Statement::get_direct_uses() { return direct_uses; }

void Statement::add_direct_uses(std::string var_name) { direct_uses.insert(var_name); }

std::unordered_set<std::string> Statement::get_uses() { return uses; }

void Statement::add_uses(std::string var_name) { uses.insert(var_name); }

std::unordered_set<int> Statement::get_direct_affects() { return direct_affects; }

void Statement::add_direct_affects(int stmt_id) { direct_affects.insert(stmt_id); }

std::unordered_set<int> Statement::get_affects() { return affects; }

void Statement::add_affects(int stmt_id) { affects.insert(stmt_id); }

std::unordered_set<int> Statement::get_direct_affected_by() { return direct_affected_by; }

void Statement::add_direct_affected_by(int stmt_id) { direct_affected_by.insert(stmt_id); }

std::unordered_set<int> Statement::get_affected_by() { return affected_by; }

void Statement::add_affected_by(int stmt_id) { affected_by.insert(stmt_id); }

std::string Statement::get_procedure_called() {
    if (this->type != StatementType::CALL) {
        throw std::runtime_error("This statement is not of type CALL");
    }
    return procedure_called;
}

void Statement::set_procedure_called(std::string proc_name) {
    if (this->type != StatementType::CALL) {
        throw std::runtime_error("This statement is not of type CALL");
    }
    procedure_called = proc_name;
}

std::shared_ptr<Parser::SimpleNode> Statement::get_pattern() {
    if (this->type != StatementType::ASSIGN) {
        throw std::runtime_error("This statement is not of type ASSIGN");
    }
    return pattern;
}

void Statement::set_pattern(std::shared_ptr<Parser::SimpleNode> ast_pattern) {
    if (this->type != StatementType::ASSIGN) {
        throw std::runtime_error("This statement is not of type ASSIGN");
    }
    pattern = ast_pattern;
}
