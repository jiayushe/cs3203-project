#include "Variable.h"

using namespace KnowledgeBase;

Variable::Variable(std::string name) : name(name) {
    modified_by = new std::unordered_set<int>();
    used_by = new std::unordered_set<int>();
}

Variable::Variable() : name("DEFAULT") {
    modified_by = new std::unordered_set<int>();
    used_by = new std::unordered_set<int>();
}

Variable::~Variable() {
    delete modified_by;
    delete used_by;
}

std::string Variable::get_name() { return name; }

std::vector<int> Variable::get_modified_by() {
    std::vector<int> res;
    res.insert(res.end(), modified_by->begin(), modified_by->end());
    return res;
}

void Variable::add_modified_by(int stmt_id) { modified_by->insert(stmt_id); }

std::vector<int> Variable::get_used_by() {
    std::vector<int> res;
    res.insert(res.end(), used_by->begin(), used_by->end());
    return res;
}

void Variable::add_used_by(int stmt_id) { used_by->insert(stmt_id); }
