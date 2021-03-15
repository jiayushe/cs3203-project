#pragma once

#include "BaseConstraintLogic.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/AttrRef.h"
#include "Parser/PQLParser/WithRef.h"
#include <memory>
#include <string>
#include <unordered_set>

namespace QueryEvaluator {

class WithConstraintLogic : public BaseConstraintLogic {
public:
    WithConstraintLogic(std::shared_ptr<KnowledgeBase::PKB> pkb, const Parser::WithRef& lhs,
                        const Parser::WithRef& rhs);

    bool is_valid(const AssignmentMap& assignment_map) const override;
    std::unordered_set<std::string> get_synonyms() const override;

private:
    Parser::WithRef lhs;
    Parser::WithRef rhs;
    std::unordered_set<std::string> synonyms;
    static bool is_string_value(const Parser::WithRef& with_ref);
    static bool is_integer_value(const Parser::WithRef& with_ref);
    static std::string get_string_value(const AssignmentMap& assignment_map,
                                        const Parser::WithRef& with_ref);
    static int get_integer_value(const AssignmentMap& assignment_map,
                                 const Parser::WithRef& with_ref);
};

} // namespace QueryEvaluator
