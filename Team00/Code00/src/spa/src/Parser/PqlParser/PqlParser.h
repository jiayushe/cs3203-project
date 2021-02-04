#pragma once

#include <map>
#include "Parser/shared/BaseLexer.h"
#include "Parser/shared/BaseParser.h"
#include "Parser/shared/Token.h"
#include "Parser/shared/TokenList.h"
#include "QueryObject.h"

namespace Parser {
    class PQLParser : public BaseParser {
    public:
        PQLParser(QueryObject* queryObject, TokenList* tokens);

        ~PQLParser();

        QueryObject* parse_query();

        void process_declaration(std::map<std::string, DesignEntityType> design_entity_map);

        void process_selection();

        void process_such_that_cl();

        void process_pattern_cl();

        ExpressionSpec process_expression_spec();

        StatementRef process_statement_ref();

        EntityRef process_entity_ref();

    private:
        QueryObject* query_object;

        void choice(const std::vector<std::function<void()>>& parse_funcs,
                           std::string error_message);
    };

} // namespace Parser
