#pragma once

#include "Parser/SimpleParser/SimpleNode.h"
#include <string>
#include <unordered_set>
#include <vector>

namespace KnowledgeBase {

enum class StatementType {
    READ,
    PRINT,
    CALL,
    WHILE,
    IF,
    ASSIGN,
};

class Statement {
public:
    Statement(StatementType type, int id, std::string procedure_name);

    Statement();

    ~Statement();

    // Gets the type of the statement.
    StatementType get_type();

    // Gets the id of the statement.
    int get_id();

    // Gets the name of the procedure containing this statement.
    std::string get_procedure_name();

    // Gets the AST node of this statement.
    // Throws unless its type is ASSIGN.
    std::shared_ptr<Parser::SimpleNode> get_pattern();

    // Sets the AST pattern of this statement
    void set_pattern(std::shared_ptr<Parser::SimpleNode> ast_pattern);

    // Gets the id of the parent statement.
    // Returns -1 if it has no parent statement.
    int get_parent();

    // Sets the id of the parent statement.
    void set_parent(int parent_id);

    // Gets the list of ids for the children statements.
    // Throws unless its type is WHILE/IF.
    std::vector<int> get_children();

    // Adds a children statement via id.
    void add_child(int child_id);

    // Gets the id of the previous statement.
    // Returns -1 if it has no following statements.
    int get_following();

    // Sets the id of the previous statement.
    void set_following(int following_id);

    // Gets the id of the next statement.
    // Returns -1 if it has no follower statements.
    int get_follower();

    // Sets the id of the next statement.
    void set_follower(int follower_id);

    // Gets the name of the procedure that is called.
    // Throws unless its type is CALL.
    std::string get_procedure_called();

    // Sets the name of the procedure that is called.
    // Throws unless its type is CALL.
    void set_procedure_called(std::string proc_name);

    // Gets the list of names of variables that are modified.
    std::vector<std::string> get_modifies();

    // Adds a variable that is modified.
    void add_modifies(std::string var_name);

    // Gets the list of names of variables that are used.
    std::vector<std::string> get_uses();

    // Adds a variable that is used.
    void add_uses(std::string var_name);

private:
    StatementType type;
    int id;
    std::string procedure_name;
    std::shared_ptr<Parser::SimpleNode> pattern;
    int parent;
    std::unordered_set<int>* children;
    int following;
    int follower;
    std::string procedure_called;
    std::unordered_set<std::string>* modifies;
    std::unordered_set<std::string>* uses;
};

} // namespace KnowledgeBase
