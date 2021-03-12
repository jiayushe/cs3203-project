#pragma once

#include "PQLLexer.h"
#include "Parser/shared/BaseParser.h"
#include "Parser/shared/Source.h"
#include "Parser/shared/Token.h"
#include "QueryObject.h"
#include <functional>
#include <map>
#include <vector>

namespace Parser {
class PQLParser : public BaseParser {
public:
    explicit PQLParser(std::shared_ptr<Source> source);

    QueryObject parse_query();

private:
    std::shared_ptr<Source> source;
    QueryObject query_object;

    // Parsing logic
    void process_declaration_synonym(DesignEntityType design_entity_type);
    void process_declaration();
    void process_selection();
    void process_such_that();
    void process_such_that_condition();
    void process_such_that_body(SuchThatType such_that_type);
    void process_pattern();
    void process_pattern_condition();
    void process_pattern_assign();
    void process_pattern_while();
    void process_pattern_if();
    void process_with();
    void process_with_condition();

    std::vector<Elem> process_selection_tuple();
    Elem process_selection_elem();
    SuchThatRef process_such_that_left_ref(SuchThatType such_that_type);
    SuchThatRef process_such_that_right_ref(SuchThatType such_that_type);
    WithRef process_with_ref();

    ExpressionSpec process_expression_spec();
    StatementRef process_statement_ref();
    EntityRef process_entity_ref();
    void choice(const std::vector<std::function<void()>>& parse_funcs, std::string error_message);
    void repeat(const std::function<void()>& parse_func);

    // Validation logic
    void validate_query();
    void validate_result(const Result& result);
    void validate_such_that(const SuchThat& such_that);
    void validate_pattern(const Pattern& pattern);
    void validate_with(const With& with);
    static int get_with_ref_data_type(const WithRef& with_ref);
    void validate_elem(const Elem& elem);
    void validate_statement_ref(const StatementRef& statement_ref);
    void validate_statement_ref(const StatementRef& statement_ref,
                                const std::vector<DesignEntityType>& expected_types);
    void validate_entity_ref(const EntityRef& entity_ref,
                             const std::vector<DesignEntityType>& expected_types);
    void validate_with_ref(const WithRef& with_ref);
    void validate_attr_ref(const AttrRef& attr_ref);
    DesignEntity expect_declaration(const std::string& synonym);
    DesignEntity expect_declaration(const std::string& synonym,
                                    const std::vector<DesignEntityType>& expected_types);
};

} // namespace Parser