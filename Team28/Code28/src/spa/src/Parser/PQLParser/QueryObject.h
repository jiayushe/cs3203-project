#pragma once

#include "DeclarationMap.h"
#include "DesignEntity.h"
#include "Pattern.h"
#include "Result.h"
#include "SuchThat.h"
#include "With.h"
#include <string>
#include <vector>

namespace Parser {

class QueryObject {
public:
    QueryObject();

    // getters
    DeclarationMap get_declarations() const;
    Result get_result() const;
    std::vector<SuchThat> get_all_such_that() const;
    std::vector<Pattern> get_all_pattern() const;
    std::vector<With> get_all_with() const;

    // setters
    void add_declaration(const std::string& synonym, const DesignEntity& design_entity);
    void set_result(const Result& result);
    void add_such_that(const SuchThat& such_that);
    void add_pattern(const Pattern& pattern);
    void add_with(const With& with);

    // this is just for view purpose
    std::string to_string() const;

private:
    DeclarationMap declarations;
    Result result;

    std::vector<SuchThat> all_such_that;
    std::vector<Pattern> all_pattern;
    std::vector<With> all_with;
};

} // namespace Parser
