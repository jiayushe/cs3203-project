#include "Statement.h"

using namespace KnowledgeBase;

Statement::Statement(StatementType type, int id, std::string procedure_name,
                     std::shared_ptr<Parser::SimpleNode> pattern)
    : type(type), id(id), procedure_name(procedure_name), pattern(pattern), parent(-1),
      following(-1), follower(-1) {
    children = new std::unordered_set<int>();
    modifies = new std::unordered_set<std::string>();
    uses = new std::unordered_set<std::string>();
}

Statement::Statement()
    : type(StatementType::ASSIGN), id(0), procedure_name("DEFAULT"), pattern(nullptr), parent(-1),
      following(-1), follower(-1) {
    children = new std::unordered_set<int>();
    modifies = new std::unordered_set<std::string>();
    uses = new std::unordered_set<std::string>();
}

Statement::~Statement() {
    delete children;
    delete modifies;
    delete uses;
}

StatementType Statement::get_type() { return type; }

int Statement::get_id() { return id; }

std::string Statement::get_procedure_name() { return procedure_name; }

std::shared_ptr<Parser::SimpleNode> Statement::get_pattern() {
    if (this->type != StatementType::ASSIGN) {
        throw "This statement is not of type ASSIGN";
    }
    return pattern;
}

void Statement::set_pattern(std::shared_ptr<Parser::SimpleNode> ast_pattern) {
    pattern = ast_pattern;
}

int Statement::get_parent() { return parent; }

std::vector<int> Statement::get_children() {
    if (this->type != StatementType::WHILE && this->type != StatementType::IF) {
        throw "This statement is not of type WHILE or IF";
    }
    std::vector<int> res;
    res.insert(res.end(), children->begin(), children->end());
    return res;
}

int Statement::get_following() { return following; }

int Statement::get_follower() { return follower; }

std::string Statement::get_procedure_called() {
    if (this->type != StatementType::CALL) {
        throw "This statement is not of type CALL";
    }
    return procedure_called;
}

std::vector<std::string> Statement::get_modifies() {
    std::vector<std::string> res;
    res.insert(res.end(), modifies->begin(), modifies->end());
    return res;
}

void Statement::add_modifies(std::string var_name) { modifies->insert(var_name); }

std::vector<std::string> Statement::get_uses() {
    std::vector<std::string> res;
    res.insert(res.end(), uses->begin(), uses->end());
    return res;
}

void Statement::add_uses(std::string var_name) { uses->insert(var_name); }
