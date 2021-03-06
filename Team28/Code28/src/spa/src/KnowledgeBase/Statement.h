#pragma once

#include "Parser/SimpleParser/SimpleNode.h"
#include <memory>
#include <string>
#include <unordered_set>

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

    // Gets the id of the parent statement.
    // Returns -1 if it has no parent statement.
    int get_parent();

    // Sets the id of the parent statement.
    void set_parent(int parent_id);

    // Gets the list of ids of the ancestor statements.
    std::shared_ptr<std::unordered_set<int>> get_ancestors();

    // Adds an ancestor statement via id.
    void add_ancestor(int ancestor_id);

    // Gets the list of ids for the children statements.
    std::shared_ptr<std::unordered_set<int>> get_children();

    // Adds a children statement via id.
    // Throws unless its type is WHILE/IF.
    void add_child(int child_id);

    // Gets the list of ids of the descendant statements.
    std::shared_ptr<std::unordered_set<int>> get_descendants();

    // Adds a descendant statement via id.
    // Throws unless its type is WHILE/IF.
    void add_descendant(int descendant_id);

    // Gets the id of the previous statement.
    // Returns -1 if it has no following statements.
    int get_direct_following();

    // Sets the id of the previous statement.
    void set_direct_following(int following_id);

    // Gets the list of ids of the previous statements.
    std::shared_ptr<std::unordered_set<int>> get_followings();

    // Add a previous statement via id.
    void add_following(int following_id);

    // Gets the id of the next statement.
    // Returns -1 if it has no follower statements.
    int get_direct_follower();

    // Sets the id of the next statement.
    void set_direct_follower(int follower_id);

    // Gets the list of ids of the subsequent statements.
    std::shared_ptr<std::unordered_set<int>> get_followers();

    // Add a subsequent statement via id.
    void add_follower(int follower_id);

    // Gets the list of ids of the direct previous prog lines.
    std::shared_ptr<std::unordered_set<int>> get_direct_previous();

    // Adds a direct previous prog line via id.
    void add_direct_previous(int previous_id);

    // Gets the list of ids of the previous prog lines.
    std::shared_ptr<std::unordered_set<int>> get_previous();

    // Adds a previous prog line via id.
    void add_previous(int previous_id);

    // Gets the list of ids of the direct next prog lines.
    std::shared_ptr<std::unordered_set<int>> get_direct_next();

    // Adds a direct next prog line via id.
    void add_direct_next(int next_id);

    // Gets the list of ids of the next prog lines.
    std::shared_ptr<std::unordered_set<int>> get_next();

    // Adds a next prog line via id.
    void add_next(int next_id);

    // Gets the list of ids of the direct previous prog lines.
    // Follows the definition of NextBip.
    std::shared_ptr<std::unordered_set<int>> get_direct_previous_bip();

    // Adds a direct previous prog line via id.
    // Follows the definition of NextBip.
    void add_direct_previous_bip(int previous_id);

    // Gets the list of ids of the previous prog lines.
    // Follows the definition of NextBip*.
    std::shared_ptr<std::unordered_set<int>> get_previous_bip();

    // Adds a previous prog line via id.
    // Follows the definition of NextBip*.
    void add_previous_bip(int previous_id);

    // Gets the list of ids of the direct next prog lines.
    // Follows the definition of NextBip.
    std::shared_ptr<std::unordered_set<int>> get_direct_next_bip();

    // Adds a direct next prog line via id.
    // Follows the definition of NextBip.
    void add_direct_next_bip(int next_id);

    // Gets the list of ids of the next prog lines.
    // Follows the definition of NextBip*.
    std::shared_ptr<std::unordered_set<int>> get_next_bip();

    // Adds a next prog line via id.
    // Follows the definition of NextBip*.
    void add_next_bip(int next_id);

    // Gets the list of names of variables that are directly modified.
    std::shared_ptr<std::unordered_set<std::string>> get_direct_modifies();

    // Adds a variable that is directly modified.
    void add_direct_modifies(std::string var_name);

    // Gets the list of names of variables that are modified.
    std::shared_ptr<std::unordered_set<std::string>> get_modifies();

    // Adds a variable that is modified.
    void add_modifies(std::string var_name);

    // Gets the list of names of variables that are directly used.
    std::shared_ptr<std::unordered_set<std::string>> get_direct_uses();

    // Adds a variable that is directly used.
    void add_direct_uses(std::string var_name);

    // Gets the list of names of variables that are used.
    std::shared_ptr<std::unordered_set<std::string>> get_uses();

    // Adds a variable that is used.
    void add_uses(std::string var_name);

    // Gets the list of ids of statements that are directly affected by this statement.
    std::shared_ptr<std::unordered_set<int>> get_direct_affects();

    // Adds a statement that is directly affected by this statement.
    void add_direct_affects(int stmt_id);

    // Gets the list of ids of statements that are affected by this statement.
    std::shared_ptr<std::unordered_set<int>> get_affects();

    // Adds a statement that is affected by this statement.
    void add_affects(int stmt_id);

    // Gets the list of ids of statements that directly affect this statement.
    std::shared_ptr<std::unordered_set<int>> get_direct_affected_by();

    // Adds a statement that directly affects this statement.
    void add_direct_affected_by(int stmt_id);

    // Gets the list of ids of statements that affect this statement.
    std::shared_ptr<std::unordered_set<int>> get_affected_by();

    // Adds a statement that affects this statement.
    void add_affected_by(int stmt_id);

    // Gets the list of ids of statements that are directly affected by this statement.
    // Follows the definition of AffectsBip.
    std::shared_ptr<std::unordered_set<int>> get_direct_affects_bip();

    // Adds a statement that is directly affected by this statement.
    // Follows the definition of AffectsBip.
    void add_direct_affects_bip(int stmt_id);

    // Gets the list of ids of statements that are affected by this statement.
    // Follows the definition of AffectsBip*.
    std::shared_ptr<std::unordered_set<int>> get_affects_bip();

    // Adds a statement that is affected by this statement.
    // Follows the definition of AffectsBip*.
    void add_affects_bip(int stmt_id);

    // Gets the list of ids of statements that directly affect this statement.
    // Follows the definition of AffectsBip.
    std::shared_ptr<std::unordered_set<int>> get_direct_affected_bip_by();

    // Adds a statement that directly affects this statement.
    // Follows the definition of AffectsBip.
    void add_direct_affected_bip_by(int stmt_id);

    // Gets the list of ids of statements that affect this statement.
    // Follows the definition of AffectsBip*.
    std::shared_ptr<std::unordered_set<int>> get_affected_bip_by();

    // Adds a statement that affects this statement.
    // Follows the definition of AffectsBip*.
    void add_affected_bip_by(int stmt_id);

    // Gets the name of the procedure that is called.
    // Throws unless its type is CALL.
    std::string get_procedure_called();

    // Sets the name of the procedure that is called.
    // Throws unless its type is CALL.
    void set_procedure_called(std::string proc_name);

    // Gets the AST node of this statement.
    // Throws unless its type is ASSIGN.
    std::shared_ptr<Parser::SimpleNode> get_pattern();

    // Sets the AST pattern of this statement
    // Throws unless its type is ASSIGN.
    void set_pattern(std::shared_ptr<Parser::SimpleNode> ast_pattern);

private:
    StatementType type;
    int id;
    std::string procedure_name;
    int parent;
    std::shared_ptr<std::unordered_set<int>> ancestors;
    std::shared_ptr<std::unordered_set<int>> children;
    std::shared_ptr<std::unordered_set<int>> descendants;
    int direct_following;
    std::shared_ptr<std::unordered_set<int>> followings;
    int direct_follower;
    std::shared_ptr<std::unordered_set<int>> followers;
    std::shared_ptr<std::unordered_set<int>> direct_previous;
    std::shared_ptr<std::unordered_set<int>> previous;
    std::shared_ptr<std::unordered_set<int>> direct_next;
    std::shared_ptr<std::unordered_set<int>> next;
    std::shared_ptr<std::unordered_set<int>> direct_previous_bip;
    std::shared_ptr<std::unordered_set<int>> previous_bip;
    std::shared_ptr<std::unordered_set<int>> direct_next_bip;
    std::shared_ptr<std::unordered_set<int>> next_bip;
    std::shared_ptr<std::unordered_set<std::string>> direct_modifies;
    std::shared_ptr<std::unordered_set<std::string>> modifies;
    std::shared_ptr<std::unordered_set<std::string>> direct_uses;
    std::shared_ptr<std::unordered_set<std::string>> uses;
    std::shared_ptr<std::unordered_set<int>> direct_affects;
    std::shared_ptr<std::unordered_set<int>> affects;
    std::shared_ptr<std::unordered_set<int>> direct_affected_by;
    std::shared_ptr<std::unordered_set<int>> affected_by;
    std::shared_ptr<std::unordered_set<int>> direct_affects_bip;
    std::shared_ptr<std::unordered_set<int>> affects_bip;
    std::shared_ptr<std::unordered_set<int>> direct_affected_bip_by;
    std::shared_ptr<std::unordered_set<int>> affected_bip_by;
    // Only for CALL.
    std::string procedure_called;
    // Only for ASSIGN.
    std::shared_ptr<Parser::SimpleNode> pattern;
};

} // namespace KnowledgeBase
