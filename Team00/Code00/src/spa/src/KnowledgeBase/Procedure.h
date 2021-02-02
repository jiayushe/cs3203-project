#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace KnowledgeBase {

    class Procedure {
    public:
        Procedure(std::string name);

        Procedure();

        ~Procedure();

        // Gets the name of the procedure.
        std::string get_name();

        // Gets the list of ids for statements in the procedure.
        std::vector<int> get_statements();

        // Adds a statement in the procedure via id.
        void add_statement(int stmt_id);

        // Gets the list of ids for statements that call this procedure.
        std::vector<int> get_called_by();

        // Adds a statement that calls this procedure via id.
        void add_called_by(int stmt_id);

    private:
        std::string name;
        std::unordered_set<int> statements;
        std::unordered_set<int> called_by;
    };

}
