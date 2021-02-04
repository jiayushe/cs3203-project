#pragma once

#include "Parser/TokenList.h"
#include "QueryObject.h"

namespace Parser {
class PqlParser {
public:
    Parser(std::string tmp_query);

    ~Parser();

    QueryObject* parse_query();

    void process_declaration(QueryObject* query_object, TokenList* tokens,
                             DesignEntityType design_entity_type);

    void expect_token(TokenType given_type, TokenType expected_type);

    SuchThat process_such_that_cl(TokenList* tokens);

    Pattern process_pattern_cl(TokenList* tokens);

    StatementRef process_statement_ref(std::string statement_string);

    EntityRef process_entity_ref(std::string statement_string);

private:
    std::string query;
};

} // namespace Parser
