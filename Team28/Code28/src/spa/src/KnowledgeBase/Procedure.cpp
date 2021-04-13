#include "Procedure.h"

using namespace KnowledgeBase;

Procedure::Procedure(std::string name) : name(name), first_statement(-1) {
    statements = std::make_shared<std::unordered_set<int>>();
    called_by_statements = std::make_shared<std::unordered_set<int>>();
    modifies = std::make_shared<std::unordered_set<std::string>>();
    uses = std::make_shared<std::unordered_set<std::string>>();
    direct_callers = std::make_shared<std::unordered_set<std::string>>();
    callers = std::make_shared<std::unordered_set<std::string>>();
    direct_callees = std::make_shared<std::unordered_set<std::string>>();
    callees = std::make_shared<std::unordered_set<std::string>>();
}

Procedure::Procedure() : name(""), first_statement(-1) {
    statements = std::make_shared<std::unordered_set<int>>();
    called_by_statements = std::make_shared<std::unordered_set<int>>();
    modifies = std::make_shared<std::unordered_set<std::string>>();
    uses = std::make_shared<std::unordered_set<std::string>>();
    direct_callers = std::make_shared<std::unordered_set<std::string>>();
    callers = std::make_shared<std::unordered_set<std::string>>();
    direct_callees = std::make_shared<std::unordered_set<std::string>>();
    callees = std::make_shared<std::unordered_set<std::string>>();
}

Procedure::~Procedure() {}

std::string Procedure::get_name() { return name; }

int Procedure::get_first_statement() { return first_statement; }

std::shared_ptr<std::unordered_set<int>> Procedure::get_statements() { return statements; }

void Procedure::add_statement(int stmt_id) {
    statements->insert(stmt_id);
    if (first_statement == -1 || first_statement > stmt_id) {
        first_statement = stmt_id;
    }
}

std::shared_ptr<std::unordered_set<int>> Procedure::get_called_by_statements() {
    return called_by_statements;
}

void Procedure::add_called_by_statement(int stmt_id) { called_by_statements->insert(stmt_id); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_modifies() { return modifies; }

void Procedure::add_modifies(std::string var_name) { modifies->insert(var_name); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_uses() { return uses; }

void Procedure::add_uses(std::string var_name) { uses->insert(var_name); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_direct_callers() {
    return direct_callers;
}

void Procedure::add_direct_caller(std::string proc_name) { direct_callers->insert(proc_name); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_callers() { return callers; }

void Procedure::add_caller(std::string proc_name) { callers->insert(proc_name); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_direct_callees() {
    return direct_callees;
}

void Procedure::add_direct_callee(std::string proc_name) { direct_callees->insert(proc_name); }

std::shared_ptr<std::unordered_set<std::string>> Procedure::get_callees() { return callees; }

void Procedure::add_callee(std::string proc_name) { callees->insert(proc_name); }
