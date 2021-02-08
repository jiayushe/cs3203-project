#pragma once

#include "DeclarationMap.h"
#include "DesignEntity.h"
#include "Pattern.h"
#include "SuchThat.h"

namespace Parser {
// AST node for the SIMPLE language.
class QueryObject {
public:
    QueryObject();

    // getters
    std::string get_selection() const;
    DeclarationMap get_declarations() const;
    bool has_such_that() const;
    bool has_pattern() const;
    SuchThat get_such_that() const;
    Pattern get_pattern() const;

    // setters
    void set_selection(const std::string& selection);
    void add_declaration(const std::string& synonym, const DesignEntity& design_entity);
    void set_has_such_that(bool has_such_that);
    void set_has_pattern(bool has_pattern);
    void set_such_that(const SuchThat& such_that_cl);
    void set_pattern(const Pattern& pattern_cl);

    // this is just for view purpose
    void to_string();

private:
    DeclarationMap declarations;
    std::string selection;

    bool has_such_that_cl;
    bool has_pattern_cl;

    SuchThat such_that_obj;
    Pattern pattern_obj;
};
} // namespace Parser
