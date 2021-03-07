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
    void process_such_that_body(SuchThatType such_that_type);
    void process_pattern();
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
    void validate_elem(const Elem& elem);
    void validate_statement_ref(const StatementRef& statement_ref);
    void validate_statement_ref(const StatementRef& statement_ref,
                                const std::vector<DesignEntityType>& expected_types);
    void validate_entity_ref(const EntityRef& entity_ref);
    void validate_entity_ref(const EntityRef& entity_ref,
                             const std::vector<DesignEntityType>& expected_types);
    DesignEntity expect_declaration(const std::string& synonym);
    DesignEntity expect_declaration(const std::string& synonym,
                                    const std::vector<DesignEntityType>& expected_types);
};

} // namespace Parser