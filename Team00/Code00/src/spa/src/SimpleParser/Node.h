#ifndef SIMPLEPARSER_NODE_H
#define SIMPLEPARSER_NODE_H

#include <vector>
#include <string>

namespace SimpleParser {

    // AST node type for the SIMPLE language.
    enum NodeType {
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
    class Node {
    public:
        Node(NodeType type);

        Node(NodeType type, std::string value);

        Node(NodeType type, int statement_id);

        ~Node();

        // Gets the value contained within the AST node.
        //
        // Node values only matter for the following node types:
        // - CONDITIONAL, value = “!” | “&&” | “||” | “>” | “>=” | “<” | “<=” | “!=” | “==”,
        // - ARITHMETIC, value = “+” | “-” | “*” | “/” | “%”
        // - VAR_NAME, value = [a-zA-Z][a-zA-Z0-9]*
        // - PROC_NAME, value = [a-zA-Z][a-zA-Z0-9]*
        // - CONST_VALUE, value = [0-9]+
        //
        // Throws unless node type is either one of the following:
        // - CONDITIONAL
        // - ARITHMETIC
        // - VAR_NAME
        // - PROC_NAME
        // - CONST_VALUE
        std::string get_value();

        // Gets the statement id of the current AST node (only valid for statement nodes).
        //
        // Throws unless node type is either one of the following:
        // - READ
        // - PRINT
        // - CALL
        // - WHILE
        // - IF
        // - ASSIGN
        int get_statement_id();

        // Gets the current AST node type.
        NodeType get_type();

        // Gets the children AST nodes of the current AST node.
        std::vector<Node *> get_children();

        // Gets the child AST node located at a specific index.
        Node *get_child(int i);

        // Adds a child AST node to the current AST node.
        void add_child(Node *node);

        // Checks if the supplied AST node is a subtree of the current AST node.
        bool is_subtree(Node *node);

        // Checks if the supplied AST node is equal to the current AST node (including all their children).
        bool is_equal(Node *node);

        // Pretty-prints the complete AST starting from the current AST node.
        std::string to_string();

    private:
        NodeType type;
        std::string value;
        int statement_id;
        std::vector<Node *> children;

        std::string to_string(int padding);
    };

}

#endif //SIMPLEPARSER_NODE_H
