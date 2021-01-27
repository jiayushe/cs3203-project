#include "Node.h"

using namespace SimpleParser;

Node::Node(NodeType type) :
        type(type),
        value(""),
        statement_id(-1),
        children(std::vector<Node *>()) {}

Node::Node(NodeType type, std::string value) :
        type(type),
        value(value),
        statement_id(-1),
        children(std::vector<Node *>()) {}

Node::Node(NodeType type, int statement_id) :
        type(type),
        value(""),
        statement_id(statement_id),
        children(std::vector<Node *>()) {}

Node::~Node() {
    for (auto const child : children) {
        delete child;
    }
}

NodeType Node::get_type() {
    return type;
}

std::string Node::get_value() {
    if (type != NodeType::CONDITIONAL
        && type != NodeType::ARITHMETIC
        && type != NodeType::VAR_NAME
        && type != NodeType::PROC_NAME
        && type != NodeType::CONST_VALUE) {
        throw "Cannot get value for node type " + to_string(type);
    }
    return value;
}

int Node::get_statement_id() {
    if (type != NodeType::READ
        && type != NodeType::PRINT
        && type != NodeType::CALL
        && type != NodeType::ASSIGN
        && type != NodeType::IF
        && type != NodeType::WHILE) {
        throw "Cannot get statement_id for node type " + to_string(type);
    }
    return statement_id;
}

std::vector<Node *> Node::get_children() {
    return children;
}

Node *Node::get_child(int i) {
    if (i > children.size()) {
        throw "Children with index " + to_string(i) + " doesn't exist";
    }
    return children[i];
}

void Node::add_child(Node *node) {
    children.push_back(node);
}

bool Node::is_subtree(Node *node) {
    throw "To be implemented";
}

std::string Node::to_string() {
    return to_string(0);
}

std::string Node::to_string(int padding) {
    std::string result;

    result += std::string(padding, ' ');
    result += "Node (type=";
    result += std::to_string(type);
    result += ", value=";
    result += value;
    result += ")\n";

    for (auto const &child : children) {
        result += child->to_string(padding + 4);
    }

    return result;
}
