#include "Procedure.h"

using namespace KnowledgeBase;

Procedure::Procedure(std::string name) : name(name) {
    statements = new std::unordered_set<int>();
    called_by = new std::unordered_set<int>();
}

Procedure::Procedure() : name("DEFAULT") {
    statements = new std::unordered_set<int>();
    called_by = new std::unordered_set<int>();
}

Procedure::~Procedure() {
    delete statements;
    delete called_by;
}

std::string Procedure::get_name() { return name; }

std::vector<int> Procedure::get_statements() {
    std::vector<int> res;
    res.insert(res.end(), statements->begin(), statements->end());
    return res;
}

std::vector<int> Procedure::get_called_by() {
    std::vector<int> res;
    res.insert(res.end(), called_by->begin(), called_by->end());
    return res;
}
