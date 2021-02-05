#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace KnowledgeBase {

class Variable {
public:
    Variable(std::string name);

    Variable();

    ~Variable();

    // Gets the name of the variable.
    std::string get_name();

    // Gets the list of ids for statement that modifies this variable.
    std::vector<int> get_modified_by();

    // Adds a statement that modifies this variable via id.
    void add_modified_by(int stmt_id);

    // Gets the list of ids for statement that uses this variable.
    std::vector<int> get_used_by();

    // Adds a statement that uses this variable via id.
    void add_used_by(int stmt_id);

private:
    std::string name;
    std::unordered_set<int> modified_by;
    std::unordered_set<int> used_by;
};

} // namespace KnowledgeBase
