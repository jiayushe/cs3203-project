#include "Variable.h"

using namespace KnowledgeBase;

Variable::Variable(std::string name) : name(name) {}

Variable::Variable() : name("") {}

Variable::~Variable() {}

std::string Variable::get_name() { return name; }

std::unordered_set<int> Variable::get_direct_modified_by() { return direct_modified_by; }

void Variable::add_direct_modified_by(int stmt_id) { direct_modified_by.insert(stmt_id); }

std::unordered_set<int> Variable::get_modified_by() { return modified_by; }

void Variable::add_modified_by(int stmt_id) { modified_by.insert(stmt_id); }

std::unordered_set<int> Variable::get_direct_used_by() { return direct_used_by; }

void Variable::add_direct_used_by(int stmt_id) { direct_used_by.insert(stmt_id); }

std::unordered_set<int> Variable::get_used_by() { return used_by; }

void Variable::add_used_by(int stmt_id) { used_by.insert(stmt_id); }

std::unordered_set<std::string> Variable::get_modified_by_procedures() {
    return modified_by_procedures;
}

void Variable::add_modified_by_procedure(std::string proc_name) {
    modified_by_procedures.insert(proc_name);
}

std::unordered_set<std::string> Variable::get_used_by_procedures() { return used_by_procedures; }

void Variable::add_used_by_procedure(std::string proc_name) {
    used_by_procedures.insert(proc_name);
}
