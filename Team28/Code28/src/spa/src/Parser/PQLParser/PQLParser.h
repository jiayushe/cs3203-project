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

    void parse_query(QueryObject& query_object);
    QueryObject parse_query();

private:
    std::shared_ptr<Source> source;

    // Parsing logic
    void process_declaration_synonym(QueryObject& query_object,
                                     DesignEntityType design_entity_type);
    void process_declaration(QueryObject& query_object);
    void process_selection(QueryObject& query_object);
    void process_such_that(QueryObject& query_object);
    void process_such_that_condition(QueryObject& query_object);
    void process_such_that_body(QueryObject& query_object, SuchThatType such_that_type);
    void process_pattern(QueryObject& query_object);
    void process_pattern_condition(QueryObject& query_object);
    void process_pattern_assign(QueryObject& query_object);
    void process_pattern_while(QueryObject& query_object);
    void process_pattern_if(QueryObject& query_object);
    void process_with(QueryObject& query_object);
    void process_with_condition(QueryObject& query_object);

    std::vector<Elem> process_selection_tuple();
    Elem process_selection_elem();
    SuchThatRef process_such_that_left_ref(const QueryObject& query_object,
                                           SuchThatType such_that_type);
    SuchThatRef process_such_that_right_ref(SuchThatType such_that_type);
    WithRef process_with_ref();

    ExpressionSpec process_expression_spec();
    StatementRef process_statement_ref();
    EntityRef process_entity_ref();
    void choice(const std::vector<std::function<void()>>& parse_funcs, std::string error_message);
    void repeat(const std::function<void()>& parse_func);

    // Validation logic
    static void validate_query(const QueryObject& query_object);
    static void validate_result(const QueryObject& query_object, const Result& result);
    static void validate_such_that(const QueryObject& query_object, const SuchThat& such_that);
    static void validate_pattern(const QueryObject& query_object, const Pattern& pattern);
    static void validate_with(const QueryObject& query_object, const With& with);
    static int get_with_ref_data_type(const WithRef& with_ref);
    static void validate_elem(const QueryObject& query_object, const Elem& elem);
    static void validate_statement_ref(const QueryObject& query_object,
                                       const StatementRef& statement_ref);
    static void validate_statement_ref(const QueryObject& query_object,
                                       const StatementRef& statement_ref,
                                       const std::vector<DesignEntityType>& expected_types);
    static void validate_entity_ref(const QueryObject& query_object, const EntityRef& entity_ref,
                                    const std::vector<DesignEntityType>& expected_types);
    static void validate_with_ref(const QueryObject& query_object, const WithRef& with_ref);
    static void validate_attr_ref(const QueryObject& query_object, const AttrRef& attr_ref);
    static DesignEntity expect_declaration(const QueryObject& query_object,
                                           const std::string& synonym);
    static DesignEntity expect_declaration(const QueryObject& query_object,
                                           const std::string& synonym,
                                           const std::vector<DesignEntityType>& expected_types);
};

} // namespace Parser