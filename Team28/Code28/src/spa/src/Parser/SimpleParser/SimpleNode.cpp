#include "SimpleNode.h"
#include <algorithm>
#include <stdexcept>
#include <string>

using namespace Parser;

SimpleNode::SimpleNode(SimpleNodeType type)
    : type(type), statement_id(-1), children(std::vector<std::shared_ptr<SimpleNode>>()) {}

SimpleNode::SimpleNode(SimpleNodeType type, const std::string& value)
    : type(type), value(value), statement_id(-1),
      children(std::vector<std::shared_ptr<SimpleNode>>()) {}

SimpleNode::SimpleNode(SimpleNodeType type, int statement_id)
    : type(type), statement_id(statement_id), children(std::vector<std::shared_ptr<SimpleNode>>()) {
}

SimpleNodeType SimpleNode::get_type() { return type; }

bool SimpleNode::has_value() {
    return type == SimpleNodeType::CONDITIONAL || type == SimpleNodeType::ARITHMETIC ||
           type == SimpleNodeType::VAR_NAME || type == SimpleNodeType::PROC_NAME ||
           type == SimpleNodeType::CONST_VALUE;
}

std::string SimpleNode::get_value() {
    if (!has_value()) {
        throw std::runtime_error("Cannot get value for node type " + Parser::to_string(type));
    }
    return value;
}

bool SimpleNode::has_statement_id() {
    return type == SimpleNodeType::READ || type == SimpleNodeType::PRINT ||
           type == SimpleNodeType::CALL || type == SimpleNodeType::ASSIGN ||
           type == SimpleNodeType::IF || type == SimpleNodeType::WHILE;
}

int SimpleNode::get_statement_id() {
    if (!has_statement_id()) {
        throw std::runtime_error("Cannot get statement_id for node type " +
                                 Parser::to_string(type));
    }
    return statement_id;
}

std::vector<std::shared_ptr<SimpleNode>> SimpleNode::get_children() { return children; }

std::shared_ptr<SimpleNode> SimpleNode::get_child(int i) {
    if (i < 0 || i >= (int)children.size()) {
        throw std::runtime_error("Children with index " + to_string(i) + " doesn't exist");
    }
    return children[i];
}

void SimpleNode::add_child(std::shared_ptr<SimpleNode> node) { children.push_back(node); }

bool SimpleNode::is_subtree(std::shared_ptr<SimpleNode> node) {
    if (node == nullptr) {
        return false;
    }

    if (is_equal(node)) {
        return true;
    }

    return std::any_of(children.begin(), children.end(),
                       [&](std::shared_ptr<SimpleNode> child) { return child->is_subtree(node); });
}

bool SimpleNode::is_equal(std::shared_ptr<SimpleNode> node) {
    if (node == nullptr) {
        return false;
    }

    if (type != node->type || value != node->value || statement_id != node->statement_id ||
        children.size() != node->children.size()) {
        return false;
    }

    for (int i = 0; i < (int)children.size(); i++) {
        if (!children[i]->is_equal(node->children[i])) {
            return false;
        }
    }

    return true;
}

std::string SimpleNode::to_string() { return to_string(0); }

std::string SimpleNode::to_string(int padding) {
    std::string result;

    result += std::string(padding, ' ');
    result += "SimpleNode (type=";
    result += Parser::to_string(type);

    if (has_value()) {
        result += ", value=";
        result += value;
    }

    if (has_statement_id()) {
        result += ", statement_id=";
        result += std::to_string(statement_id);
    }

    result += ")\n";

    for (auto const& child : children) {
        result += child->to_string(padding + 4);
    }

    return result;
}

std::string Parser::to_string(SimpleNodeType simple_node_type) {
    switch (simple_node_type) {
    case SimpleNodeType::INVALID:
        return "INVALID";
    case SimpleNodeType::PROGRAM:
        return "PROGRAM";
    case SimpleNodeType::PROCEDURE:
        return "PROCEDURE";
    case SimpleNodeType::STMT_LST:
        return "STMT_LST";
    case SimpleNodeType::READ:
        return "READ";
    case SimpleNodeType::PRINT:
        return "PRINT";
    case SimpleNodeType::CALL:
        return "CALL";
    case SimpleNodeType::WHILE:
        return "WHILE";
    case SimpleNodeType::IF:
        return "IF";
    case SimpleNodeType::ASSIGN:
        return "ASSIGN";
    case SimpleNodeType::CONDITIONAL:
        return "CONDITIONAL";
    case SimpleNodeType::ARITHMETIC:
        return "ARITHMETIC";
    case SimpleNodeType::VAR_NAME:
        return "VAR_NAME";
    case SimpleNodeType::PROC_NAME:
        return "PROC_NAME";
    case SimpleNodeType::CONST_VALUE:
        return "CONST_VALUE";
    default:
        throw std::runtime_error("Unknown simple node type");
    }
}
