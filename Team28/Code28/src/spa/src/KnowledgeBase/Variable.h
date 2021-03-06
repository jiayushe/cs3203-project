#pragma once

#include <memory>
#include <string>
#include <unordered_set>

namespace KnowledgeBase {

class Variable {
public:
    Variable(std::string name);

    Variable();

    ~Variable();

    // Gets the name of the variable.
    std::string get_name();

    // Gets the list of ids for statement that directly modifies this variable.
    std::shared_ptr<std::unordered_set<int>> get_direct_modified_by();

    // Adds a statement that directly modifies this variable via id.
    void add_direct_modified_by(int stmt_id);

    // Gets the list of ids for statement that modifies this variable.
    std::shared_ptr<std::unordered_set<int>> get_modified_by();

    // Adds a statement that modifies this variable via id.
    void add_modified_by(int stmt_id);

    // Gets the list of ids for statement that directly uses this variable.
    std::shared_ptr<std::unordered_set<int>> get_direct_used_by();

    // Adds a statement that directly uses this variable via id.
    void add_direct_used_by(int stmt_id);

    // Gets the list of ids for statement that uses this variable.
    std::shared_ptr<std::unordered_set<int>> get_used_by();

    // Adds a statement that uses this variable via id.
    void add_used_by(int stmt_id);

    // Gets the list of procedures that modifies this variable.
    std::shared_ptr<std::unordered_set<std::string>> get_modified_by_procedures();

    // Adds a procedure that modifies this variable via id.
    void add_modified_by_procedure(std::string);

    // Gets the list of procedures that uses this variable.
    std::shared_ptr<std::unordered_set<std::string>> get_used_by_procedures();

    // Adds a procedure that uses this variable via id.
    void add_used_by_procedure(std::string);

private:
    std::string name;
    std::shared_ptr<std::unordered_set<int>> direct_modified_by;
    std::shared_ptr<std::unordered_set<int>> modified_by;
    std::shared_ptr<std::unordered_set<int>> direct_used_by;
    std::shared_ptr<std::unordered_set<int>> used_by;
    std::shared_ptr<std::unordered_set<std::string>> modified_by_procedures;
    std::shared_ptr<std::unordered_set<std::string>> used_by_procedures;
};

} // namespace KnowledgeBase
