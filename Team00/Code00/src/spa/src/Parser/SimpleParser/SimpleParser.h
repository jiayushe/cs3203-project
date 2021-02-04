#pragma once

#include "Parser/Lexer.h"
#include "SimpleNode.h"
#include "Parser/Token.h"
#include "Parser/TokenList.h"
#include <functional>

namespace Parser {

// SimpleParser for the SIMPLE language (converts tokens to AST).
class SimpleParser {
public:
    explicit SimpleParser(Lexer& lexer);

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
    TokenList* tokens;
    int next_statement_id;

    // Helper for choosing one valid parser from parse_funcs.
    // Validity is decided based on whether the chosen parser throws an error.
    // Useful for building a disjunctive parsing logic (see parse_cond_expr for an example).
    SimpleNode* choice(const std::vector<std::function<SimpleNode*()>>& parse_funcs, std::string error_message);

    // Helper for repeatedly calling parse_func until it throws an error.
    // Useful for building an iterative parsing logic (see parse_stmt_lst for an example).
    SimpleNode* repeat(const std::function<SimpleNode*(SimpleNode*)>& parse_func, SimpleNode* initial_node);

    // Registers that the next token should be of the expected type.
    // Throws if the expectation is not fulfilled.
    Token* expect_token(TokenType expected_type);

    // Registers that the next token should be any of the expected types.
    // Throws if the expectation is not fulfilled.
    Token* expect_token(const std::vector<TokenType>& expected_types);

    // Registers that the next token should be a WORD token with the expected value.
    // Throws if the expectation is not fulfilled.
    Token* expect_word(const std::string& expected_value);

    // Registers that the next token should be a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    // Throws if the expectation is not fulfilled.
    Token* expect_name(const std::string& identifier);

    // Registers that the next token should be a WORD token whose value is an integer
    // (a numeric string).
    // Throws if the expectation is not fulfilled.
    Token* expect_integer(const std::string& identifier);

    // Checks if the given token type matches with the expected type.
    static bool is_token_type(Token* token, TokenType expected_type);

    // Checks if the given token type matches with any of the expected types.
    static bool is_word(Token* token, const std::string& expected_value);

    // Checks if the given token is a WORD token whose value is a name
    // (an alphanumeric string which starts with an alphabet).
    static bool is_name(Token* token);

    // Checks if the given token is a WORD token whose value is an integer
    // (a numeric string).
    static bool is_integer(Token* token);

    static std::string to_string(const std::vector<TokenType>& token_types);
};

}
