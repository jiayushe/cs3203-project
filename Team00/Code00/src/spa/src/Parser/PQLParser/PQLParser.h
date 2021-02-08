#pragma once

#include "Parser/shared/BaseLexer.h"
#include "Parser/shared/BaseParser.h"
#include "Parser/shared/Token.h"
#include "Parser/shared/TokenList.h"
#include "QueryObject.h"
#include <map>
#include <vector>
#include <functional>

namespace Parser {
class PQLParser : public BaseParser {
public:
    explicit PQLParser(BaseLexer& lexer);

    QueryObject parse_query();

    void process_declaration_synonym(DesignEntityType design_entity_type);

    void process_declaration();

    void process_selection();

    void process_such_that_cl();

    void process_such_that_body(SuchThatType such_that_type);

    void process_pattern_cl();

    ExpressionSpec process_expression_spec();

    StatementRef process_statement_ref();

    EntityRef process_entity_ref();

private:
    QueryObject query_object;

    void choice(const std::vector<std::function<void()>>& parse_funcs, std::string error_message);

    void repeat(const std::function<void()>& parse_func);
};

} // namespace Parser