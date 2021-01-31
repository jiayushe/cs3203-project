//
// Created by Victor Varian on 27/1/21.
//

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

        void process_declaration(QueryObject *queryObject, TokenList *tokens, DesignEntityType designEntityType);

        void process_such_that_cl(QueryObject *queryObject, TokenList *tokens);

        void *expect_token(TokenType given_type, TokenType expected_type);

        StatementRef process_statement_ref(std::string statement_string);

        EntityRef process_entity_ref(std::string statement_string);

    private:
        std::string query;

    };

}

#endif //PQLPARSER_PARSER_H
