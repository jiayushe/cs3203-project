#pragma once

#include <memory>
#include <string>
#include <unordered_set>

namespace KnowledgeBase {

class Procedure {
public:
    Procedure(std::string name);

    Procedure();

    ~Procedure();

    // Gets the name of the procedure.
    std::string get_name();

    // Gets the first statement of the procedure.
    int get_first_statement();

    // Gets the list of ids for statements in the procedure.
    std::shared_ptr<std::unordered_set<int>> get_statements();

    // Adds a statement in the procedure via id.
    void add_statement(int stmt_id);

    // Gets the list of ids for statements that call this procedure.
    std::shared_ptr<std::unordered_set<int>> get_called_by_statements();

    // Adds a statement that calls this procedure via id.
    void add_called_by_statement(int stmt_id);

    // Gets the list of names of variables that are modified.
    std::shared_ptr<std::unordered_set<std::string>> get_modifies();

    // Adds a variable that is modified.
    void add_modifies(std::string var_name);

    // Gets the list of names of variables that are used.
    std::shared_ptr<std::unordered_set<std::string>> get_uses();

    // Adds a variable that is used.
    void add_uses(std::string var_name);

    // Gets the list of names of procedures that directly call this procedure.
    std::shared_ptr<std::unordered_set<std::string>> get_direct_callers();

    // Adds a procedure that directly calls this procedure.
    void add_direct_caller(std::string proc_name);

    // Gets the list of names of procedures that call this procedure.
    std::shared_ptr<std::unordered_set<std::string>> get_callers();

    // Adds a procedure that calls this procedure.
    void add_caller(std::string proc_name);

    // Gets the list of names of procedures that are directly called by this procedure.
    std::shared_ptr<std::unordered_set<std::string>> get_direct_callees();

    // Adds a procedure that is directly called by this procedure.
    void add_direct_callee(std::string proc_name);

    // Gets the list of names of procedures that are called by this procedure.
    std::shared_ptr<std::unordered_set<std::string>> get_callees();

    // Adds a procedure that is called by this procedure.
    void add_callee(std::string proc_name);

private:
    std::string name;
    int first_statement;
    std::shared_ptr<std::unordered_set<int>> statements;
    std::shared_ptr<std::unordered_set<int>> called_by_statements;
    std::shared_ptr<std::unordered_set<std::string>> modifies;
    std::shared_ptr<std::unordered_set<std::string>> uses;
    std::shared_ptr<std::unordered_set<std::string>> direct_callers;
    std::shared_ptr<std::unordered_set<std::string>> callers;
    std::shared_ptr<std::unordered_set<std::string>> direct_callees;
    std::shared_ptr<std::unordered_set<std::string>> callees;
};

} // namespace KnowledgeBase
