#include "catch.hpp"

#include "SimpleParser/Node.h"

using namespace SimpleParser;

// Helper function to build an AST representing "x + y * 5"
Node *build_test_node() {
    auto node = new Node(NodeType::ARITHMETIC, "+");
    node->add_child(new Node(NodeType::VAR_NAME, "x"));
    node->add_child(new Node(NodeType::ARITHMETIC, "*"));
    node->get_child(1)->add_child(new Node(NodeType::VAR_NAME, "y"));
    node->get_child(1)->add_child(new Node(NodeType::CONST_VALUE, "5"));
    return node;
}

TEST_CASE("SimpleParser::Node") {
    auto node = build_test_node();

    SECTION("is_subtree") {
        SECTION("return false if null pointer is supplied") {
            REQUIRE_FALSE(node->is_subtree(NULL));
            REQUIRE_FALSE(node->is_subtree(nullptr));
        }

        SECTION("return true if the supplied node is a subtree") {
            auto identical_node = build_test_node();

            // "x + y * 5" is a subtree of "x + y * 5" (reflexive)
            REQUIRE(node->is_subtree(identical_node));

            // "x" is a subtree of "x + y * 5"
            REQUIRE(node->is_subtree(identical_node->get_child(0)));

            // "y * 5" is a subtree of "x + y * 5"
            REQUIRE(node->is_subtree(identical_node->get_child(1)));

            // "y" is a subtree of "x + y * 5"
            REQUIRE(node->is_subtree(identical_node->get_child(1)->get_child(0)));

            // "5" is a subtree of "x + y * 5"
            REQUIRE(node->is_subtree(identical_node->get_child(1)->get_child(1)));

            delete identical_node;
        }

        SECTION("return false if the supplied node is not a subtree") {
            // Build an AST representing "x + y"
            auto non_subtree_node = new Node(NodeType::ARITHMETIC, "+");
            non_subtree_node->add_child(new Node(NodeType::VAR_NAME, "x"));
            non_subtree_node->add_child(new Node(NodeType::VAR_NAME, "y"));

            // "x + y" is not a subtree of "x + y * 5"
            REQUIRE_FALSE(node->is_subtree(non_subtree_node));

            delete non_subtree_node;
        }
    }

    SECTION("is_equal") {
        SECTION("return false if null pointer is supplied") {
            REQUIRE_FALSE(node->is_equal(NULL));
            REQUIRE_FALSE(node->is_equal(nullptr));
        }

        SECTION("return true if the supplied node is equal (recursively)") {
            auto identical_node = build_test_node();
            REQUIRE(node->is_equal(identical_node));
            delete identical_node;
        }

        SECTION("return false if the supplied node is not equal (recursively)") {
            // Build an AST representing x + y
            auto different_node = new Node(NodeType::ARITHMETIC, "+");
            different_node->add_child(new Node(NodeType::VAR_NAME, "x"));
            different_node->add_child(new Node(NodeType::VAR_NAME, "y"));

            // "x + y" is not equal to "x + y * 5"
            REQUIRE_FALSE(node->is_equal(different_node));

            delete different_node;
        }
    }

    delete node;
}
