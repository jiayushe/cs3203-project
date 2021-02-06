#pragma once

#include "DesignEntity.h"
#include "Pattern.h"
#include "SuchThat.h"
#include <unordered_map>

namespace Parser {
// AST node for the SIMPLE language.
class QueryObject {
public:
    QueryObject();

    // Function
    void add_declaration(const std::string& synonym, const DesignEntity& design_entity);

    // getters
    std::unordered_map<std::string, DesignEntity> get_declarations();
    bool has_such_that();
    bool has_pattern();

    std::string get_selection();
    SuchThat get_such_that();
    Pattern get_pattern();

    // setters
    void set_selection(const std::string& selection);
    void set_has_such_that(bool has_such_that);
    void set_has_pattern(bool has_pattern);

    void set_such_that(const SuchThat& such_that_cl);
    void set_pattern(const Pattern& pattern_cl);

    // this is just for view purpose
    void to_string();

private:
    std::unordered_map<std::string, DesignEntity> declarations;
    std::string selection;

    bool has_such_that_cl;
    bool has_pattern_cl;

    SuchThat such_that_obj;
    Pattern pattern_obj;
};
} // namespace Parser
