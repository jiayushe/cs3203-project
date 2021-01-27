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

bool Node::has_value() {
    return type == NodeType::CONDITIONAL
           || type == NodeType::ARITHMETIC
           || type == NodeType::VAR_NAME
           || type == NodeType::PROC_NAME
           || type == NodeType::CONST_VALUE;
}

std::string Node::get_value() {
    if (!has_value()) {
        throw "Cannot get value for node type " + to_string(type);
    }
    return value;
}

bool Node::has_statement_id() {
    return type == NodeType::READ
           || type == NodeType::PRINT
           || type == NodeType::CALL
           || type == NodeType::ASSIGN
           || type == NodeType::IF
           || type == NodeType::WHILE;
}

int Node::get_statement_id() {
    if (!has_statement_id()) {
        throw "Cannot get statement_id for node type " + to_string(type);
    }
    return statement_id;
}

std::vector<Node *> Node::get_children() {
    return children;
}

Node *Node::get_child(int i) {
    if (i < 0 || i >= children.size()) {
        throw "Children with index " + to_string(i) + " doesn't exist";
    }
    return children[i];
}

void Node::add_child(Node *node) {
    children.push_back(node);
}

bool Node::is_subtree(Node *node) {
    if (node == nullptr) {
        return false;
    }

    if (is_equal(node)) {
        return true;
    }

    for (auto const child : children) {
        if (child->is_subtree(node)) {
            return true;
        }
    }

    return false;
}

bool Node::is_equal(Node *node) {
    if (node == nullptr) {
        return false;
    }

    if (type != node->type
        || value != node->value
        || statement_id != node->statement_id
        || children.size() != node->children.size()) {
        return false;
    }

    for (int i = 0; i < children.size(); i++) {
        if (!children[i]->is_equal(node->children[i])) {
            return false;
        }
    }

    return true;
}

std::string Node::to_string() {
    return to_string(0);
}

std::string Node::to_string(int padding) {
    std::string result;

    result += std::string(padding, ' ');
    result += "Node (type=";
    result += std::to_string(type);

    if (has_value()) {
        result += ", value=";
        result += value;
    }

    if (has_statement_id()) {
        result += ", statement_id=";
        result += std::to_string(statement_id);
    }

    result += ")\n";

    for (auto const &child : children) {
        result += child->to_string(padding + 4);
    }

    return result;
}
