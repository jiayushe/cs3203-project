#ifndef PQLPARSER_PARSER_H
#define PQLPARSER_PARSER_H

#include "QueryObject.h"
#include "TokenList.h"

namespace PQLParser {
    class Parser {
    public:
        Parser(std::string tmp_query);

        ~Parser();

        QueryObject *parse_query();

        void process_declaration(QueryObject *query_object, TokenList *tokens, DesignEntityType design_entity_type);

        void expect_token(TokenType given_type, TokenType expected_type);

        void expect_token(std::string given_string, std::string expected_string);

        SuchThat process_such_that_cl(TokenList *tokens);

        Pattern process_pattern_cl(TokenList *tokens);

        StatementRef process_statement_ref(std::string statement_string);

        EntityRef process_entity_ref(std::string statement_string);

    private:
        std::string query;

    };

}

#endif //PQLPARSER_PARSER_H
