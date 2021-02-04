#pragma once

#include <string>
#include <vector>

namespace Parser {

// AST node type for the SIMPLE language.
enum SimpleNodeType {
    INVALID_NODE,

    PROGRAM,
    PROCEDURE,
    STMT_LST,

    READ,
    PRINT,
    CALL,
    WHILE,
    IF,
    ASSIGN,

    CONDITIONAL,
    ARITHMETIC,

    VAR_NAME,
    PROC_NAME,
    CONST_VALUE,
};

// AST node for the SIMPLE language.
class SimpleNode {
public:
    explicit SimpleNode(SimpleNodeType type);

    SimpleNode(SimpleNodeType type, std::string value);

    SimpleNode(SimpleNodeType type, int statement_id);

    ~SimpleNode();

    // Returns true when node type is either one of the following:
    // - CONDITIONAL
    // - ARITHMETIC
    // - VAR_NAME
    // - PROC_NAME
    // - CONST_VALUE
    bool has_value();

    // Gets the value contained within the AST node.
    //
    // SimpleNode values only matter for the following node types:
    // - CONDITIONAL, value = “!” | “&&” | “||” | “>” | “>=” | “<” | “<=” | “!=” | “==”,
    // - ARITHMETIC, value = “+” | “-” | “*” | “/” | “%”
    // - VAR_NAME, value = [a-zA-Z][a-zA-Z0-9]*
    // - PROC_NAME, value = [a-zA-Z][a-zA-Z0-9]*
    // - CONST_VALUE, value = [0-9]+
    //
    // Throws unless has_value returns true.
    std::string get_value();

    // Returns true when node type is either one of the following:
    // - READ
    // - PRINT
    // - CALL
    // - WHILE
    // - IF
    // - ASSIGN
    bool has_statement_id();

    // Gets the statement id of the current AST node (only valid for statement nodes).
    // Throws unless has_statement_id returns true.
    int get_statement_id();

    // Gets the current AST node type.
    SimpleNodeType get_type();

    // Gets the children AST nodes of the current AST node.
    std::vector<SimpleNode*> get_children();

    // Gets the child AST node located at a specific index.
    SimpleNode* get_child(int i);

    // Adds a child AST node to the current AST node.
    void add_child(SimpleNode* node);

    // Checks if the supplied AST node is a subtree of the current AST node.
    bool is_subtree(SimpleNode* node);

    // Checks if the supplied AST node is equal to the current AST node (including all their
    // children).
    bool is_equal(SimpleNode* node);

    // Pretty-prints the complete AST starting from the current AST node.
    std::string to_string();

private:
    SimpleNodeType type;
    std::string value;
    int statement_id;
    std::vector<SimpleNode*> children;

    std::string to_string(int padding);
};

} // namespace Parser
