#pragma once

#include "KnowledgeBase/PKB.h"

namespace SimpleExtractor {

class DesignExtractor {
public:
    // Extracts modify relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_modify_relationship(KnowledgeBase::PKB pkb);

    // Extracts use relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_use_relationship(KnowledgeBase::PKB pkb);

    // Extracts follow relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_follow_relationship(KnowledgeBase::PKB pkb);

    // Extracts parent relationships from the given PKB.
    // Throws if the given PKB has an invalid AST.
    static void extract_parent_relationship(KnowledgeBase::PKB pkb);

private:
    static void
    extract_modify_relationship_from_stmt_list(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt_list);
    static void extract_modify_relationship_from_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                      std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_read_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                               std::shared_ptr<Parser::SimpleNode> stmt);
    static void
    extract_modify_relationship_from_assign_stmt(KnowledgeBase::PKB pkb, std::string proc_name,
                                                 std::shared_ptr<Parser::SimpleNode> stmt);
};

} // namespace SimpleExtractor
