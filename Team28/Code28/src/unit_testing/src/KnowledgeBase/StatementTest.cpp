#include "catch.hpp"

#include "KnowledgeBase/Statement.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include <memory>
#include <string>

using namespace KnowledgeBase;
using namespace Parser;

std::shared_ptr<SimpleNode> build_node() {
    auto assign_stmt = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 6);
    assign_stmt->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    auto assign_rhs = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "10"));
    assign_rhs->add_child(std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "y"));
    return assign_stmt;
}

TEST_CASE("KnowledgeBase::Statement") {
    SECTION("general") {
        // READ/PRINT
        StatementType stmt_type = StatementType::READ;
        int stmt_id = 10;
        std::string proc_name = "main";
        Statement stmt(stmt_type, stmt_id, proc_name);

        SECTION("get_type") { REQUIRE(stmt.get_type() == stmt_type); }

        SECTION("get_id") { REQUIRE(stmt.get_id() == stmt_id); }

        SECTION("get_procedure_name") { REQUIRE(stmt.get_procedure_name() == proc_name); }

        SECTION("set_get_parent") {
            REQUIRE(stmt.get_parent() == -1);
            int parent_id = 8;
            stmt.set_parent(parent_id);
            REQUIRE(stmt.get_parent() == parent_id);
        }

        SECTION("add_get_ancestors") {
            int ancestor_id = 8;
            stmt.add_ancestor(ancestor_id);
            auto ancestor_set = stmt.get_ancestors();
            REQUIRE(ancestor_set->size() == 1);
            REQUIRE(ancestor_set->count(ancestor_id) == 1);
            REQUIRE(ancestor_set->count(100) == 0);
        }

        SECTION("add_get_children") {
            int child_id = 12;
            REQUIRE_THROWS(stmt.add_child(child_id));
            REQUIRE(stmt.get_children()->size() == 0);
        }

        SECTION("add_get_descendants") {
            int descendant_id = 12;
            REQUIRE_THROWS(stmt.add_descendant(descendant_id));
            REQUIRE(stmt.get_descendants()->size() == 0);
        }

        SECTION("set_get_direct_following") {
            REQUIRE(stmt.get_direct_following() == -1);
            int direct_following_id = 9;
            stmt.set_direct_following(direct_following_id);
            REQUIRE(stmt.get_direct_following() == direct_following_id);
        }

        SECTION("add_get_followings") {
            int following_id = 9;
            stmt.add_following(following_id);
            auto following_set = stmt.get_followings();
            REQUIRE(following_set->size() == 1);
            REQUIRE(following_set->count(following_id) == 1);
            REQUIRE(following_set->count(100) == 0);
        }

        SECTION("set_get_direct_follower") {
            REQUIRE(stmt.get_direct_follower() == -1);
            int direct_follower_id = 11;
            stmt.set_direct_follower(direct_follower_id);
            REQUIRE(stmt.get_direct_follower() == direct_follower_id);
        }

        SECTION("add_get_followers") {
            int follower_id = 9;
            stmt.add_follower(follower_id);
            auto follower_set = stmt.get_followers();
            REQUIRE(follower_set->size() == 1);
            REQUIRE(follower_set->count(follower_id) == 1);
            REQUIRE(follower_set->count(100) == 0);
        }

        SECTION("add_get_direct_modifies") {
            std::string var_name = "direct_modifies";
            stmt.add_direct_modifies(var_name);
            auto direct_modifies_set = stmt.get_direct_modifies();
            REQUIRE(direct_modifies_set->size() == 1);
            REQUIRE(direct_modifies_set->count(var_name) == 1);
            REQUIRE(direct_modifies_set->count("a") == 0);
        }

        SECTION("add_get_modifies") {
            std::string var_name = "modifies";
            stmt.add_modifies(var_name);
            auto modifies_set = stmt.get_modifies();
            REQUIRE(modifies_set->size() == 1);
            REQUIRE(modifies_set->count(var_name) == 1);
            REQUIRE(modifies_set->count("a") == 0);
        }

        SECTION("add_get_direct_uses") {
            std::string var_name = "direct_uses";
            stmt.add_direct_uses(var_name);
            auto direct_uses_set = stmt.get_direct_uses();
            REQUIRE(direct_uses_set->size() == 1);
            REQUIRE(direct_uses_set->count(var_name) == 1);
            REQUIRE(direct_uses_set->count("a") == 0);
        }

        SECTION("add_get_uses") {
            std::string var_name = "uses";
            stmt.add_uses(var_name);
            auto uses_set = stmt.get_uses();
            REQUIRE(uses_set->size() == 1);
            REQUIRE(uses_set->count(var_name) == 1);
            REQUIRE(uses_set->count("a") == 0);
        }

        SECTION("set_get_procedure_called") {
            std::string proc_name = "proc_called";
            REQUIRE_THROWS(stmt.set_procedure_called(proc_name));
            REQUIRE_THROWS(stmt.get_procedure_called());
        }

        SECTION("set_get_pattern") {
            auto node = build_node();
            REQUIRE_THROWS(stmt.set_pattern(node));
            REQUIRE_THROWS(stmt.get_pattern());
        }

        SECTION("add_get_direct_next") {
            REQUIRE(stmt.get_direct_next()->size() == 0);
            int first_next_id = 11;
            int second_next_id = 12;
            stmt.add_direct_next(first_next_id);
            stmt.add_direct_next(second_next_id);
            REQUIRE(stmt.get_direct_next()->size() == 2);
            REQUIRE(stmt.get_direct_next()->count(first_next_id) == 1);
            REQUIRE(stmt.get_direct_next()->count(second_next_id) == 1);
            REQUIRE(stmt.get_direct_next()->count(100) == 0);
        }

        SECTION("add_get_next") {
            REQUIRE(stmt.get_next()->size() == 0);
            int first_next_id = 11;
            int second_next_id = 12;
            stmt.add_next(first_next_id);
            stmt.add_next(second_next_id);
            REQUIRE(stmt.get_next()->size() == 2);
            REQUIRE(stmt.get_next()->count(first_next_id) == 1);
            REQUIRE(stmt.get_next()->count(second_next_id) == 1);
            REQUIRE(stmt.get_next()->count(100) == 0);
        }

        SECTION("add_get_direct_previous") {
            REQUIRE(stmt.get_direct_previous()->size() == 0);
            int first_previous_id = 8;
            int second_previous_id = 9;
            stmt.add_direct_previous(first_previous_id);
            stmt.add_direct_previous(second_previous_id);
            REQUIRE(stmt.get_direct_previous()->size() == 2);
            REQUIRE(stmt.get_direct_previous()->count(first_previous_id) == 1);
            REQUIRE(stmt.get_direct_previous()->count(second_previous_id) == 1);
            REQUIRE(stmt.get_direct_previous()->count(100) == 0);
        }

        SECTION("add_get_previous") {
            REQUIRE(stmt.get_previous()->size() == 0);
            int first_previous_id = 8;
            int second_previous_id = 9;
            stmt.add_previous(first_previous_id);
            stmt.add_previous(second_previous_id);
            REQUIRE(stmt.get_previous()->size() == 2);
            REQUIRE(stmt.get_previous()->count(first_previous_id) == 1);
            REQUIRE(stmt.get_previous()->count(second_previous_id) == 1);
            REQUIRE(stmt.get_previous()->count(100) == 0);
        }
    }

    SECTION("assign") {
        StatementType stmt_type = StatementType::ASSIGN;
        int stmt_id = 10;
        std::string proc_name = "main";
        Statement stmt(stmt_type, stmt_id, proc_name);

        SECTION("set_get_pattern") {
            auto node = build_node();
            stmt.set_pattern(node);
            REQUIRE(stmt.get_pattern()->is_equal(node));
        }

        SECTION("add_get_direct_affects") {
            REQUIRE(stmt.get_direct_affects()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_direct_affects(first_id);
            stmt.add_direct_affects(second_id);
            REQUIRE(stmt.get_direct_affects()->size() == 2);
            REQUIRE(stmt.get_direct_affects()->count(first_id) == 1);
            REQUIRE(stmt.get_direct_affects()->count(second_id) == 1);
            REQUIRE(stmt.get_direct_affects()->count(100) == 0);
        }

        SECTION("add_get_affects") {
            REQUIRE(stmt.get_affects()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_affects(first_id);
            stmt.add_affects(second_id);
            REQUIRE(stmt.get_affects()->size() == 2);
            REQUIRE(stmt.get_affects()->count(first_id) == 1);
            REQUIRE(stmt.get_affects()->count(second_id) == 1);
            REQUIRE(stmt.get_affects()->count(100) == 0);
        }

        SECTION("add_get_direct_affected_by") {
            REQUIRE(stmt.get_direct_affected_by()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_direct_affected_by(first_id);
            stmt.add_direct_affected_by(second_id);
            REQUIRE(stmt.get_direct_affected_by()->size() == 2);
            REQUIRE(stmt.get_direct_affected_by()->count(first_id) == 1);
            REQUIRE(stmt.get_direct_affected_by()->count(second_id) == 1);
            REQUIRE(stmt.get_direct_affected_by()->count(100) == 0);
        }

        SECTION("add_get_affected_by") {
            REQUIRE(stmt.get_affected_by()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_affected_by(first_id);
            stmt.add_affected_by(second_id);
            REQUIRE(stmt.get_affected_by()->size() == 2);
            REQUIRE(stmt.get_affected_by()->count(first_id) == 1);
            REQUIRE(stmt.get_affected_by()->count(second_id) == 1);
            REQUIRE(stmt.get_affected_by()->count(100) == 0);
        }
    }

    SECTION("call") {
        StatementType stmt_type = StatementType::CALL;
        int stmt_id = 10;
        std::string proc_name = "main";
        Statement stmt(stmt_type, stmt_id, proc_name);

        SECTION("set_get_procedure_called") {
            std::string proc_name = "proc_called";
            stmt.set_procedure_called(proc_name);
            REQUIRE(stmt.get_procedure_called() == proc_name);
        }
    }

    SECTION("container") {
        // WHILE/IF
        StatementType stmt_type = StatementType::WHILE;
        int stmt_id = 10;
        std::string proc_name = "main";
        Statement stmt(stmt_type, stmt_id, proc_name);

        SECTION("add_get_children") {
            int child_id = 12;
            stmt.add_child(child_id);
            auto child_set = stmt.get_children();
            REQUIRE(child_set->size() == 1);
            REQUIRE(child_set->count(child_id) == 1);
            REQUIRE(child_set->count(100) == 0);
        }

        SECTION("add_get_descendants") {
            int descendant_id = 12;
            stmt.add_descendant(descendant_id);
            auto descendant_set = stmt.get_descendants();
            REQUIRE(descendant_set->size() == 1);
            REQUIRE(descendant_set->count(descendant_id) == 1);
            REQUIRE(descendant_set->count(100) == 0);
        }
    }

    SECTION("bip") {
        StatementType stmt_type = StatementType::ASSIGN;
        int stmt_id = 10;
        std::string proc_name = "main";
        Statement stmt(stmt_type, stmt_id, proc_name);

        SECTION("add_get_direct_next_bip") {
            REQUIRE(stmt.get_direct_next_bip()->size() == 0);
            int first_next_id = 11;
            int second_next_id = 12;
            stmt.add_direct_next_bip(first_next_id);
            stmt.add_direct_next_bip(second_next_id);
            REQUIRE(stmt.get_direct_next_bip()->size() == 2);
            REQUIRE(stmt.get_direct_next_bip()->count(first_next_id) == 1);
            REQUIRE(stmt.get_direct_next_bip()->count(second_next_id) == 1);
            REQUIRE(stmt.get_direct_next_bip()->count(100) == 0);
        }

        SECTION("add_get_next_bip") {
            REQUIRE(stmt.get_next_bip()->size() == 0);
            int first_next_id = 11;
            int second_next_id = 12;
            stmt.add_next_bip(first_next_id);
            stmt.add_next_bip(second_next_id);
            REQUIRE(stmt.get_next_bip()->size() == 2);
            REQUIRE(stmt.get_next_bip()->count(first_next_id) == 1);
            REQUIRE(stmt.get_next_bip()->count(second_next_id) == 1);
            REQUIRE(stmt.get_next_bip()->count(100) == 0);
        }

        SECTION("add_get_direct_previous_bip") {
            REQUIRE(stmt.get_direct_previous_bip()->size() == 0);
            int first_previous_id = 8;
            int second_previous_id = 9;
            stmt.add_direct_previous_bip(first_previous_id);
            stmt.add_direct_previous_bip(second_previous_id);
            REQUIRE(stmt.get_direct_previous_bip()->size() == 2);
            REQUIRE(stmt.get_direct_previous_bip()->count(first_previous_id) == 1);
            REQUIRE(stmt.get_direct_previous_bip()->count(second_previous_id) == 1);
            REQUIRE(stmt.get_direct_previous_bip()->count(100) == 0);
        }

        SECTION("add_get_previous_bip") {
            REQUIRE(stmt.get_previous_bip()->size() == 0);
            int first_previous_id = 8;
            int second_previous_id = 9;
            stmt.add_previous_bip(first_previous_id);
            stmt.add_previous_bip(second_previous_id);
            REQUIRE(stmt.get_previous_bip()->size() == 2);
            REQUIRE(stmt.get_previous_bip()->count(first_previous_id) == 1);
            REQUIRE(stmt.get_previous_bip()->count(second_previous_id) == 1);
            REQUIRE(stmt.get_previous_bip()->count(100) == 0);
        }

        SECTION("add_get_direct_affects_bip") {
            REQUIRE(stmt.get_direct_affects_bip()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_direct_affects_bip(first_id);
            stmt.add_direct_affects_bip(second_id);
            REQUIRE(stmt.get_direct_affects_bip()->size() == 2);
            REQUIRE(stmt.get_direct_affects_bip()->count(first_id) == 1);
            REQUIRE(stmt.get_direct_affects_bip()->count(second_id) == 1);
            REQUIRE(stmt.get_direct_affects_bip()->count(100) == 0);
        }

        SECTION("add_get_affects_bip") {
            REQUIRE(stmt.get_affects_bip()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_affects_bip(first_id);
            stmt.add_affects_bip(second_id);
            REQUIRE(stmt.get_affects_bip()->size() == 2);
            REQUIRE(stmt.get_affects_bip()->count(first_id) == 1);
            REQUIRE(stmt.get_affects_bip()->count(second_id) == 1);
            REQUIRE(stmt.get_affects_bip()->count(100) == 0);
        }

        SECTION("add_get_direct_affected_bip_by") {
            REQUIRE(stmt.get_direct_affected_bip_by()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_direct_affected_bip_by(first_id);
            stmt.add_direct_affected_bip_by(second_id);
            REQUIRE(stmt.get_direct_affected_bip_by()->size() == 2);
            REQUIRE(stmt.get_direct_affected_bip_by()->count(first_id) == 1);
            REQUIRE(stmt.get_direct_affected_bip_by()->count(second_id) == 1);
            REQUIRE(stmt.get_direct_affected_bip_by()->count(100) == 0);
        }

        SECTION("add_get_direct_affected_bip_by") {
            REQUIRE(stmt.get_direct_affected_bip_by()->size() == 0);
            int first_id = 8;
            int second_id = 9;
            stmt.add_affected_bip_by(first_id);
            stmt.add_affected_bip_by(second_id);
            REQUIRE(stmt.get_affected_bip_by()->size() == 2);
            REQUIRE(stmt.get_affected_bip_by()->count(first_id) == 1);
            REQUIRE(stmt.get_affected_bip_by()->count(second_id) == 1);
            REQUIRE(stmt.get_affected_bip_by()->count(100) == 0);
        }
    }
}
