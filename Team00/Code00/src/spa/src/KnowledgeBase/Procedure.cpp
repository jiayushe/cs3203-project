#include "Procedure.h"

using namespace KnowledgeBase;

Procedure::Procedure(std::string name) : name(name) {
    statements = new std::unordered_set<int>();
    called_by = new std::unordered_set<int>();
}

Procedure::Procedure() : name("") {
    statements = new std::unordered_set<int>();
    called_by = new std::unordered_set<int>();
}

Procedure::~Procedure() {
    delete statements;
    delete called_by;
}

std::string Procedure::get_name() { return name; }

std::unordered_set<int> Procedure::get_statements() { return *statements; }

void Procedure::add_statement(int stmt_id) { statements->insert(stmt_id); }

std::unordered_set<int> Procedure::get_called_by() { return *called_by; }

void Procedure::add_called_by(int stmt_id) { called_by->insert(stmt_id); }
