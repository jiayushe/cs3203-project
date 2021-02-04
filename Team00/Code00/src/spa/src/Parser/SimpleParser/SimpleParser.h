#pragma once

#include "Parser/shared/BaseLexer.h"
#include "Parser/shared/BaseParser.h"
#include "Parser/shared/Token.h"
#include "Parser/shared/TokenList.h"
#include "SimpleNode.h"
#include <functional>

namespace Parser {

// SimpleParser for the SIMPLE language (converts tokens to AST).
class SimpleParser : public BaseParser {
public:
    // Create a SimpleParser instance from the given lexer instance.
    explicit SimpleParser(BaseLexer& lexer);

    // Create a SimpleParser instance from the given token list.
    // Caller is responsible for cleaning up the memory for the supplied token list.
    explicit SimpleParser(TokenList* tokens);

    ~SimpleParser();

    // The methods below correspond 1-to-1 to the grammar rule for the SIMPLE language.
    // For example, parse_stmt is called when we're parsing a SIMPLE "stmt".
    // See
    // https://github.com/nus-cs3203/project-wiki/wiki/Project-Iteration-1#21-implement-the-almost-full-specification-of-simple

    SimpleNode* parse_program();

    SimpleNode* parse_procedure();

    SimpleNode* parse_stmt_lst();

    SimpleNode* parse_stmt();

    SimpleNode* parse_read();

    SimpleNode* parse_print();

    SimpleNode* parse_while();

    SimpleNode* parse_if();

    SimpleNode* parse_call();

    SimpleNode* parse_assign();

    SimpleNode* parse_cond_expr();

    SimpleNode* parse_rel_expr();

    SimpleNode* parse_rel_factor();

    SimpleNode* parse_expr();

    SimpleNode* parse_term();

    SimpleNode* parse_factor();

    SimpleNode* parse_var_name();

    SimpleNode* parse_proc_name();

    SimpleNode* parse_const_value();

private:
    int next_statement_id;
    bool should_delete_token_list;

    // Helper for choosing one valid parser from parse_funcs.
    // Validity is decided based on whether the chosen parser throws an error.
    // Useful for building a disjunctive parsing logic (see parse_cond_expr for an example).
    SimpleNode* choice(const std::vector<std::function<SimpleNode*()>>& parse_funcs,
                       std::string error_message);

    // Helper for repeatedly calling parse_func until it throws an error.
    // Useful for building an iterative parsing logic (see parse_stmt_lst for an example).
    SimpleNode* repeat(const std::function<SimpleNode*(SimpleNode*)>& parse_func,
                       SimpleNode* initial_node);
};

} // namespace Parser
