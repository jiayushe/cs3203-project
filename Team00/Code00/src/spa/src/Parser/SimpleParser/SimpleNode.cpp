#include "SimpleNode.h"

using namespace Parser;

SimpleNode::SimpleNode(SimpleNodeType type)
    : type(type), value(""), statement_id(-1), children(std::vector<SimpleNode*>()) {}

SimpleNode::SimpleNode(SimpleNodeType type, std::string value)
    : type(type), value(value), statement_id(-1), children(std::vector<SimpleNode*>()) {}

SimpleNode::SimpleNode(SimpleNodeType type, int statement_id)
    : type(type), value(""), statement_id(statement_id), children(std::vector<SimpleNode*>()) {}

SimpleNode::~SimpleNode() {
    for (auto const child : children) {
        delete child;
    }
}

SimpleNodeType SimpleNode::get_type() { return type; }

bool SimpleNode::has_value() {
    return type == SimpleNodeType::CONDITIONAL || type == SimpleNodeType::ARITHMETIC ||
           type == SimpleNodeType::VAR_NAME || type == SimpleNodeType::PROC_NAME ||
           type == SimpleNodeType::CONST_VALUE;
}

std::string SimpleNode::get_value() {
    if (!has_value()) {
        throw "Cannot get value for node type " + to_string(type);
    }
    return value;
}

bool SimpleNode::has_statement_id() {
    return type == SimpleNodeType::READ || type == SimpleNodeType::PRINT || type == SimpleNodeType::CALL ||
           type == SimpleNodeType::ASSIGN || type == SimpleNodeType::IF || type == SimpleNodeType::WHILE;
}

int SimpleNode::get_statement_id() {
    if (!has_statement_id()) {
        throw "Cannot get statement_id for node type " + to_string(type);
    }
    return statement_id;
}

std::vector<SimpleNode*> SimpleNode::get_children() { return children; }

SimpleNode* SimpleNode::get_child(int i) {
    if (i < 0 || i >= children.size()) {
        throw "Children with index " + to_string(i) + " doesn't exist";
    }
    return children[i];
}

void SimpleNode::add_child(SimpleNode* node) { children.push_back(node); }

bool SimpleNode::is_subtree(SimpleNode* node) {
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

bool SimpleNode::is_equal(SimpleNode* node) {
    if (node == nullptr) {
        return false;
    }

    if (type != node->type || value != node->value || statement_id != node->statement_id ||
        children.size() != node->children.size()) {
        return false;
    }

    for (int i = 0; i < children.size(); i++) {
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

    for (auto const& child : children) {
        result += child->to_string(padding + 4);
    }

    return result;
}
